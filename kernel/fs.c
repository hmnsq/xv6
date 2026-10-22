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

#ਅਨ੍ਤਰ੍ਭਾਵ "types.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "riscv.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "defs.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "param.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "stat.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "spinlock.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "proc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "sleeplock.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "fs.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "buf.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "file.h"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ min(a, b) ((a) < (b) ? (a) : (b))
// there should be one superblock per disk device, but we run with
// only one device
ਬਣਤਰ superblock sb;

// Read the super block.
ਸਥਿਰ ਖਾਲੀ
readsb(ਪੂਰਨ dev, ਬਣਤਰ superblock *sb)
{
  ਬਣਤਰ buf *bp;

  bp = bread(dev, 1);
  memmove(sb, bp->ਅੰਕੜੇ, ਆਕਾਰ(*sb));
  brelse(bp);
}

// Init fs
ਖਾਲੀ
fsinit(ਪੂਰਨ dev)
{
  readsb(dev, &sb);
  ਜੇ (sb.magic != FSMAGIC)
    panic("invalid file system");
  initlog(dev, &sb);
  ireclaim(dev);
}

// Zero a block.
ਸਥਿਰ ਖਾਲੀ
bzero(ਪੂਰਨ dev, ਪੂਰਨ bno)
{
  ਬਣਤਰ buf *bp;

  bp = bread(dev, bno);
  memset(bp->ਅੰਕੜੇ, 0, BSIZE);
  log_write(bp);
  brelse(bp);
}

// Blocks.

// Allocate a zeroed disk block.
// returns 0 if out of disk space.
ਸਥਿਰ uint
balloc(uint dev)
{
  ਪੂਰਨ b, bi, m;
  ਬਣਤਰ buf *bp;

  bp = 0;
  ਲਈ (b = 0; b < sb.ਮਾਪ; b += BPB) {
    bp = bread(dev, BBLOCK(b, sb));
    ਲਈ (bi = 0; bi < BPB && b + bi < sb.ਮਾਪ; bi++) {
      m = 1 << (bi % 8);
      ਜੇ ((bp->ਅੰਕੜੇ[bi / 8] & m) == 0) { // Is block free?
        bp->ਅੰਕੜੇ[bi / 8] |= m;           // Mark block in use.
        log_write(bp);
        brelse(bp);
        bzero(dev, b + bi);
        ਵਾਪਸ b + bi;
      }
    }
    brelse(bp);
  }
  printk("balloc: out of blocks\n");
  ਵਾਪਸ 0;
}

