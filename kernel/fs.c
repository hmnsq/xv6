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

#অন্তর্ভাৱ "types.h"
#অন্তর্ভাৱ "riscv.h"
#অন্তর্ভাৱ "defs.h"
#অন্তর্ভাৱ "param.h"
#অন্তর্ভাৱ "stat.h"
#অন্তর্ভাৱ "spinlock.h"
#অন্তর্ভাৱ "proc.h"
#অন্তর্ভাৱ "sleeplock.h"
#অন্তর্ভাৱ "fs.h"
#অন্তর্ভাৱ "buf.h"
#অন্তর্ভাৱ "file.h"

#define min(a, b) ((a) < (b) ? (a) : (b))
// there should be one superblock per disk device, but we run with
// only one device
সংরচনা superblock sb;

// Read the super block.
স্থির শূন্য
readsb(পূর্ণাঙ্ক dev, সংরচনা superblock *sb)
{
  সংরচনা buf *bp;

  bp = bread(dev, 1);
  memmove(sb, bp->data, পরিমাণ(*sb));
  brelse(bp);
}

// Init fs
শূন্য
fsinit(পূর্ণাঙ্ক dev)
{
  readsb(dev, &sb);
  যদি (sb.magic != FSMAGIC)
    panic("invalid file system");
  initlog(dev, &sb);
  ireclaim(dev);
}

// Zero a block.
স্থির শূন্য
bzero(পূর্ণাঙ্ক dev, পূর্ণাঙ্ক bno)
{
  সংরচনা buf *bp;

  bp = bread(dev, bno);
  memset(bp->data, 0, BSIZE);
  log_write(bp);
  brelse(bp);
}

// Blocks.

// Allocate a zeroed disk block.
// returns 0 if out of disk space.
স্থির uint
balloc(uint dev)
{
  পূর্ণাঙ্ক b, bi, m;
  সংরচনা buf *bp;

  bp = 0;
  বাবে (b = 0; b < sb.size; b += BPB) {
    bp = bread(dev, BBLOCK(b, sb));
    বাবে (bi = 0; bi < BPB && b + bi < sb.size; bi++) {
      m = 1 << (bi % 8);
      যদি ((bp->data[bi / 8] & m) == 0) { // Is block free?
        bp->data[bi / 8] |= m;           // Mark block in use.
        log_write(bp);
        brelse(bp);
        bzero(dev, b + bi);
        ঘূৰাই b + bi;
      }
    }
    brelse(bp);
  }
  printk("balloc: out of blocks\n");
  ঘূৰাই 0;
}

