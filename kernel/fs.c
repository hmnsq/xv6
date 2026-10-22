// File system implementation.  Five layers:
//   + Blocks: allocator for raw disk blocks.
//   + Log: crash recovery for multi-step updates.
//   + Files: inode allocator, reading, writing, metadata.
//   + Directories: inode with special contents (list of other inodes!)
//   + Names: paths like /usr/rtm/xv6/fs.c for convenient naming.
//
// This file contains the low-level file system manipulation
// routines.  The (higher-level) system call implementations
// are in sysfile.c.

#ಅನ್ತರ್ಭಾವ "types.h"
#ಅನ್ತರ್ಭಾವ "riscv.h"
#ಅನ್ತರ್ಭಾವ "defs.h"
#ಅನ್ತರ್ಭಾವ "param.h"
#ಅನ್ತರ್ಭಾವ "stat.h"
#ಅನ್ತರ್ಭಾವ "spinlock.h"
#ಅನ್ತರ್ಭಾವ "proc.h"
#ಅನ್ತರ್ಭಾವ "sleeplock.h"
#ಅನ್ತರ್ಭಾವ "fs.h"
#ಅನ್ತರ್ಭಾವ "buf.h"
#ಅನ್ತರ್ಭಾವ "file.h"

#ವ್ಯಾಖ್ಯಾನಿಸು min(a, b) ((a) < (b) ? (a) : (b))
// there should be one superblock per disk device, but we run with
// only one device
ರಚನೆ superblock sb;

// Read the super block.
ಸ್ಥಿರ ಶೂನ್ಯ
readsb(ಪೂರ್ಣಾಂಕ dev, ರಚನೆ superblock *sb)
{
  ರಚನೆ buf *bp;

  bp = bread(dev, 1);
  memmove(sb, bp->ದತ್ತಾಂಶ, ಗಾತ್ರ(*sb));
  brelse(bp);
}

// Init fs
ಶೂನ್ಯ
fsinit(ಪೂರ್ಣಾಂಕ dev)
{
  readsb(dev, &sb);
  ಆದರೆ (sb.magic != FSMAGIC)
    panic("invalid file system");
  initlog(dev, &sb);
  ireclaim(dev);
}

// Zero a block.
ಸ್ಥಿರ ಶೂನ್ಯ
bzero(ಪೂರ್ಣಾಂಕ dev, ಪೂರ್ಣಾಂಕ bno)
{
  ರಚನೆ buf *bp;

  bp = bread(dev, bno);
  memset(bp->ದತ್ತಾಂಶ, 0, BSIZE);
  log_write(bp);
  brelse(bp);
}

// Blocks.

// Allocate a zeroed disk block.
// returns 0 if out of disk space.
ಸ್ಥಿರ uint
balloc(uint dev)
{
  ಪೂರ್ಣಾಂಕ b, bi, m;
  ರಚನೆ buf *bp;

  bp = 0;
  ಗಾಗಿ (b = 0; b < sb.ಗಾತ್ರ_2; b += BPB) {
    bp = bread(dev, BBLOCK(b, sb));
    ಗಾಗಿ (bi = 0; bi < BPB && b + bi < sb.ಗಾತ್ರ_2; bi++) {
      m = 1 << (bi % 8);
      ಆದರೆ ((bp->ದತ್ತಾಂಶ[bi / 8] & m) == 0) { // Is block free?
        bp->ದತ್ತಾಂಶ[bi / 8] |= m;           // Mark block in use.
        log_write(bp);
        brelse(bp);
        bzero(dev, b + bi);
        ಹಿಂತಿರುಗಿ b + bi;
      }
    }
    brelse(bp);
  }
  printk("balloc: out of blocks\n");
  ಹಿಂತಿರುಗಿ 0;
}

// Free a disk block.
ಸ್ಥಿರ ಶೂನ್ಯ
bfree(ಪೂರ್ಣಾಂಕ dev, uint b)
{
  ರಚನೆ buf *bp;
  ಪೂರ್ಣಾಂಕ bi, m;

  bp = bread(dev, BBLOCK(b, sb));
  bi = b % BPB;
  m = 1 << (bi % 8);
  ಆದರೆ ((bp->ದತ್ತಾಂಶ[bi / 8] & m) == 0)
    panic("freeing free block");
  bp->ದತ್ತಾಂಶ[bi / 8] &= ~m;
  log_write(bp);
  brelse(bp);
}

