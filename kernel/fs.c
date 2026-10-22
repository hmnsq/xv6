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

#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "stat.h"
#include "spinlock.h"
#include "proc.h"
#include "sleeplock.h"
#include "fs.h"
#include "buf.h"
#include "file.h"

#define min(a, b) ((a) < (b) ? (a) : (b))
// there should be one superblock per disk device, but we run with
// only one device
sa_nracanaa superblock sb;

// Read the super block.
static void
readsb(ཧྲིལ dev, sa_nracanaa superblock *sb)
{
  sa_nracanaa buf *bp;

  bp = bread(dev, 1);
  memmove(sb, bp->data, parimaa_na(*sb));
  brelse(bp);
}

// Init fs
void
fsinit(ཧྲིལ dev)
{
  readsb(dev, &sb);
  གལ་ཏེ (sb.magic != FSMAGIC)
    panic("invalid file system");
  initlog(dev, &sb);
  ireclaim(dev);
}

// Zero a block.
static void
bzero(ཧྲིལ dev, ཧྲིལ bno)
{
  sa_nracanaa buf *bp;

  bp = bread(dev, bno);
  memset(bp->data, 0, BSIZE);
  log_write(bp);
  brelse(bp);
}

// Blocks.

// Allocate a zeroed disk block.
// returns 0 if out of disk space.
static uint
balloc(uint dev)
{
  ཧྲིལ b, bi, m;
  sa_nracanaa buf *bp;

  bp = 0;
  ཆེད་དུ (b = 0; b < sb.size; b += BPB) {
    bp = bread(dev, BBLOCK(b, sb));
    ཆེད་དུ (bi = 0; bi < BPB && b + bi < sb.size; bi++) {
      m = 1 << (bi % 8);
      གལ་ཏེ ((bp->data[bi / 8] & m) == 0) { // Is block free?
        bp->data[bi / 8] |= m;           // Mark block in use.
        log_write(bp);
        brelse(bp);
        bzero(dev, b + bi);
        ཕྱིར b + bi;
      }
    }
    brelse(bp);
  }
  printk("balloc: out of blocks\n");
  ཕྱིར 0;
}