// Free a disk block.
স্থির শূন্য
bfree(পূর্ণাঙ্ক dev, uint b)
{
  সংরচনা buf *bp;
  পূর্ণাঙ্ক bi, m;

  bp = bread(dev, BBLOCK(b, sb));
  bi = b % BPB;
  m = 1 << (bi % 8);
  যদি ((bp->data[bi / 8] & m) == 0)
    panic("freeing free block");
  bp->data[bi / 8] &= ~m;
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

সংরচনা {
  সংরচনা spinlock lock;
  সংরচনা inode inode[NINODE];
} itable;

শূন্য
iinit()
{
  পূর্ণাঙ্ক i = 0;

  initlock(&itable.lock, "itable");
  বাবে (i = 0; i < NINODE; i++) {
    initsleeplock(&itable.inode[i].lock, "inode");
  }
}

স্থির সংরচনা inode *iget(uint dev, uint inum);

// Allocate an inode on device dev.
// Mark it as allocated by  giving it type type.
// Returns an unlocked but allocated and referenced inode,
// or NULL if there is no free inode.
সংরচনা inode *
ialloc(uint dev, short type)
{
  পূর্ণাঙ্ক inum;
  সংরচনা buf *bp;
  সংরচনা dinode *dip;

  বাবে (inum = 1; inum < sb.ninodes; inum++) {
    bp = bread(dev, IBLOCK(inum, sb));
    dip = (সংরচনা dinode *)bp->data + inum % IPB;
    যদি (dip->type == 0) { // a free inode
      memset(dip, 0, পরিমাণ(*dip));
      dip->type = type;
      log_write(bp); // mark it allocated on the disk
      brelse(bp);
      ঘূৰাই iget(dev, inum);
    }
    brelse(bp);
  }
  printk("ialloc: no inodes\n");
  ঘূৰাই 0;
}

// Copy a modified in-memory inode to disk.
// Must be called after every change to an ip->xxx field
// that lives on disk.
// Caller must hold ip->lock.
শূন্য
iupdate(সংরচনা inode *ip)
{
  সংরচনা buf *bp;
  সংরচনা dinode *dip;

  bp = bread(ip->dev, IBLOCK(ip->inum, sb));
  dip = (সংরচনা dinode *)bp->data + ip->inum % IPB;
  dip->type = ip->type;
  dip->major = ip->major;
  dip->minor = ip->minor;
  dip->nlink = ip->nlink;
  dip->size = ip->size;
  memmove(dip->addrs, ip->addrs, পরিমাণ(ip->addrs));
  log_write(bp);
  brelse(bp);
}

// Find the inode with number inum on device dev
// and return the in-memory copy. Does not lock
// the inode and does not read it from disk.
স্থির সংরচনা inode *
iget(uint dev, uint inum)
{
  সংরচনা inode *ip, *empty;

  acquire(&itable.lock);

  // Is the inode already in the table?
  empty = 0;
  বাবে (ip = &itable.inode[0]; ip < &itable.inode[NINODE]; ip++) {
    যদি (ip->ref > 0 && ip->dev == dev && ip->inum == inum) {
      ip->ref++;
      release(&itable.lock);
      ঘূৰাই ip;
    }
    যদি (empty == 0 && ip->ref == 0) // Remember empty slot.
      empty = ip;
  }

  // Recycle an inode entry.
  যদি (empty == 0)
    panic("iget: no inodes");

  ip = empty;
  ip->dev = dev;
  ip->inum = inum;
  ip->ref = 1;
  ip->valid = 0;
  release(&itable.lock);

  ঘূৰাই ip;
}

// Increment reference count for ip.
// Returns ip to enable ip = idup(ip1) idiom.
সংরচনা inode *
idup(সংরচনা inode *ip)
{
  acquire(&itable.lock);
  ip->ref++;
  release(&itable.lock);
  ঘূৰাই ip;
}

// Lock the given inode.
// Reads the inode from disk if necessary.
শূন্য
ilock(সংরচনা inode *ip)
{
  সংরচনা buf *bp;
  সংরচনা dinode *dip;

  যদি (ip == 0 || ip->ref < 1)
    panic("ilock");

  acquiresleep(&ip->lock);

  যদি (ip->valid == 0) {
    bp = bread(ip->dev, IBLOCK(ip->inum, sb));
    dip = (সংরচনা dinode *)bp->data + ip->inum % IPB;
    ip->type = dip->type;
    ip->major = dip->major;
    ip->minor = dip->minor;
    ip->nlink = dip->nlink;
    ip->size = dip->size;
    memmove(ip->addrs, dip->addrs, পরিমাণ(ip->addrs));
    brelse(bp);
    ip->valid = 1;
    যদি (ip->type == 0)
      panic("ilock: no type");
  }
}

// Unlock the given inode.
শূন্য
iunlock(সংরচনা inode *ip)
{
  যদি (ip == 0 || !holdingsleep(&ip->lock) || ip->ref < 1)
    panic("iunlock");

  releasesleep(&ip->lock);
}

// Mark the on-disk inode free.
স্থির শূন্য
ifree(uint dev, uint inum)
{
  সংরচনা buf *bp = bread(dev, IBLOCK(inum, sb));
  সংরচনা dinode *dip = (সংরচনা dinode *)bp->data + inum % IPB;
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
শূন্য
iput(সংরচনা inode *ip)
{
  acquire(&itable.lock);

  // Last reference of an unlinked inode?  Capture dev/inum before ref--,
  // since once ref hits 0, ip may be recycled by a concurrent iget()
  // for a different inum.
  পূর্ণাঙ্ক last = (ip->ref == 1 && ip->valid && ip->nlink == 0);
  uint dev = ip->dev, inum = ip->inum;

  যদি (last) {
    // ip->ref == 1 means no other process can have ip locked.
    acquiresleep(&ip->lock);
    release(&itable.lock);

    itrunc(ip); // free the data blocks (type stays nonzero on disk)
    ip->valid = 0;

    releasesleep(&ip->lock);

    acquire(&itable.lock);
  }

  ip->ref--;
  release(&itable.lock);

  যদি (last)
    ifree(dev, inum); // now clear type on disk: inum becomes allocatable
}

// Common idiom: unlock, then put.
শূন্য
iunlockput(সংরচনা inode *ip)
{
  iunlock(ip);
  iput(ip);
}

শূন্য
ireclaim(পূর্ণাঙ্ক dev)
{
  বাবে (পূর্ণাঙ্ক inum = 1; inum < sb.ninodes; inum++) {
    সংরচনা inode *ip = 0;
    সংরচনা buf *bp = bread(dev, IBLOCK(inum, sb));
    সংরচনা dinode *dip = (সংরচনা dinode *)bp->data + inum % IPB;
    যদি (dip->type != 0 && dip->nlink == 0) { // is an orphaned inode
      printk("ireclaim: orphaned inode %d\n", inum);
      ip = iget(dev, inum);
    }
    brelse(bp);
    যদি (ip) {
      begin_op();
      ilock(ip);
      iunlock(ip);
      iput(ip);
      end_op();
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
স্থির uint
bmap(সংরচনা inode *ip, uint bn)
{
  uint addr, *a;
  সংরচনা buf *bp;

  যদি (bn < NDIRECT) {
    যদি ((addr = ip->addrs[bn]) == 0) {
      addr = balloc(ip->dev);
      যদি (addr == 0)
        ঘূৰাই 0;
      ip->addrs[bn] = addr;
    }
    ঘূৰাই addr;
  }
  bn -= NDIRECT;

  যদি (bn < NINDIRECT) {
    // Load indirect block, allocating if necessary.
    যদি ((addr = ip->addrs[NDIRECT]) == 0) {
      addr = balloc(ip->dev);
      যদি (addr == 0)
        ঘূৰাই 0;
      ip->addrs[NDIRECT] = addr;
    }
    bp = bread(ip->dev, addr);
    a = (uint *)bp->data;
    যদি ((addr = a[bn]) == 0) {
      addr = balloc(ip->dev);
      যদি (addr) {
        a[bn] = addr;
        log_write(bp);
      }
    }
    brelse(bp);
    ঘূৰাই addr;
  }

  panic("bmap: out of range");
}

// Truncate inode (discard contents).
// Caller must hold ip->lock.
শূন্য
itrunc(সংরচনা inode *ip)
{
  পূর্ণাঙ্ক i, j;
  সংরচনা buf *bp;
  uint *a;

  বাবে (i = 0; i < NDIRECT; i++) {
    যদি (ip->addrs[i]) {
      bfree(ip->dev, ip->addrs[i]);
      ip->addrs[i] = 0;
    }
  }

  যদি (ip->addrs[NDIRECT]) {
    bp = bread(ip->dev, ip->addrs[NDIRECT]);
    a = (uint *)bp->data;
    বাবে (j = 0; j < NINDIRECT; j++) {
      যদি (a[j])
        bfree(ip->dev, a[j]);
    }
    brelse(bp);
    bfree(ip->dev, ip->addrs[NDIRECT]);
    ip->addrs[NDIRECT] = 0;
  }

  ip->size = 0;
  iupdate(ip);
}

// Copy stat information from inode.
// Caller must hold ip->lock.
শূন্য
stati(সংরচনা inode *ip, সংরচনা stat *st)
{
  st->dev = ip->dev;
  st->ino = ip->inum;
  st->type = ip->type;
  st->nlink = ip->nlink;
  st->size = ip->size;
}

// Read data from inode.
// Caller must hold ip->lock.
// If user_dst==1, then dst is a user virtual address;
// otherwise, dst is a kernel address.
পূর্ণাঙ্ক
readi(সংরচনা inode *ip, পূর্ণাঙ্ক user_dst, uint64 dst, uint off, uint n)
{
  uint tot, m;
  সংরচনা buf *bp;

  যদি (off > ip->size || off + n < off)
    ঘূৰাই 0;
  যদি (off + n > ip->size)
    n = ip->size - off;

  বাবে (tot = 0; tot < n; tot += m, off += m, dst += m) {
    uint addr = bmap(ip, off / BSIZE);
    যদি (addr == 0)
      ভঙ্গ;
    bp = bread(ip->dev, addr);
    m = min(n - tot, BSIZE - off % BSIZE);
    যদি (either_copyout(user_dst, dst, bp->data + (off % BSIZE), m) == -1) {
      brelse(bp);
      tot = -1;
      ভঙ্গ;
    }
    brelse(bp);
  }
  ঘূৰাই tot;
}

// Write data to inode.
// Caller must hold ip->lock.
// If user_src==1, then src is a user virtual address;
// otherwise, src is a kernel address.
// Returns the number of bytes successfully written.
// If the return value is less than the requested n,
// there was an error of some kind.
পূর্ণাঙ্ক
writei(সংরচনা inode *ip, পূর্ণাঙ্ক user_src, uint64 src, uint off, uint n)
{
  uint tot, m;
  সংরচনা buf *bp;

  যদি (off > ip->size || off + n < off)
    ঘূৰাই -1;
  যদি (off + n > MAXFILE * BSIZE)
    ঘূৰাই -1;

  বাবে (tot = 0; tot < n; tot += m, off += m, src += m) {
    uint addr = bmap(ip, off / BSIZE);
    যদি (addr == 0)
      ভঙ্গ;
    bp = bread(ip->dev, addr);
    m = min(n - tot, BSIZE - off % BSIZE);
    যদি (either_copyin(bp->data + (off % BSIZE), user_src, src, m) == -1) {
      // Might have partially updated the block, so we need to log it.
      log_write(bp);
      brelse(bp);
      ভঙ্গ;
    }
    log_write(bp);
    brelse(bp);
  }

  যদি (off > ip->size)
    ip->size = off;

  // write the i-node back to disk even if the size didn't change
  // because the loop above might have called bmap() and added a new
  // block to ip->addrs[].
  iupdate(ip);

  ঘূৰাই tot;
}

// Directories

পূর্ণাঙ্ক
namecmp(ধ্রুৱ ৱর্ণ *s, ধ্রুৱ ৱর্ণ *t)
{
  ঘূৰাই strncmp(s, t, DIRSIZ);
}

// Look for a directory entry in a directory.
// If found, set *poff to byte offset of entry.
সংরচনা inode *
dirlookup(সংরচনা inode *dp, ৱর্ণ *name, uint *poff)
{
  uint off, inum;
  সংরচনা dirent de;

  যদি (dp->type != T_DIR)
    panic("dirlookup not DIR");

  বাবে (off = 0; off < dp->size; off += পরিমাণ(de)) {
    যদি (readi(dp, 0, (uint64)&de, off, পরিমাণ(de)) != পরিমাণ(de))
      panic("dirlookup read");
    যদি (de.inum == 0)
      অনুৱর্ত;
    যদি (namecmp(name, de.name) == 0) {
      // entry matches path element
      যদি (poff)
        *poff = off;
      inum = de.inum;
      ঘূৰাই iget(dp->dev, inum);
    }
  }

  ঘূৰাই 0;
}

// Write a new directory entry (name, inum) into the directory dp.
// Returns 0 on success, -1 on failure (e.g. out of disk blocks).
পূর্ণাঙ্ক
dirlink(সংরচনা inode *dp, ৱর্ণ *name, uint inum)
{
  পূর্ণাঙ্ক off;
  সংরচনা dirent de;
  সংরচনা inode *ip;

  // Check that name is not present.
  যদি ((ip = dirlookup(dp, name, 0)) != 0) {
    iput(ip);
    ঘূৰাই -1;
  }

  // Look for an empty dirent.
  বাবে (off = 0; off < dp->size; off += পরিমাণ(de)) {
    যদি (readi(dp, 0, (uint64)&de, off, পরিমাণ(de)) != পরিমাণ(de))
      panic("dirlink read");
    যদি (de.inum == 0)
      ভঙ্গ;
  }

  strncpy(de.name, name, DIRSIZ);
  de.inum = inum;
  যদি (writei(dp, 0, (uint64)&de, off, পরিমাণ(de)) != পরিমাণ(de))
    ঘূৰাই -1;

  ঘূৰাই 0;
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
স্থির ৱর্ণ *
skipelem(ৱর্ণ *path, ৱর্ণ *name)
{
  ৱর্ণ *s;
  পূর্ণাঙ্ক len;

  যেতিয়ালৈ (*path == '/')
    path++;
  যদি (*path == 0)
    ঘূৰাই 0;
  s = path;
  যেতিয়ালৈ (*path != '/' && *path != 0)
    path++;
  len = path - s;
  যদি (len >= DIRSIZ)
    memmove(name, s, DIRSIZ);
  অন্যথা {
    memmove(name, s, len);
    name[len] = 0;
  }
  যেতিয়ালৈ (*path == '/')
    path++;
  ঘূৰাই path;
}

// Look up and return the inode for a path name.
// If parent != 0, return the inode for the parent and copy the final
// path element into name, which must have room for DIRSIZ bytes.
// Must be called inside a transaction since it calls iput().
স্থির সংরচনা inode *
namex(ৱর্ণ *path, পূর্ণাঙ্ক nameiparent, ৱর্ণ *name)
{
  সংরচনা inode *ip, *next;

  যদি (*path == '/')
    ip = iget(ROOTDEV, ROOTINO);
  অন্যথা
    ip = idup(myproc()->cwd);

  যেতিয়ালৈ ((path = skipelem(path, name)) != 0) {
    ilock(ip);
    যদি (ip->type != T_DIR) {
      iunlockput(ip);
      ঘূৰাই 0;
    }
    যদি (ip->nlink == 0) {
      iunlockput(ip);
      ঘূৰাই 0;
    }
    যদি (nameiparent && *path == '\0') {
      // Stop one level early.
      iunlock(ip);
      ঘূৰাই ip;
    }
    যদি ((next = dirlookup(ip, name, 0)) == 0) {
      iunlockput(ip);
      ঘূৰাই 0;
    }
    iunlockput(ip);
    ip = next;
  }
  যদি (nameiparent) {
    iput(ip);
    ঘূৰাই 0;
  }
  ঘূৰাই ip;
}

সংরচনা inode *
namei(ৱর্ণ *path)
{
  ৱর্ণ name[DIRSIZ];
  ঘূৰাই namex(path, 0, name);
}

সংরচনা inode *
nameiparent(ৱর্ণ *path, ৱর্ণ *name)
{
  ঘূৰাই namex(path, 1, name);
}