// Inodes.
//
// An inode describes a single unnamed file.
// The inode disk structure holds metadata: the file's type,
// its size, the number of links referring to it, and the
// list of blocks holding the file's content.
//
// The inodes are laid out sequentially on disk at block
// sb.inodestart. Each inode has a number, indicating its
// position on the disk.
//
// The kernel keeps a table of in-use inodes in memory
// to provide a place for synchronizing access
// to inodes used by multiple processes. The in-memory
// inodes include book-keeping information that is
// not stored on disk: ip->ref and ip->valid.
//
// An inode and its in-memory representation go through a
// sequence of states before they can be used by the
// rest of the file system code.
//
// * Allocation: an inode is allocated if its type (on disk)
//   is non-zero. ialloc() allocates, and iput() frees if
//   the reference and link counts have fallen to zero.
//
// * Referencing in table: an entry in the inode table
//   is free if ip->ref is zero. Otherwise ip->ref tracks
//   the number of in-memory pointers to the entry (open
//   files and current directories). iget() finds or
//   creates a table entry and increments its ref; iput()
//   decrements ref.
//
// * Valid: the information (type, size, &c) in an inode
//   table entry is only correct when ip->valid is 1.
//   ilock() reads the inode from
//   the disk and sets ip->valid, while iput() clears
//   ip->valid if ip->ref has fallen to zero.
//
// * Locked: file system code may only examine and modify
//   the information in an inode and its content if it
//   has first locked the inode.
//
// Thus a typical sequence is:
//   ip = iget(dev, inum)
//   ilock(ip)
//   ... examine and modify ip->xxx ...
//   iunlock(ip)
//   iput(ip)
//
// ilock() is separate from iget() so that system calls can
// get a long-term reference to an inode (as for an open file)
// and only lock it for short periods (e.g., in read()).
// The separation also helps avoid deadlock and races during
// pathname lookup. iget() increments ip->ref so that the inode
// stays in the table and pointers to it remain valid.
//
// Many internal file system functions expect the caller to
// have locked the inodes involved; this lets callers create
// multi-step atomic operations.
//
// The itable.lock spin-lock protects the allocation of itable
// entries. Since ip->ref indicates whether an entry is free,
// and ip->dev and ip->inum indicate which i-node an entry
// holds, one must hold itable.lock while using any of those fields.
//
// An ip->lock sleep-lock protects all ip-> fields other than ref,
// dev, and inum.  One must hold ip->lock in order to
// read or write that inode's ip->valid, ip->size, ip->type, &c.

ರಚನೆ {
  ರಚನೆ spinlock ಬೀಗ;
  ರಚನೆ inode inode[NINODE];
} itable;

ಶೂನ್ಯ
iinit()
{
  ಪೂರ್ಣಾಂಕ i = 0;

  initlock(&itable.ಬೀಗ, "itable");
  ಗಾಗಿ (i = 0; i < NINODE; i++) {
    initsleeplock(&itable.inode[i].ಬೀಗ, "inode");
  }
}

ಸ್ಥಿರ ರಚನೆ inode *iget(uint dev, uint inum);

// Allocate an inode on device dev.
// Mark it as allocated by  giving it type type.
// Returns an unlocked but allocated and referenced inode,
// or NULL if there is no free inode.
ರಚನೆ inode *
ialloc(uint dev, ಗಿಡ್ಡ type)
{
  ಪೂರ್ಣಾಂಕ inum;
  ರಚನೆ buf *bp;
  ರಚನೆ dinode *dip;

  ಗಾಗಿ (inum = 1; inum < sb.ninodes; inum++) {
    bp = bread(dev, IBLOCK(inum, sb));
    dip = (ರಚನೆ dinode *)bp->ದತ್ತಾಂಶ + inum % IPB;
    ಆದರೆ (dip->type == 0) { // a free inode
      memset(dip, 0, ಗಾತ್ರ(*dip));
      dip->type = type;
      log_write(bp); // mark it allocated on the disk
      brelse(bp);
      ಹಿಂತಿರುಗಿ iget(dev, inum);
    }
    brelse(bp);
  }
  printk("ialloc: no inodes\n");
  ಹಿಂತಿರುಗಿ 0;
}

