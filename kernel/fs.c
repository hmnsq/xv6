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

#شامل "types.h"
#شامل "riscv.h"
#شامل "defs.h"
#شامل "param.h"
#شامل "stat.h"
#شامل "spinlock.h"
#شامل "proc.h"
#شامل "sleeplock.h"
#شامل "fs.h"
#شامل "buf.h"
#شامل "file.h"

#define min(a, b) ((a) < (b) ? (a) : (b))
// there should be one superblock per disk device, but we run with
// only one device
ساختار superblock sb;

// Read the super block.
ایستا تهی
readsb(صحیح dev, ساختار superblock *sb)
{
  ساختار buf *bp;

  bp = bread(dev, 1);
  memmove(sb, bp->data, اندازه(*sb));
  brelse(bp);
}

// Init fs
تهی
fsinit(صحیح dev)
{
  readsb(dev, &sb);
  اگر (sb.magic != FSMAGIC)
    panic("invalid file system");
  initlog(dev, &sb);
  ireclaim(dev);
}

// Zero a block.
ایستا تهی
bzero(صحیح dev, صحیح bno)
{
  ساختار buf *bp;

  bp = bread(dev, bno);
  memset(bp->data, 0, BSIZE);
  log_write(bp);
  brelse(bp);
}

// Blocks.

// Allocate a zeroed disk block.
// returns 0 if out of disk space.
ایستا uint
balloc(uint dev)
{
  صحیح b, bi, m;
  ساختار buf *bp;

  bp = 0;
  برای (b = 0; b < sb.size; b += BPB) {
    bp = bread(dev, BBLOCK(b, sb));
    برای (bi = 0; bi < BPB && b + bi < sb.size; bi++) {
      m = 1 << (bi % 8);
      اگر ((bp->data[bi / 8] & m) == 0) { // Is block free?
        bp->data[bi / 8] |= m;           // Mark block in use.
        log_write(bp);
        brelse(bp);
        bzero(dev, b + bi);
        بازگشت b + bi;
      }
    }
    brelse(bp);
  }
  printk("balloc: out of blocks\n");
  بازگشت 0;
}