// Free a disk block.
static void
bfree(ཧྲིལ dev, uint b)
{
  sa_nracanaa buf *bp;
  ཧྲིལ bi, m;

  bp = bread(dev, BBLOCK(b, sb));
  bi = b % BPB;
  m = 1 << (bi % 8);
  གལ་ཏེ ((bp->data[bi / 8] & m) == 0)
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

sa_nracanaa {
  sa_nracanaa spinlock lock;
  sa_nracanaa inode inode[NINODE];
} itable;

void
iinit()
{
  ཧྲིལ i = 0;

  initlock(&itable.lock, "itable");
  ཆེད་དུ (i = 0; i < NINODE; i++) {
    initsleeplock(&itable.inode[i].lock, "inode");
  }
}

static sa_nracanaa inode *iget(uint dev, uint inum);

// Allocate an inode on device dev.
// Mark it as allocated by  giving it type type.
// Returns an unlocked but allocated and referenced inode,
// or NULL if there is no free inode.
sa_nracanaa inode *
ialloc(uint dev, short type)
{
  ཧྲིལ inum;
  sa_nracanaa buf *bp;
  sa_nracanaa dinode *dip;

  ཆེད་དུ (inum = 1; inum < sb.ninodes; inum++) {
    bp = bread(dev, IBLOCK(inum, sb));
    dip = (sa_nracanaa dinode *)bp->data + inum % IPB;
    གལ་ཏེ (dip->type == 0) { // a free inode
      memset(dip, 0, parimaa_na(*dip));
      dip->type = type;
      log_write(bp); // mark it allocated on the disk
      brelse(bp);
      ཕྱིར iget(dev, inum);
    }
    brelse(bp);
  }
  printk("ialloc: no inodes\n");
  ཕྱིར 0;
}

// Copy a modified in-memory inode to disk.
// Must be called after every change to an ip->xxx field
// that lives on disk.
// Caller must hold ip->lock.
void
iupdate(sa_nracanaa inode *ip)
{
  sa_nracanaa buf *bp;
  sa_nracanaa dinode *dip;

  bp = bread(ip->dev, IBLOCK(ip->inum, sb));
  dip = (sa_nracanaa dinode *)bp->data + ip->inum % IPB;
  dip->type = ip->type;
  dip->major = ip->major;
  dip->minor = ip->minor;
  dip->nlink = ip->nlink;
  dip->size = ip->size;
  memmove(dip->addrs, ip->addrs, parimaa_na(ip->addrs));
  log_write(bp);
  brelse(bp);
}

// Find the inode with number inum on device dev
// and return the in-memory copy. Does not lock
// the inode and does not read it from disk.
static sa_nracanaa inode *
iget(uint dev, uint inum)
{
  sa_nracanaa inode *ip, *empty;

  acquire(&itable.lock);

  // Is the inode already in the table?
  empty = 0;
  ཆེད་དུ (ip = &itable.inode[0]; ip < &itable.inode[NINODE]; ip++) {
    གལ་ཏེ (ip->ref > 0 && ip->dev == dev && ip->inum == inum) {
      ip->ref++;
      release(&itable.lock);
      ཕྱིར ip;
    }
    གལ་ཏེ (empty == 0 && ip->ref == 0) // Remember empty slot.
      empty = ip;
  }

  // Recycle an inode entry.
  གལ་ཏེ (empty == 0)
    panic("iget: no inodes");

  ip = empty;
  ip->dev = dev;
  ip->inum = inum;
  ip->ref = 1;
  ip->valid = 0;
  release(&itable.lock);

  ཕྱིར ip;
}

// Increment reference count for ip.
// Returns ip to enable ip = idup(ip1) idiom.
sa_nracanaa inode *
idup(sa_nracanaa inode *ip)
{
  acquire(&itable.lock);
  ip->ref++;
  release(&itable.lock);
  ཕྱིར ip;
}

// Lock the given inode.
// Reads the inode from disk if necessary.
void
ilock(sa_nracanaa inode *ip)
{
  sa_nracanaa buf *bp;
  sa_nracanaa dinode *dip;

  གལ་ཏེ (ip == 0 || ip->ref < 1)
    panic("ilock");

  acquiresleep(&ip->lock);

  གལ་ཏེ (ip->valid == 0) {
    bp = bread(ip->dev, IBLOCK(ip->inum, sb));
    dip = (sa_nracanaa dinode *)bp->data + ip->inum % IPB;
    ip->type = dip->type;
    ip->major = dip->major;
    ip->minor = dip->minor;
    ip->nlink = dip->nlink;
    ip->size = dip->size;
    memmove(ip->addrs, dip->addrs, parimaa_na(ip->addrs));
    brelse(bp);
    ip->valid = 1;
    གལ་ཏེ (ip->type == 0)
      panic("ilock: no type");
  }
}

// Unlock the given inode.
void
iunlock(sa_nracanaa inode *ip)
{
  གལ་ཏེ (ip == 0 || !holdingsleep(&ip->lock) || ip->ref < 1)
    panic("iunlock");

  releasesleep(&ip->lock);
}

// Mark the on-disk inode free.
static void
ifree(uint dev, uint inum)
{
  sa_nracanaa buf *bp = bread(dev, IBLOCK(inum, sb));
  sa_nracanaa dinode *dip = (sa_nracanaa dinode *)bp->data + inum % IPB;
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
void
iput(sa_nracanaa inode *ip)
{
  acquire(&itable.lock);

  // Last reference of an unlinked inode?  Capture dev/inum before ref--,
  // since once ref hits 0, ip may be recycled by a concurrent iget()
  // for a different inum.
  ཧྲིལ last = (ip->ref == 1 && ip->valid && ip->nlink == 0);
  uint dev = ip->dev, inum = ip->inum;

  གལ་ཏེ (last) {
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

  གལ་ཏེ (last)
    ifree(dev, inum); // now clear type on disk: inum becomes allocatable
}

// Common idiom: unlock, then put.
void
iunlockput(sa_nracanaa inode *ip)
{
  iunlock(ip);
  iput(ip);
}

void
ireclaim(ཧྲིལ dev)
{
  ཆེད་དུ (ཧྲིལ inum = 1; inum < sb.ninodes; inum++) {
    sa_nracanaa inode *ip = 0;
    sa_nracanaa buf *bp = bread(dev, IBLOCK(inum, sb));
    sa_nracanaa dinode *dip = (sa_nracanaa dinode *)bp->data + inum % IPB;
    གལ་ཏེ (dip->type != 0 && dip->nlink == 0) { // is an orphaned inode
      printk("ireclaim: orphaned inode %d\n", inum);
      ip = iget(dev, inum);
    }
    brelse(bp);
    གལ་ཏེ (ip) {
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
static uint
bmap(sa_nracanaa inode *ip, uint bn)
{
  uint addr, *a;
  sa_nracanaa buf *bp;

  གལ་ཏེ (bn < NDIRECT) {
    གལ་ཏེ ((addr = ip->addrs[bn]) == 0) {
      addr = balloc(ip->dev);
      གལ་ཏེ (addr == 0)
        ཕྱིར 0;
      ip->addrs[bn] = addr;
    }
    ཕྱིར addr;
  }
  bn -= NDIRECT;

  གལ་ཏེ (bn < NINDIRECT) {
    // Load indirect block, allocating if necessary.
    གལ་ཏེ ((addr = ip->addrs[NDIRECT]) == 0) {
      addr = balloc(ip->dev);
      གལ་ཏེ (addr == 0)
        ཕྱིར 0;
      ip->addrs[NDIRECT] = addr;
    }
    bp = bread(ip->dev, addr);
    a = (uint *)bp->data;
    གལ་ཏེ ((addr = a[bn]) == 0) {
      addr = balloc(ip->dev);
      གལ་ཏེ (addr) {
        a[bn] = addr;
        log_write(bp);
      }
    }
    brelse(bp);
    ཕྱིར addr;
  }

  panic("bmap: out of range");
}

// Truncate inode (discard contents).
// Caller must hold ip->lock.
void
itrunc(sa_nracanaa inode *ip)
{
  ཧྲིལ i, j;
  sa_nracanaa buf *bp;
  uint *a;

  ཆེད་དུ (i = 0; i < NDIRECT; i++) {
    གལ་ཏེ (ip->addrs[i]) {
      bfree(ip->dev, ip->addrs[i]);
      ip->addrs[i] = 0;
    }
  }

  གལ་ཏེ (ip->addrs[NDIRECT]) {
    bp = bread(ip->dev, ip->addrs[NDIRECT]);
    a = (uint *)bp->data;
    ཆེད་དུ (j = 0; j < NINDIRECT; j++) {
      གལ་ཏེ (a[j])
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
void
stati(sa_nracanaa inode *ip, sa_nracanaa stat *st)
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
ཧྲིལ
readi(sa_nracanaa inode *ip, ཧྲིལ user_dst, uint64 dst, uint off, uint n)
{
  uint tot, m;
  sa_nracanaa buf *bp;

  གལ་ཏེ (off > ip->size || off + n < off)
    ཕྱིར 0;
  གལ་ཏེ (off + n > ip->size)
    n = ip->size - off;

  ཆེད་དུ (tot = 0; tot < n; tot += m, off += m, dst += m) {
    uint addr = bmap(ip, off / BSIZE);
    གལ་ཏེ (addr == 0)
      bha_nga;
    bp = bread(ip->dev, addr);
    m = min(n - tot, BSIZE - off % BSIZE);
    གལ་ཏེ (either_copyout(user_dst, dst, bp->data + (off % BSIZE), m) == -1) {
      brelse(bp);
      tot = -1;
      bha_nga;
    }
    brelse(bp);
  }
  ཕྱིར tot;
}

// Write data to inode.
// Caller must hold ip->lock.
// If user_src==1, then src is a user virtual address;
// otherwise, src is a kernel address.
// Returns the number of bytes successfully written.
// If the return value is less than the requested n,
// there was an error of some kind.
ཧྲིལ
writei(sa_nracanaa inode *ip, ཧྲིལ user_src, uint64 src, uint off, uint n)
{
  uint tot, m;
  sa_nracanaa buf *bp;

  གལ་ཏེ (off > ip->size || off + n < off)
    ཕྱིར -1;
  གལ་ཏེ (off + n > MAXFILE * BSIZE)
    ཕྱིར -1;

  ཆེད་དུ (tot = 0; tot < n; tot += m, off += m, src += m) {
    uint addr = bmap(ip, off / BSIZE);
    གལ་ཏེ (addr == 0)
      bha_nga;
    bp = bread(ip->dev, addr);
    m = min(n - tot, BSIZE - off % BSIZE);
    གལ་ཏེ (either_copyin(bp->data + (off % BSIZE), user_src, src, m) == -1) {
      // Might have partially updated the block, so we need to log it.
      log_write(bp);
      brelse(bp);
      bha_nga;
    }
    log_write(bp);
    brelse(bp);
  }

  གལ་ཏེ (off > ip->size)
    ip->size = off;

  // write the i-node back to disk even if the size didn't change
  // because the loop above might have called bmap() and added a new
  // block to ip->addrs[].
  iupdate(ip);

  ཕྱིར tot;
}

// Directories

ཧྲིལ
namecmp(const var_na *s, const var_na *t)
{
  ཕྱིར strncmp(s, t, DIRSIZ);
}

// Look for a directory entry in a directory.
// If found, set *poff to byte offset of entry.
sa_nracanaa inode *
dirlookup(sa_nracanaa inode *dp, var_na *name, uint *poff)
{
  uint off, inum;
  sa_nracanaa dirent de;

  གལ་ཏེ (dp->type != T_DIR)
    panic("dirlookup not DIR");

  ཆེད་དུ (off = 0; off < dp->size; off += parimaa_na(de)) {
    གལ་ཏེ (readi(dp, 0, (uint64)&de, off, parimaa_na(de)) != parimaa_na(de))
      panic("dirlookup read");
    གལ་ཏེ (de.inum == 0)
      continue;
    གལ་ཏེ (namecmp(name, de.name) == 0) {
      // entry matches path element
      གལ་ཏེ (poff)
        *poff = off;
      inum = de.inum;
      ཕྱིར iget(dp->dev, inum);
    }
  }

  ཕྱིར 0;
}

// Write a new directory entry (name, inum) into the directory dp.
// Returns 0 on success, -1 on failure (e.g. out of disk blocks).
ཧྲིལ
dirlink(sa_nracanaa inode *dp, var_na *name, uint inum)
{
  ཧྲིལ off;
  sa_nracanaa dirent de;
  sa_nracanaa inode *ip;

  // Check that name is not present.
  གལ་ཏེ ((ip = dirlookup(dp, name, 0)) != 0) {
    iput(ip);
    ཕྱིར -1;
  }

  // Look for an empty dirent.
  ཆེད་དུ (off = 0; off < dp->size; off += parimaa_na(de)) {
    གལ་ཏེ (readi(dp, 0, (uint64)&de, off, parimaa_na(de)) != parimaa_na(de))
      panic("dirlink read");
    གལ་ཏེ (de.inum == 0)
      bha_nga;
  }

  strncpy(de.name, name, DIRSIZ);
  de.inum = inum;
  གལ་ཏེ (writei(dp, 0, (uint64)&de, off, parimaa_na(de)) != parimaa_na(de))
    ཕྱིར -1;

  ཕྱིར 0;
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
static var_na *
skipelem(var_na *path, var_na *name)
{
  var_na *s;
  ཧྲིལ len;

  སྐབས (*path == '/')
    path++;
  གལ་ཏེ (*path == 0)
    ཕྱིར 0;
  s = path;
  སྐབས (*path != '/' && *path != 0)
    path++;
  len = path - s;
  གལ་ཏེ (len >= DIRSIZ)
    memmove(name, s, DIRSIZ);
  དེ་མིན {
    memmove(name, s, len);
    name[len] = 0;
  }
  སྐབས (*path == '/')
    path++;
  ཕྱིར path;
}

// Look up and return the inode for a path name.
// If parent != 0, return the inode for the parent and copy the final
// path element into name, which must have room for DIRSIZ bytes.
// Must be called inside a transaction since it calls iput().
static sa_nracanaa inode *
namex(var_na *path, ཧྲིལ nameiparent, var_na *name)
{
  sa_nracanaa inode *ip, *next;

  གལ་ཏེ (*path == '/')
    ip = iget(ROOTDEV, ROOTINO);
  དེ་མིན
    ip = idup(myproc()->cwd);

  སྐབས ((path = skipelem(path, name)) != 0) {
    ilock(ip);
    གལ་ཏེ (ip->type != T_DIR) {
      iunlockput(ip);
      ཕྱིར 0;
    }
    གལ་ཏེ (ip->nlink == 0) {
      iunlockput(ip);
      ཕྱིར 0;
    }
    གལ་ཏེ (nameiparent && *path == '\0') {
      // Stop one level early.
      iunlock(ip);
      ཕྱིར ip;
    }
    གལ་ཏེ ((next = dirlookup(ip, name, 0)) == 0) {
      iunlockput(ip);
      ཕྱིར 0;
    }
    iunlockput(ip);
    ip = next;
  }
  གལ་ཏེ (nameiparent) {
    iput(ip);
    ཕྱིར 0;
  }
  ཕྱིར ip;
}

sa_nracanaa inode *
namei(var_na *path)
{
  var_na name[DIRSIZ];
  ཕྱིར namex(path, 0, name);
}

sa_nracanaa inode *
nameiparent(var_na *path, var_na *name)
{
  ཕྱིར namex(path, 1, name);
}