// Copy a modified in-memory inode to disk.
// Must be called after every change to an ip->xxx field
// that lives on disk.
// Caller must hold ip->lock.
ಶೂನ್ಯ
iupdate(ರಚನೆ inode *ip)
{
  ರಚನೆ buf *bp;
  ರಚನೆ dinode *dip;

  bp = bread(ip->dev, IBLOCK(ip->inum, sb));
  dip = (ರಚನೆ dinode *)bp->ದತ್ತಾಂಶ + ip->inum % IPB;
  dip->type = ip->type;
  dip->major = ip->major;
  dip->minor = ip->minor;
  dip->nlink = ip->nlink;
  dip->ಗಾತ್ರ_2 = ip->ಗಾತ್ರ_2;
  memmove(dip->addrs, ip->addrs, ಗಾತ್ರ(ip->addrs));
  log_write(bp);
  brelse(bp);
}

// Find the inode with number inum on device dev
// and return the in-memory copy. Does not lock
// the inode and does not read it from disk.
ಸ್ಥಿರ ರಚನೆ inode *
iget(uint dev, uint inum)
{
  ರಚನೆ inode *ip, *ಖಾಲಿಗಳು;

  acquire(&itable.ಬೀಗ);

  // Is the inode already in the table?
  ಖಾಲಿಗಳು = 0;
  ಗಾಗಿ (ip = &itable.inode[0]; ip < &itable.inode[NINODE]; ip++) {
    ಆದರೆ (ip->ref > 0 && ip->dev == dev && ip->inum == inum) {
      ip->ref++;
      release(&itable.ಬೀಗ);
      ಹಿಂತಿರುಗಿ ip;
    }
    ಆದರೆ (ಖಾಲಿಗಳು == 0 && ip->ref == 0) // Remember empty slot.
      ಖಾಲಿಗಳು = ip;
  }

  // Recycle an inode entry.
  ಆದರೆ (ಖಾಲಿಗಳು == 0)
    panic("iget: no inodes");

  ip = ಖಾಲಿಗಳು;
  ip->dev = dev;
  ip->inum = inum;
  ip->ref = 1;
  ip->valid = 0;
  release(&itable.ಬೀಗ);

  ಹಿಂತಿರುಗಿ ip;
}

// Increment reference count for ip.
// Returns ip to enable ip = idup(ip1) idiom.
ರಚನೆ inode *
idup(ರಚನೆ inode *ip)
{
  acquire(&itable.ಬೀಗ);
  ip->ref++;
  release(&itable.ಬೀಗ);
  ಹಿಂತಿರುಗಿ ip;
}

// Lock the given inode.
// Reads the inode from disk if necessary.
ಶೂನ್ಯ
ilock(ರಚನೆ inode *ip)
{
  ರಚನೆ buf *bp;
  ರಚನೆ dinode *dip;

  ಆದರೆ (ip == 0 || ip->ref < 1)
    panic("ilock");

  acquiresleep(&ip->ಬೀಗ);

  ಆದರೆ (ip->valid == 0) {
    bp = bread(ip->dev, IBLOCK(ip->inum, sb));
    dip = (ರಚನೆ dinode *)bp->ದತ್ತಾಂಶ + ip->inum % IPB;
    ip->type = dip->type;
    ip->major = dip->major;
    ip->minor = dip->minor;
    ip->nlink = dip->nlink;
    ip->ಗಾತ್ರ_2 = dip->ಗಾತ್ರ_2;
    memmove(ip->addrs, dip->addrs, ಗಾತ್ರ(ip->addrs));
    brelse(bp);
    ip->valid = 1;
    ಆದರೆ (ip->type == 0)
      panic("ilock: no type");
  }
}