// Free a disk block.
ایستا تهی
bfree(صحیح dev, uint b)
{
  ساختار buf *bp;
  صحیح bi, m;

  bp = bread(dev, BBLOCK(b, sb));
  bi = b % BPB;
  m = 1 << (bi % 8);
  اگر ((bp->data[bi / 8] & m) == 0)
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

ساختار {
  ساختار spinlock lock;
  ساختار inode inode[NINODE];
} itable;

تهی
iinit()
{
  صحیح i = 0;

  initlock(&itable.lock, "itable");
  برای (i = 0; i < NINODE; i++) {
    initsleeplock(&itable.inode[i].lock, "inode");
  }
}

ایستا ساختار inode *iget(uint dev, uint inum);

// Allocate an inode on device dev.
// Mark it as allocated by  giving it type type.
// Returns an unlocked but allocated and referenced inode,
// or NULL if there is no free inode.
ساختار inode *
ialloc(uint dev, short type)
{
  صحیح inum;
  ساختار buf *bp;
  ساختار dinode *dip;

  برای (inum = 1; inum < sb.ninodes; inum++) {
    bp = bread(dev, IBLOCK(inum, sb));
    dip = (ساختار dinode *)bp->data + inum % IPB;
    اگر (dip->type == 0) { // a free inode
      memset(dip, 0, اندازه(*dip));
      dip->type = type;
      log_write(bp); // mark it allocated on the disk
      brelse(bp);
      بازگشت iget(dev, inum);
    }
    brelse(bp);
  }
  printk("ialloc: no inodes\n");
  بازگشت 0;
}

// Copy a modified in-memory inode to disk.
// Must be called after every change to an ip->xxx field
// that lives on disk.
// Caller must hold ip->lock.
تهی
iupdate(ساختار inode *ip)
{
  ساختار buf *bp;
  ساختار dinode *dip;

  bp = bread(ip->dev, IBLOCK(ip->inum, sb));
  dip = (ساختار dinode *)bp->data + ip->inum % IPB;
  dip->type = ip->type;
  dip->major = ip->major;
  dip->minor = ip->minor;
  dip->nlink = ip->nlink;
  dip->size = ip->size;
  memmove(dip->addrs, ip->addrs, اندازه(ip->addrs));
  log_write(bp);
  brelse(bp);
}

// Find the inode with number inum on device dev
// and return the in-memory copy. Does not lock
// the inode and does not read it from disk.
ایستا ساختار inode *
iget(uint dev, uint inum)
{
  ساختار inode *ip, *empty;

  acquire(&itable.lock);

  // Is the inode already in the table?
  empty = 0;
  برای (ip = &itable.inode[0]; ip < &itable.inode[NINODE]; ip++) {
    اگر (ip->ref > 0 && ip->dev == dev && ip->inum == inum) {
      ip->ref++;
      release(&itable.lock);
      بازگشت ip;
    }
    اگر (empty == 0 && ip->ref == 0) // Remember empty slot.
      empty = ip;
  }

  // Recycle an inode entry.
  اگر (empty == 0)
    panic("iget: no inodes");

  ip = empty;
  ip->dev = dev;
  ip->inum = inum;
  ip->ref = 1;
  ip->valid = 0;
  release(&itable.lock);

  بازگشت ip;
}

// Increment reference count for ip.
// Returns ip to enable ip = idup(ip1) idiom.
ساختار inode *
idup(ساختار inode *ip)
{
  acquire(&itable.lock);
  ip->ref++;
  release(&itable.lock);
  بازگشت ip;
}

// Lock the given inode.
// Reads the inode from disk if necessary.
تهی
ilock(ساختار inode *ip)
{
  ساختار buf *bp;
  ساختار dinode *dip;

  اگر (ip == 0 || ip->ref < 1)
    panic("ilock");

  acquiresleep(&ip->lock);

  اگر (ip->valid == 0) {
    bp = bread(ip->dev, IBLOCK(ip->inum, sb));
    dip = (ساختار dinode *)bp->data + ip->inum % IPB;
    ip->type = dip->type;
    ip->major = dip->major;
    ip->minor = dip->minor;
    ip->nlink = dip->nlink;
    ip->size = dip->size;
    memmove(ip->addrs, dip->addrs, اندازه(ip->addrs));
    brelse(bp);
    ip->valid = 1;
    اگر (ip->type == 0)
      panic("ilock: no type");
  }
}

// Unlock the given inode.
تهی
iunlock(ساختار inode *ip)
{
  اگر (ip == 0 || !holdingsleep(&ip->lock) || ip->ref < 1)
    panic("iunlock");

  releasesleep(&ip->lock);
}

// Mark the on-disk inode free.
ایستا تهی
ifree(uint dev, uint inum)
{
  ساختار buf *bp = bread(dev, IBLOCK(inum, sb));
  ساختار dinode *dip = (ساختار dinode *)bp->data + inum % IPB;
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
تهی
iput(ساختار inode *ip)
{
  acquire(&itable.lock);

  // Last reference of an unlinked inode?  Capture dev/inum before ref--,
  // since once ref hits 0, ip may be recycled by a concurrent iget()
  // for a different inum.
  صحیح last = (ip->ref == 1 && ip->valid && ip->nlink == 0);
  uint dev = ip->dev, inum = ip->inum;

  اگر (last) {
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

  اگر (last)
    ifree(dev, inum); // now clear type on disk: inum becomes allocatable
}

// Common idiom: unlock, then put.
تهی
iunlockput(ساختار inode *ip)
{
  iunlock(ip);
  iput(ip);
}

تهی
ireclaim(صحیح dev)
{
  برای (صحیح inum = 1; inum < sb.ninodes; inum++) {
    ساختار inode *ip = 0;
    ساختار buf *bp = bread(dev, IBLOCK(inum, sb));
    ساختار dinode *dip = (ساختار dinode *)bp->data + inum % IPB;
    اگر (dip->type != 0 && dip->nlink == 0) { // is an orphaned inode
      printk("ireclaim: orphaned inode %d\n", inum);
      ip = iget(dev, inum);
    }
    brelse(bp);
    اگر (ip) {
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
ایستا uint
bmap(ساختار inode *ip, uint bn)
{
  uint addr, *a;
  ساختار buf *bp;

  اگر (bn < NDIRECT) {
    اگر ((addr = ip->addrs[bn]) == 0) {
      addr = balloc(ip->dev);
      اگر (addr == 0)
        بازگشت 0;
      ip->addrs[bn] = addr;
    }
    بازگشت addr;
  }
  bn -= NDIRECT;

  اگر (bn < NINDIRECT) {
    // Load indirect block, allocating if necessary.
    اگر ((addr = ip->addrs[NDIRECT]) == 0) {
      addr = balloc(ip->dev);
      اگر (addr == 0)
        بازگشت 0;
      ip->addrs[NDIRECT] = addr;
    }
    bp = bread(ip->dev, addr);
    a = (uint *)bp->data;
    اگر ((addr = a[bn]) == 0) {
      addr = balloc(ip->dev);
      اگر (addr) {
        a[bn] = addr;
        log_write(bp);
      }
    }
    brelse(bp);
    بازگشت addr;
  }

  panic("bmap: out of range");
}

// Truncate inode (discard contents).
// Caller must hold ip->lock.
تهی
itrunc(ساختار inode *ip)
{
  صحیح i, j;
  ساختار buf *bp;
  uint *a;

  برای (i = 0; i < NDIRECT; i++) {
    اگر (ip->addrs[i]) {
      bfree(ip->dev, ip->addrs[i]);
      ip->addrs[i] = 0;
    }
  }

  اگر (ip->addrs[NDIRECT]) {
    bp = bread(ip->dev, ip->addrs[NDIRECT]);
    a = (uint *)bp->data;
    برای (j = 0; j < NINDIRECT; j++) {
      اگر (a[j])
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
تهی
stati(ساختار inode *ip, ساختار stat *st)
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
صحیح
readi(ساختار inode *ip, صحیح user_dst, uint64 dst, uint off, uint n)
{
  uint tot, m;
  ساختار buf *bp;

  اگر (off > ip->size || off + n < off)
    بازگشت 0;
  اگر (off + n > ip->size)
    n = ip->size - off;

  برای (tot = 0; tot < n; tot += m, off += m, dst += m) {
    uint addr = bmap(ip, off / BSIZE);
    اگر (addr == 0)
      بشکن;
    bp = bread(ip->dev, addr);
    m = min(n - tot, BSIZE - off % BSIZE);
    اگر (either_copyout(user_dst, dst, bp->data + (off % BSIZE), m) == -1) {
      brelse(bp);
      tot = -1;
      بشکن;
    }
    brelse(bp);
  }
  بازگشت tot;
}

// Write data to inode.
// Caller must hold ip->lock.
// If user_src==1, then src is a user virtual address;
// otherwise, src is a kernel address.
// Returns the number of bytes successfully written.
// If the return value is less than the requested n,
// there was an error of some kind.
صحیح
writei(ساختار inode *ip, صحیح user_src, uint64 src, uint off, uint n)
{
  uint tot, m;
  ساختار buf *bp;

  اگر (off > ip->size || off + n < off)
    بازگشت -1;
  اگر (off + n > MAXFILE * BSIZE)
    بازگشت -1;

  برای (tot = 0; tot < n; tot += m, off += m, src += m) {
    uint addr = bmap(ip, off / BSIZE);
    اگر (addr == 0)
      بشکن;
    bp = bread(ip->dev, addr);
    m = min(n - tot, BSIZE - off % BSIZE);
    اگر (either_copyin(bp->data + (off % BSIZE), user_src, src, m) == -1) {
      // Might have partially updated the block, so we need to log it.
      log_write(bp);
      brelse(bp);
      بشکن;
    }
    log_write(bp);
    brelse(bp);
  }

  اگر (off > ip->size)
    ip->size = off;

  // write the i-node back to disk even if the size didn't change
  // because the loop above might have called bmap() and added a new
  // block to ip->addrs[].
  iupdate(ip);

  بازگشت tot;
}

// Directories

صحیح
namecmp(ثابت نویسه *s, ثابت نویسه *t)
{
  بازگشت strncmp(s, t, DIRSIZ);
}

// Look for a directory entry in a directory.
// If found, set *poff to byte offset of entry.
ساختار inode *
dirlookup(ساختار inode *dp, نویسه *name, uint *poff)
{
  uint off, inum;
  ساختار dirent de;

  اگر (dp->type != T_DIR)
    panic("dirlookup not DIR");

  برای (off = 0; off < dp->size; off += اندازه(de)) {
    اگر (readi(dp, 0, (uint64)&de, off, اندازه(de)) != اندازه(de))
      panic("dirlookup read");
    اگر (de.inum == 0)
      ادامه;
    اگر (namecmp(name, de.name) == 0) {
      // entry matches path element
      اگر (poff)
        *poff = off;
      inum = de.inum;
      بازگشت iget(dp->dev, inum);
    }
  }

  بازگشت 0;
}

// Write a new directory entry (name, inum) into the directory dp.
// Returns 0 on success, -1 on failure (e.g. out of disk blocks).
صحیح
dirlink(ساختار inode *dp, نویسه *name, uint inum)
{
  صحیح off;
  ساختار dirent de;
  ساختار inode *ip;

  // Check that name is not present.
  اگر ((ip = dirlookup(dp, name, 0)) != 0) {
    iput(ip);
    بازگشت -1;
  }

  // Look for an empty dirent.
  برای (off = 0; off < dp->size; off += اندازه(de)) {
    اگر (readi(dp, 0, (uint64)&de, off, اندازه(de)) != اندازه(de))
      panic("dirlink read");
    اگر (de.inum == 0)
      بشکن;
  }

  strncpy(de.name, name, DIRSIZ);
  de.inum = inum;
  اگر (writei(dp, 0, (uint64)&de, off, اندازه(de)) != اندازه(de))
    بازگشت -1;

  بازگشت 0;
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
ایستا نویسه *
skipelem(نویسه *path, نویسه *name)
{
  نویسه *s;
  صحیح len;

  وقتی (*path == '/')
    path++;
  اگر (*path == 0)
    بازگشت 0;
  s = path;
  وقتی (*path != '/' && *path != 0)
    path++;
  len = path - s;
  اگر (len >= DIRSIZ)
    memmove(name, s, DIRSIZ);
  وگرنه {
    memmove(name, s, len);
    name[len] = 0;
  }
  وقتی (*path == '/')
    path++;
  بازگشت path;
}

// Look up and return the inode for a path name.
// If parent != 0, return the inode for the parent and copy the final
// path element into name, which must have room for DIRSIZ bytes.
// Must be called inside a transaction since it calls iput().
ایستا ساختار inode *
namex(نویسه *path, صحیح nameiparent, نویسه *name)
{
  ساختار inode *ip, *next;

  اگر (*path == '/')
    ip = iget(ROOTDEV, ROOTINO);
  وگرنه
    ip = idup(myproc()->cwd);

  وقتی ((path = skipelem(path, name)) != 0) {
    ilock(ip);
    اگر (ip->type != T_DIR) {
      iunlockput(ip);
      بازگشت 0;
    }
    اگر (ip->nlink == 0) {
      iunlockput(ip);
      بازگشت 0;
    }
    اگر (nameiparent && *path == '\0') {
      // Stop one level early.
      iunlock(ip);
      بازگشت ip;
    }
    اگر ((next = dirlookup(ip, name, 0)) == 0) {
      iunlockput(ip);
      بازگشت 0;
    }
    iunlockput(ip);
    ip = next;
  }
  اگر (nameiparent) {
    iput(ip);
    بازگشت 0;
  }
  بازگشت ip;
}

ساختار inode *
namei(نویسه *path)
{
  نویسه name[DIRSIZ];
  بازگشت namex(path, 0, name);
}

ساختار inode *
nameiparent(نویسه *path, نویسه *name)
{
  بازگشت namex(path, 1, name);
}