// Free a disk block.
ਸਥਿਰ ਖਾਲੀ
bfree(ਪੂਰਨ dev, uint b)
{
  ਬਣਤਰ buf *bp;
  ਪੂਰਨ bi, m;

  bp = bread(dev, BBLOCK(b, sb));
  bi = b % BPB;
  m = 1 << (bi % 8);
  ਜੇ ((bp->ਅੰਕੜੇ[bi / 8] & m) == 0)
    panic("freeing free block");
  bp->ਅੰਕੜੇ[bi / 8] &= ~m;
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

ਬਣਤਰ {
  ਬਣਤਰ spinlock ਤਾਲਾ;
  ਬਣਤਰ inode inode[NINODE];
} itable;

ਖਾਲੀ
iinit()
{
  ਪੂਰਨ i = 0;

  initlock(&itable.ਤਾਲਾ, "itable");
  ਲਈ (i = 0; i < NINODE; i++) {
    initsleeplock(&itable.inode[i].ਤਾਲਾ, "inode");
  }
}

ਸਥਿਰ ਬਣਤਰ inode *iget(uint dev, uint inum);

// Allocate an inode on device dev.
// Mark it as allocated by  giving it type type.
// Returns an unlocked but allocated and referenced inode,
// or NULL if there is no free inode.
ਬਣਤਰ inode *
ialloc(uint dev, ਛੋਟਾ type)
{
  ਪੂਰਨ inum;
  ਬਣਤਰ buf *bp;
  ਬਣਤਰ dinode *dip;

  ਲਈ (inum = 1; inum < sb.ninodes; inum++) {
    bp = bread(dev, IBLOCK(inum, sb));
    dip = (ਬਣਤਰ dinode *)bp->ਅੰਕੜੇ + inum % IPB;
    ਜੇ (dip->type == 0) { // a free inode
      memset(dip, 0, ਆਕਾਰ(*dip));
      dip->type = type;
      log_write(bp); // mark it allocated on the disk
      brelse(bp);
      ਵਾਪਸ iget(dev, inum);
    }
    brelse(bp);
  }
  printk("ialloc: no inodes\n");
  ਵਾਪਸ 0;
}

// Copy a modified in-memory inode to disk.
// Must be called after every change to an ip->xxx field
// that lives on disk.
// Caller must hold ip->lock.
ਖਾਲੀ
iupdate(ਬਣਤਰ inode *ip)
{
  ਬਣਤਰ buf *bp;
  ਬਣਤਰ dinode *dip;

  bp = bread(ip->dev, IBLOCK(ip->inum, sb));
  dip = (ਬਣਤਰ dinode *)bp->ਅੰਕੜੇ + ip->inum % IPB;
  dip->type = ip->type;
  dip->major = ip->major;
  dip->minor = ip->minor;
  dip->nlink = ip->nlink;
  dip->ਮਾਪ = ip->ਮਾਪ;
  memmove(dip->addrs, ip->addrs, ਆਕਾਰ(ip->addrs));
  log_write(bp);
  brelse(bp);
}

// Find the inode with number inum on device dev
// and return the in-memory copy. Does not lock
// the inode and does not read it from disk.
ਸਥਿਰ ਬਣਤਰ inode *
iget(uint dev, uint inum)
{
  ਬਣਤਰ inode *ip, *ਖਾਲੀ_ਥਾਵਾਂ;

  acquire(&itable.ਤਾਲਾ);

  // Is the inode already in the table?
  ਖਾਲੀ_ਥਾਵਾਂ = 0;
  ਲਈ (ip = &itable.inode[0]; ip < &itable.inode[NINODE]; ip++) {
    ਜੇ (ip->ref > 0 && ip->dev == dev && ip->inum == inum) {
      ip->ref++;
      release(&itable.ਤਾਲਾ);
      ਵਾਪਸ ip;
    }
    ਜੇ (ਖਾਲੀ_ਥਾਵਾਂ == 0 && ip->ref == 0) // Remember empty slot.
      ਖਾਲੀ_ਥਾਵਾਂ = ip;
  }

  // Recycle an inode entry.
  ਜੇ (ਖਾਲੀ_ਥਾਵਾਂ == 0)
    panic("iget: no inodes");

  ip = ਖਾਲੀ_ਥਾਵਾਂ;
  ip->dev = dev;
  ip->inum = inum;
  ip->ref = 1;
  ip->valid = 0;
  release(&itable.ਤਾਲਾ);

  ਵਾਪਸ ip;
}

// Increment reference count for ip.
// Returns ip to enable ip = idup(ip1) idiom.
ਬਣਤਰ inode *
idup(ਬਣਤਰ inode *ip)
{
  acquire(&itable.ਤਾਲਾ);
  ip->ref++;
  release(&itable.ਤਾਲਾ);
  ਵਾਪਸ ip;
}

// Lock the given inode.
// Reads the inode from disk if necessary.
ਖਾਲੀ
ilock(ਬਣਤਰ inode *ip)
{
  ਬਣਤਰ buf *bp;
  ਬਣਤਰ dinode *dip;

  ਜੇ (ip == 0 || ip->ref < 1)
    panic("ilock");

  acquiresleep(&ip->ਤਾਲਾ);

  ਜੇ (ip->valid == 0) {
    bp = bread(ip->dev, IBLOCK(ip->inum, sb));
    dip = (ਬਣਤਰ dinode *)bp->ਅੰਕੜੇ + ip->inum % IPB;
    ip->type = dip->type;
    ip->major = dip->major;
    ip->minor = dip->minor;
    ip->nlink = dip->nlink;
    ip->ਮਾਪ = dip->ਮਾਪ;
    memmove(ip->addrs, dip->addrs, ਆਕਾਰ(ip->addrs));
    brelse(bp);
    ip->valid = 1;
    ਜੇ (ip->type == 0)
      panic("ilock: no type");
  }
}

// Unlock the given inode.
ਖਾਲੀ
iunlock(ਬਣਤਰ inode *ip)
{
  ਜੇ (ip == 0 || !holdingsleep(&ip->ਤਾਲਾ) || ip->ref < 1)
    panic("iunlock");

  releasesleep(&ip->ਤਾਲਾ);
}

// Mark the on-disk inode free.
ਸਥਿਰ ਖਾਲੀ
ifree(uint dev, uint inum)
{
  ਬਣਤਰ buf *bp = bread(dev, IBLOCK(inum, sb));
  ਬਣਤਰ dinode *dip = (ਬਣਤਰ dinode *)bp->ਅੰਕੜੇ + inum % IPB;
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
ਖਾਲੀ
iput(ਬਣਤਰ inode *ip)
{
  acquire(&itable.ਤਾਲਾ);

  // Last reference of an unlinked inode?  Capture dev/inum before ref--,
  // since once ref hits 0, ip may be recycled by a concurrent iget()
  // for a different inum.
  ਪੂਰਨ ਆਖਰੀ = (ip->ref == 1 && ip->valid && ip->nlink == 0);
  uint dev = ip->dev, inum = ip->inum;

  ਜੇ (ਆਖਰੀ) {
    // ip->ref == 1 means no other process can have ip locked.
    acquiresleep(&ip->ਤਾਲਾ);
    release(&itable.ਤਾਲਾ);

    itrunc(ip); // free the data blocks (type stays nonzero on disk)
    ip->valid = 0;

    releasesleep(&ip->ਤਾਲਾ);

    acquire(&itable.ਤਾਲਾ);
  }

  ip->ref--;
  release(&itable.ਤਾਲਾ);

  ਜੇ (ਆਖਰੀ)
    ifree(dev, inum); // now clear type on disk: inum becomes allocatable
}

// Common idiom: unlock, then put.
ਖਾਲੀ
iunlockput(ਬਣਤਰ inode *ip)
{
  iunlock(ip);
  iput(ip);
}

ਖਾਲੀ
ireclaim(ਪੂਰਨ dev)
{
  ਲਈ (ਪੂਰਨ inum = 1; inum < sb.ninodes; inum++) {
    ਬਣਤਰ inode *ip = 0;
    ਬਣਤਰ buf *bp = bread(dev, IBLOCK(inum, sb));
    ਬਣਤਰ dinode *dip = (ਬਣਤਰ dinode *)bp->ਅੰਕੜੇ + inum % IPB;
    ਜੇ (dip->type != 0 && dip->nlink == 0) { // is an orphaned inode
      printk("ireclaim: orphaned inode %d\n", inum);
      ip = iget(dev, inum);
    }
    brelse(bp);
    ਜੇ (ip) {
      begin_op();
      ilock(ip);
      iunlock(ip);
      iput(ip);
      ਸਿਰਾ_op();
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
ਸਥਿਰ uint
bmap(ਬਣਤਰ inode *ip, uint bn)
{
  uint addr, *a;
  ਬਣਤਰ buf *bp;

  ਜੇ (bn < NDIRECT) {
    ਜੇ ((addr = ip->addrs[bn]) == 0) {
      addr = balloc(ip->dev);
      ਜੇ (addr == 0)
        ਵਾਪਸ 0;
      ip->addrs[bn] = addr;
    }
    ਵਾਪਸ addr;
  }
  bn -= NDIRECT;

  ਜੇ (bn < NINDIRECT) {
    // Load indirect block, allocating if necessary.
    ਜੇ ((addr = ip->addrs[NDIRECT]) == 0) {
      addr = balloc(ip->dev);
      ਜੇ (addr == 0)
        ਵਾਪਸ 0;
      ip->addrs[NDIRECT] = addr;
    }
    bp = bread(ip->dev, addr);
    a = (uint *)bp->ਅੰਕੜੇ;
    ਜੇ ((addr = a[bn]) == 0) {
      addr = balloc(ip->dev);
      ਜੇ (addr) {
        a[bn] = addr;
        log_write(bp);
      }
    }
    brelse(bp);
    ਵਾਪਸ addr;
  }

  panic("bmap: out of range");
}

// Truncate inode (discard contents).
// Caller must hold ip->lock.
ਖਾਲੀ
itrunc(ਬਣਤਰ inode *ip)
{
  ਪੂਰਨ i, j;
  ਬਣਤਰ buf *bp;
  uint *a;

  ਲਈ (i = 0; i < NDIRECT; i++) {
    ਜੇ (ip->addrs[i]) {
      bfree(ip->dev, ip->addrs[i]);
      ip->addrs[i] = 0;
    }
  }

  ਜੇ (ip->addrs[NDIRECT]) {
    bp = bread(ip->dev, ip->addrs[NDIRECT]);
    a = (uint *)bp->ਅੰਕੜੇ;
    ਲਈ (j = 0; j < NINDIRECT; j++) {
      ਜੇ (a[j])
        bfree(ip->dev, a[j]);
    }
    brelse(bp);
    bfree(ip->dev, ip->addrs[NDIRECT]);
    ip->addrs[NDIRECT] = 0;
  }

  ip->ਮਾਪ = 0;
  iupdate(ip);
}

// Copy stat information from inode.
// Caller must hold ip->lock.
ਖਾਲੀ
stati(ਬਣਤਰ inode *ip, ਬਣਤਰ stat *st)
{
  st->dev = ip->dev;
  st->ino = ip->inum;
  st->type = ip->type;
  st->nlink = ip->nlink;
  st->ਮਾਪ = ip->ਮਾਪ;
}

// Read data from inode.
// Caller must hold ip->lock.
// If user_dst==1, then dst is a user virtual address;
// otherwise, dst is a kernel address.
ਪੂਰਨ
readi(ਬਣਤਰ inode *ip, ਪੂਰਨ user_dst, uint64 dst, uint off, uint n)
{
  uint tot, m;
  ਬਣਤਰ buf *bp;

  ਜੇ (off > ip->ਮਾਪ || off + n < off)
    ਵਾਪਸ 0;
  ਜੇ (off + n > ip->ਮਾਪ)
    n = ip->ਮਾਪ - off;

  ਲਈ (tot = 0; tot < n; tot += m, off += m, dst += m) {
    uint addr = bmap(ip, off / BSIZE);
    ਜੇ (addr == 0)
      ਰੋਕੋ;
    bp = bread(ip->dev, addr);
    m = min(n - tot, BSIZE - off % BSIZE);
    ਜੇ (either_copyout(user_dst, dst, bp->ਅੰਕੜੇ + (off % BSIZE), m) == -1) {
      brelse(bp);
      tot = -1;
      ਰੋਕੋ;
    }
    brelse(bp);
  }
  ਵਾਪਸ tot;
}

// Write data to inode.
// Caller must hold ip->lock.
// If user_src==1, then src is a user virtual address;
// otherwise, src is a kernel address.
// Returns the number of bytes successfully written.
// If the return value is less than the requested n,
// there was an error of some kind.
ਪੂਰਨ
writei(ਬਣਤਰ inode *ip, ਪੂਰਨ user_src, uint64 src, uint off, uint n)
{
  uint tot, m;
  ਬਣਤਰ buf *bp;

  ਜੇ (off > ip->ਮਾਪ || off + n < off)
    ਵਾਪਸ -1;
  ਜੇ (off + n > MAXFILE * BSIZE)
    ਵਾਪਸ -1;

  ਲਈ (tot = 0; tot < n; tot += m, off += m, src += m) {
    uint addr = bmap(ip, off / BSIZE);
    ਜੇ (addr == 0)
      ਰੋਕੋ;
    bp = bread(ip->dev, addr);
    m = min(n - tot, BSIZE - off % BSIZE);
    ਜੇ (either_copyin(bp->ਅੰਕੜੇ + (off % BSIZE), user_src, src, m) == -1) {
      // Might have partially updated the block, so we need to log it.
      log_write(bp);
      brelse(bp);
      ਰੋਕੋ;
    }
    log_write(bp);
    brelse(bp);
  }

  ਜੇ (off > ip->ਮਾਪ)
    ip->ਮਾਪ = off;

  // write the i-node back to disk even if the size didn't change
  // because the loop above might have called bmap() and added a new
  // block to ip->addrs[].
  iupdate(ip);

  ਵਾਪਸ tot;
}

// Directories

ਪੂਰਨ
namecmp(ਅਚਲ ਅੱਖਰ *s, ਅਚਲ ਅੱਖਰ *t)
{
  ਵਾਪਸ ਲੜੀ_ਤੁਲਨਾ(s, t, DIRSIZ);
}

// Look for a directory entry in a directory.
// If found, set *poff to byte offset of entry.
ਬਣਤਰ inode *
dirlookup(ਬਣਤਰ inode *dp, ਅੱਖਰ *name, uint *poff)
{
  uint off, inum;
  ਬਣਤਰ dirent de;

  ਜੇ (dp->type != T_DIR)
    panic("dirlookup not DIR");

  ਲਈ (off = 0; off < dp->ਮਾਪ; off += ਆਕਾਰ(de)) {
    ਜੇ (readi(dp, 0, (uint64)&de, off, ਆਕਾਰ(de)) != ਆਕਾਰ(de))
      panic("dirlookup read");
    ਜੇ (de.inum == 0)
      ਜਾਰੀ;
    ਜੇ (namecmp(name, de.name) == 0) {
      // entry matches path element
      ਜੇ (poff)
        *poff = off;
      inum = de.inum;
      ਵਾਪਸ iget(dp->dev, inum);
    }
  }

  ਵਾਪਸ 0;
}

// Write a new directory entry (name, inum) into the directory dp.
// Returns 0 on success, -1 on failure (e.g. out of disk blocks).
ਪੂਰਨ
dirlink(ਬਣਤਰ inode *dp, ਅੱਖਰ *name, uint inum)
{
  ਪੂਰਨ off;
  ਬਣਤਰ dirent de;
  ਬਣਤਰ inode *ip;

  // Check that name is not present.
  ਜੇ ((ip = dirlookup(dp, name, 0)) != 0) {
    iput(ip);
    ਵਾਪਸ -1;
  }

  // Look for an empty dirent.
  ਲਈ (off = 0; off < dp->ਮਾਪ; off += ਆਕਾਰ(de)) {
    ਜੇ (readi(dp, 0, (uint64)&de, off, ਆਕਾਰ(de)) != ਆਕਾਰ(de))
      panic("dirlink read");
    ਜੇ (de.inum == 0)
      ਰੋਕੋ;
  }

  strncpy(de.name, name, DIRSIZ);
  de.inum = inum;
  ਜੇ (writei(dp, 0, (uint64)&de, off, ਆਕਾਰ(de)) != ਆਕਾਰ(de))
    ਵਾਪਸ -1;

  ਵਾਪਸ 0;
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
ਸਥਿਰ ਅੱਖਰ *
skipelem(ਅੱਖਰ *ਰਸਤਾ, ਅੱਖਰ *name)
{
  ਅੱਖਰ *s;
  ਪੂਰਨ len;

  ਜਦੋਂ (*ਰਸਤਾ == '/')
    ਰਸਤਾ++;
  ਜੇ (*ਰਸਤਾ == 0)
    ਵਾਪਸ 0;
  s = ਰਸਤਾ;
  ਜਦੋਂ (*ਰਸਤਾ != '/' && *ਰਸਤਾ != 0)
    ਰਸਤਾ++;
  len = ਰਸਤਾ - s;
  ਜੇ (len >= DIRSIZ)
    memmove(name, s, DIRSIZ);
  ਨਹੀਂਤਾਂ {
    memmove(name, s, len);
    name[len] = 0;
  }
  ਜਦੋਂ (*ਰਸਤਾ == '/')
    ਰਸਤਾ++;
  ਵਾਪਸ ਰਸਤਾ;
}

// Look up and return the inode for a path name.
// If parent != 0, return the inode for the parent and copy the final
// path element into name, which must have room for DIRSIZ bytes.
// Must be called inside a transaction since it calls iput().
ਸਥਿਰ ਬਣਤਰ inode *
namex(ਅੱਖਰ *ਰਸਤਾ, ਪੂਰਨ nameiparent, ਅੱਖਰ *name)
{
  ਬਣਤਰ inode *ip, *ਅਗਲਾ;

  ਜੇ (*ਰਸਤਾ == '/')
    ip = iget(ROOTDEV, ROOTINO);
  ਨਹੀਂਤਾਂ
    ip = idup(myproc()->cwd);

  ਜਦੋਂ ((ਰਸਤਾ = skipelem(ਰਸਤਾ, name)) != 0) {
    ilock(ip);
    ਜੇ (ip->type != T_DIR) {
      iunlockput(ip);
      ਵਾਪਸ 0;
    }
    ਜੇ (ip->nlink == 0) {
      iunlockput(ip);
      ਵਾਪਸ 0;
    }
    ਜੇ (nameiparent && *ਰਸਤਾ == '\0') {
      // Stop one level early.
      iunlock(ip);
      ਵਾਪਸ ip;
    }
    ਜੇ ((ਅਗਲਾ = dirlookup(ip, name, 0)) == 0) {
      iunlockput(ip);
      ਵਾਪਸ 0;
    }
    iunlockput(ip);
    ip = ਅਗਲਾ;
  }
  ਜੇ (nameiparent) {
    iput(ip);
    ਵਾਪਸ 0;
  }
  ਵਾਪਸ ip;
}

ਬਣਤਰ inode *
namei(ਅੱਖਰ *ਰਸਤਾ)
{
  ਅੱਖਰ name[DIRSIZ];
  ਵਾਪਸ namex(ਰਸਤਾ, 0, name);
}

ਬਣਤਰ inode *
nameiparent(ਅੱਖਰ *ਰਸਤਾ, ਅੱਖਰ *name)
{
  ਵਾਪਸ namex(ਰਸਤਾ, 1, name);
}