// Unlock the given inode.
ಶೂನ್ಯ
iunlock(ರಚನೆ inode *ip)
{
  ಆದರೆ (ip == 0 || !holdingsleep(&ip->ಬೀಗ) || ip->ref < 1)
    panic("iunlock");

  releasesleep(&ip->ಬೀಗ);
}

// Mark the on-disk inode free.
ಸ್ಥಿರ ಶೂನ್ಯ
ifree(uint dev, uint inum)
{
  ರಚನೆ buf *bp = bread(dev, IBLOCK(inum, sb));
  ರಚನೆ dinode *dip = (ರಚನೆ dinode *)bp->ದತ್ತಾಂಶ + inum % IPB;
  dip->type = 0;
  log_write(bp);
  brelse(bp);
}

// Drop a reference to an in-memory inode.
// If that was the last reference, the inode table entry can
// be recycled.
// If that was the last reference and the inode has no links
// to it, free the inode (and its content) on disk.
// All calls to iput() must be inside a transaction in
// case it has to free the inode.
ಶೂನ್ಯ
iput(ರಚನೆ inode *ip)
{
  acquire(&itable.ಬೀಗ);

  // Last reference of an unlinked inode?  Capture dev/inum before ref--,
  // since once ref hits 0, ip may be recycled by a concurrent iget()
  // for a different inum.
  ಪೂರ್ಣಾಂಕ ಕೊನೆಯ = (ip->ref == 1 && ip->valid && ip->nlink == 0);
  uint dev = ip->dev, inum = ip->inum;

  ಆದರೆ (ಕೊನೆಯ) {
    // ip->ref == 1 means no other process can have ip locked.
    acquiresleep(&ip->ಬೀಗ);
    release(&itable.ಬೀಗ);

    itrunc(ip); // free the data blocks (type stays nonzero on disk)
    ip->valid = 0;

    releasesleep(&ip->ಬೀಗ);

    acquire(&itable.ಬೀಗ);
  }

  ip->ref--;
  release(&itable.ಬೀಗ);

  ಆದರೆ (ಕೊನೆಯ)
    ifree(dev, inum); // now clear type on disk: inum becomes allocatable
}

// Common idiom: unlock, then put.
ಶೂನ್ಯ
iunlockput(ರಚನೆ inode *ip)
{
  iunlock(ip);
  iput(ip);
}

ಶೂನ್ಯ
ireclaim(ಪೂರ್ಣಾಂಕ dev)
{
  ಗಾಗಿ (ಪೂರ್ಣಾಂಕ inum = 1; inum < sb.ninodes; inum++) {
    ರಚನೆ inode *ip = 0;
    ರಚನೆ buf *bp = bread(dev, IBLOCK(inum, sb));
    ರಚನೆ dinode *dip = (ರಚನೆ dinode *)bp->ದತ್ತಾಂಶ + inum % IPB;
    ಆದರೆ (dip->type != 0 && dip->nlink == 0) { // is an orphaned inode
      printk("ireclaim: orphaned inode %d\n", inum);
      ip = iget(dev, inum);
    }
    brelse(bp);
    ಆದರೆ (ip) {
      begin_op();
      ilock(ip);
      iunlock(ip);
      iput(ip);
      ಕೊನೆ_op();
    }
  }
}

// Inode content
//
// The content (data) associated with each inode is stored
// in blocks on the disk. The first NDIRECT block numbers
// are listed in ip->addrs[].  The next NINDIRECT blocks are
// listed in block ip->addrs[NDIRECT].

// Return the disk block address of the nth block in inode ip.
// If there is no such block, bmap allocates one.
// returns 0 if out of disk space.
ಸ್ಥಿರ uint
bmap(ರಚನೆ inode *ip, uint bn)
{
  uint addr, *a;
  ರಚನೆ buf *bp;

  ಆದರೆ (bn < NDIRECT) {
    ಆದರೆ ((addr = ip->addrs[bn]) == 0) {
      addr = balloc(ip->dev);
      ಆದರೆ (addr == 0)
        ಹಿಂತಿರುಗಿ 0;
      ip->addrs[bn] = addr;
    }
    ಹಿಂತಿರುಗಿ addr;
  }
  bn -= NDIRECT;

  ಆದರೆ (bn < NINDIRECT) {
    // Load indirect block, allocating if necessary.
    ಆದರೆ ((addr = ip->addrs[NDIRECT]) == 0) {
      addr = balloc(ip->dev);
      ಆದರೆ (addr == 0)
        ಹಿಂತಿರುಗಿ 0;
      ip->addrs[NDIRECT] = addr;
    }
    bp = bread(ip->dev, addr);
    a = (uint *)bp->ದತ್ತಾಂಶ;
    ಆದರೆ ((addr = a[bn]) == 0) {
      addr = balloc(ip->dev);
      ಆದರೆ (addr) {
        a[bn] = addr;
        log_write(bp);
      }
    }
    brelse(bp);
    ಹಿಂತಿರುಗಿ addr;
  }

  panic("bmap: out of range");
}

// Truncate inode (discard contents).
// Caller must hold ip->lock.
ಶೂನ್ಯ
itrunc(ರಚನೆ inode *ip)
{
  ಪೂರ್ಣಾಂಕ i, j;
  ರಚನೆ buf *bp;
  uint *a;

  ಗಾಗಿ (i = 0; i < NDIRECT; i++) {
    ಆದರೆ (ip->addrs[i]) {
      bfree(ip->dev, ip->addrs[i]);
      ip->addrs[i] = 0;
    }
  }

  ಆದರೆ (ip->addrs[NDIRECT]) {
    bp = bread(ip->dev, ip->addrs[NDIRECT]);
    a = (uint *)bp->ದತ್ತಾಂಶ;
    ಗಾಗಿ (j = 0; j < NINDIRECT; j++) {
      ಆದರೆ (a[j])
        bfree(ip->dev, a[j]);
    }
    brelse(bp);
    bfree(ip->dev, ip->addrs[NDIRECT]);
    ip->addrs[NDIRECT] = 0;
  }

  ip->ಗಾತ್ರ_2 = 0;
  iupdate(ip);
}

// Copy stat information from inode.
// Caller must hold ip->lock.
ಶೂನ್ಯ
stati(ರಚನೆ inode *ip, ರಚನೆ stat *st)
{
  st->dev = ip->dev;
  st->ino = ip->inum;
  st->type = ip->type;
  st->nlink = ip->nlink;
  st->ಗಾತ್ರ_2 = ip->ಗಾತ್ರ_2;
}

// Read data from inode.
// Caller must hold ip->lock.
// If user_dst==1, then dst is a user virtual address;
// otherwise, dst is a kernel address.
ಪೂರ್ಣಾಂಕ
readi(ರಚನೆ inode *ip, ಪೂರ್ಣಾಂಕ user_dst, uint64 dst, uint off, uint n)
{
  uint tot, m;
  ರಚನೆ buf *bp;

  ಆದರೆ (off > ip->ಗಾತ್ರ_2 || off + n < off)
    ಹಿಂತಿರುಗಿ 0;
  ಆದರೆ (off + n > ip->ಗಾತ್ರ_2)
    n = ip->ಗಾತ್ರ_2 - off;

  ಗಾಗಿ (tot = 0; tot < n; tot += m, off += m, dst += m) {
    uint addr = bmap(ip, off / BSIZE);
    ಆದರೆ (addr == 0)
      ನಿಲ್ಲಿಸು;
    bp = bread(ip->dev, addr);
    m = min(n - tot, BSIZE - off % BSIZE);
    ಆದರೆ (either_copyout(user_dst, dst, bp->ದತ್ತಾಂಶ + (off % BSIZE), m) == -1) {
      brelse(bp);
      tot = -1;
      ನಿಲ್ಲಿಸು;
    }
    brelse(bp);
  }
  ಹಿಂತಿರುಗಿ tot;
}

// Write data to inode.
// Caller must hold ip->lock.
// If user_src==1, then src is a user virtual address;
// otherwise, src is a kernel address.
// Returns the number of bytes successfully written.
// If the return value is less than the requested n,
// there was an error of some kind.
ಪೂರ್ಣಾಂಕ
writei(ರಚನೆ inode *ip, ಪೂರ್ಣಾಂಕ user_src, uint64 src, uint off, uint n)
{
  uint tot, m;
  ರಚನೆ buf *bp;

  ಆದರೆ (off > ip->ಗಾತ್ರ_2 || off + n < off)
    ಹಿಂತಿರುಗಿ -1;
  ಆದರೆ (off + n > MAXFILE * BSIZE)
    ಹಿಂತಿರುಗಿ -1;

  ಗಾಗಿ (tot = 0; tot < n; tot += m, off += m, src += m) {
    uint addr = bmap(ip, off / BSIZE);
    ಆದರೆ (addr == 0)
      ನಿಲ್ಲಿಸು;
    bp = bread(ip->dev, addr);
    m = min(n - tot, BSIZE - off % BSIZE);
    ಆದರೆ (either_copyin(bp->ದತ್ತಾಂಶ + (off % BSIZE), user_src, src, m) == -1) {
      // Might have partially updated the block, so we need to log it.
      log_write(bp);
      brelse(bp);
      ನಿಲ್ಲಿಸು;
    }
    log_write(bp);
    brelse(bp);
  }

  ಆದರೆ (off > ip->ಗಾತ್ರ_2)
    ip->ಗಾತ್ರ_2 = off;

  // write the i-node back to disk even if the size didn't change
  // because the loop above might have called bmap() and added a new
  // block to ip->addrs[].
  iupdate(ip);

  ಹಿಂತಿರುಗಿ tot;
}

// Directories

ಪೂರ್ಣಾಂಕ
namecmp(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *t)
{
  ಹಿಂತಿರುಗಿ ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(s, t, DIRSIZ);
}

// Look for a directory entry in a directory.
// If found, set *poff to byte offset of entry.
ರಚನೆ inode *
dirlookup(ರಚನೆ inode *dp, ಅಕ್ಷರ *name, uint *poff)
{
  uint off, inum;
  ರಚನೆ dirent de;

  ಆದರೆ (dp->type != T_DIR)
    panic("dirlookup not DIR");

  ಗಾಗಿ (off = 0; off < dp->ಗಾತ್ರ_2; off += ಗಾತ್ರ(de)) {
    ಆದರೆ (readi(dp, 0, (uint64)&de, off, ಗಾತ್ರ(de)) != ಗಾತ್ರ(de))
      panic("dirlookup read");
    ಆದರೆ (de.inum == 0)
      ಮುಂದುವರಿ;
    ಆದರೆ (namecmp(name, de.name) == 0) {
      // entry matches path element
      ಆದರೆ (poff)
        *poff = off;
      inum = de.inum;
      ಹಿಂತಿರುಗಿ iget(dp->dev, inum);
    }
  }

  ಹಿಂತಿರುಗಿ 0;
}

// Write a new directory entry (name, inum) into the directory dp.
// Returns 0 on success, -1 on failure (e.g. out of disk blocks).
ಪೂರ್ಣಾಂಕ
dirlink(ರಚನೆ inode *dp, ಅಕ್ಷರ *name, uint inum)
{
  ಪೂರ್ಣಾಂಕ off;
  ರಚನೆ dirent de;
  ರಚನೆ inode *ip;

  // Check that name is not present.
  ಆದರೆ ((ip = dirlookup(dp, name, 0)) != 0) {
    iput(ip);
    ಹಿಂತಿರುಗಿ -1;
  }

  // Look for an empty dirent.
  ಗಾಗಿ (off = 0; off < dp->ಗಾತ್ರ_2; off += ಗಾತ್ರ(de)) {
    ಆದರೆ (readi(dp, 0, (uint64)&de, off, ಗಾತ್ರ(de)) != ಗಾತ್ರ(de))
      panic("dirlink read");
    ಆದರೆ (de.inum == 0)
      ನಿಲ್ಲಿಸು;
  }

  strncpy(de.name, name, DIRSIZ);
  de.inum = inum;
  ಆದರೆ (writei(dp, 0, (uint64)&de, off, ಗಾತ್ರ(de)) != ಗಾತ್ರ(de))
    ಹಿಂತಿರುಗಿ -1;

  ಹಿಂತಿರುಗಿ 0;
}

// Paths

// Copy the next path element from path into name.
// Return a pointer to the element following the copied one.
// The returned path has no leading slashes,
// so the caller can check *path=='\0' to see if the name is the last one.
// If no name to remove, return 0.
//
// Examples:
//   skipelem("a/bb/c", name) = "bb/c", setting name = "a"
//   skipelem("///a//bb", name) = "bb", setting name = "a"
//   skipelem("a", name) = "", setting name = "a"
//   skipelem("", name) = skipelem("////", name) = 0
//
ಸ್ಥಿರ ಅಕ್ಷರ *
skipelem(ಅಕ್ಷರ *ಮಾರ್ಗ, ಅಕ್ಷರ *name)
{
  ಅಕ್ಷರ *s;
  ಪೂರ್ಣಾಂಕ len;

  ಆಗ (*ಮಾರ್ಗ == '/')
    ಮಾರ್ಗ++;
  ಆದರೆ (*ಮಾರ್ಗ == 0)
    ಹಿಂತಿರುಗಿ 0;
  s = ಮಾರ್ಗ;
  ಆಗ (*ಮಾರ್ಗ != '/' && *ಮಾರ್ಗ != 0)
    ಮಾರ್ಗ++;
  len = ಮಾರ್ಗ - s;
  ಆದರೆ (len >= DIRSIZ)
    memmove(name, s, DIRSIZ);
  ಇಲ್ಲದಿದ್ದರೆ {
    memmove(name, s, len);
    name[len] = 0;
  }
  ಆಗ (*ಮಾರ್ಗ == '/')
    ಮಾರ್ಗ++;
  ಹಿಂತಿರುಗಿ ಮಾರ್ಗ;
}

// Look up and return the inode for a path name.
// If parent != 0, return the inode for the parent and copy the final
// path element into name, which must have room for DIRSIZ bytes.
// Must be called inside a transaction since it calls iput().
ಸ್ಥಿರ ರಚನೆ inode *
namex(ಅಕ್ಷರ *ಮಾರ್ಗ, ಪೂರ್ಣಾಂಕ nameiparent, ಅಕ್ಷರ *name)
{
  ರಚನೆ inode *ip, *ಮುಂದಿನ;

  ಆದರೆ (*ಮಾರ್ಗ == '/')
    ip = iget(ROOTDEV, ROOTINO);
  ಇಲ್ಲದಿದ್ದರೆ
    ip = idup(myproc()->cwd);

  ಆಗ ((ಮಾರ್ಗ = skipelem(ಮಾರ್ಗ, name)) != 0) {
    ilock(ip);
    ಆದರೆ (ip->type != T_DIR) {
      iunlockput(ip);
      ಹಿಂತಿರುಗಿ 0;
    }
    ಆದರೆ (ip->nlink == 0) {
      iunlockput(ip);
      ಹಿಂತಿರುಗಿ 0;
    }
    ಆದರೆ (nameiparent && *ಮಾರ್ಗ == '\0') {
      // Stop one level early.
      iunlock(ip);
      ಹಿಂತಿರುಗಿ ip;
    }
    ಆದರೆ ((ಮುಂದಿನ = dirlookup(ip, name, 0)) == 0) {
      iunlockput(ip);
      ಹಿಂತಿರುಗಿ 0;
    }
    iunlockput(ip);
    ip = ಮುಂದಿನ;
  }
  ಆದರೆ (nameiparent) {
    iput(ip);
    ಹಿಂತಿರುಗಿ 0;
  }
  ಹಿಂತಿರುಗಿ ip;
}

ರಚನೆ inode *
namei(ಅಕ್ಷರ *ಮಾರ್ಗ)
{
  ಅಕ್ಷರ name[DIRSIZ];
  ಹಿಂತಿರುಗಿ namex(ಮಾರ್ಗ, 0, name);
}

ರಚನೆ inode *
nameiparent(ಅಕ್ಷರ *ಮಾರ್ಗ, ಅಕ್ಷರ *name)
{
  ಹಿಂತಿರುಗಿ namex(ಮಾರ್ಗ, 1, name);
}
