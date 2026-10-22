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

#അന്തര്ഭാവ "types.h"
#അന്തര്ഭാവ "riscv.h"
#അന്തര്ഭാവ "defs.h"
#അന്തര്ഭാവ "param.h"
#അന്തര്ഭാവ "stat.h"
#അന്തര്ഭാവ "spinlock.h"
#അന്തര്ഭാവ "proc.h"
#അന്തര്ഭാവ "sleeplock.h"
#അന്തര്ഭാവ "fs.h"
#അന്തര്ഭാവ "buf.h"
#അന്തര്ഭാവ "file.h"

#define min(a, b) ((a) < (b) ? (a) : (b))
// there should be one superblock per disk device, but we run with
// only one device
ഘടന superblock sb;

// Read the super block.
സ്ഥിതം ശൂന്യം
readsb(പൂർണ്ണസംഖ്യ dev, ഘടന superblock *sb)
{
  ഘടന buf *bp;

  bp = bread(dev, 1);
  memmove(sb, bp->data, വലുപ്പം(*sb));
  brelse(bp);
}

// Init fs
ശൂന്യം
fsinit(പൂർണ്ണസംഖ്യ dev)
{
  readsb(dev, &sb);
  എങ്കിൽ (sb.magic != FSMAGIC)
    panic("invalid file system");
  initlog(dev, &sb);
  ireclaim(dev);
}

// Zero a block.
സ്ഥിതം ശൂന്യം
bzero(പൂർണ്ണസംഖ്യ dev, പൂർണ്ണസംഖ്യ bno)
{
  ഘടന buf *bp;

  bp = bread(dev, bno);
  memset(bp->data, 0, BSIZE);
  log_write(bp);
  brelse(bp);
}

// Blocks.

// Allocate a zeroed disk block.
// returns 0 if out of disk space.
സ്ഥിതം uint
balloc(uint dev)
{
  പൂർണ്ണസംഖ്യ b, bi, m;
  ഘടന buf *bp;

  bp = 0;
  വേണ്ടി (b = 0; b < sb.size; b += BPB) {
    bp = bread(dev, BBLOCK(b, sb));
    വേണ്ടി (bi = 0; bi < BPB && b + bi < sb.size; bi++) {
      m = 1 << (bi % 8);
      എങ്കിൽ ((bp->data[bi / 8] & m) == 0) { // Is block free?
        bp->data[bi / 8] |= m;           // Mark block in use.
        log_write(bp);
        brelse(bp);
        bzero(dev, b + bi);
        തിരികെ b + bi;
      }
    }
    brelse(bp);
  }
  printk("balloc: out of blocks\n");
  തിരികെ 0;
}

// Free a disk block.
സ്ഥിതം ശൂന്യം
bfree(പൂർണ്ണസംഖ്യ dev, uint b)
{
  ഘടന buf *bp;
  പൂർണ്ണസംഖ്യ bi, m;

  bp = bread(dev, BBLOCK(b, sb));
  bi = b % BPB;
  m = 1 << (bi % 8);
  എങ്കിൽ ((bp->data[bi / 8] & m) == 0)
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

ഘടന {
  ഘടന spinlock lock;
  ഘടന inode inode[NINODE];
} itable;

ശൂന്യം
iinit()
{
  പൂർണ്ണസംഖ്യ i = 0;

  initlock(&itable.lock, "itable");
  വേണ്ടി (i = 0; i < NINODE; i++) {
    initsleeplock(&itable.inode[i].lock, "inode");
  }
}

സ്ഥിതം ഘടന inode *iget(uint dev, uint inum);

// Allocate an inode on device dev.
// Mark it as allocated by  giving it type type.
// Returns an unlocked but allocated and referenced inode,
// or NULL if there is no free inode.
ഘടന inode *
ialloc(uint dev, short type)
{
  പൂർണ്ണസംഖ്യ inum;
  ഘടന buf *bp;
  ഘടന dinode *dip;

  വേണ്ടി (inum = 1; inum < sb.ninodes; inum++) {
    bp = bread(dev, IBLOCK(inum, sb));
    dip = (ഘടന dinode *)bp->data + inum % IPB;
    എങ്കിൽ (dip->type == 0) { // a free inode
      memset(dip, 0, വലുപ്പം(*dip));
      dip->type = type;
      log_write(bp); // mark it allocated on the disk
      brelse(bp);
      തിരികെ iget(dev, inum);
    }
    brelse(bp);
  }
  printk("ialloc: no inodes\n");
  തിരികെ 0;
}

// Copy a modified in-memory inode to disk.
// Must be called after every change to an ip->xxx field
// that lives on disk.
// Caller must hold ip->lock.
ശൂന്യം
iupdate(ഘടന inode *ip)
{
  ഘടന buf *bp;
  ഘടന dinode *dip;

  bp = bread(ip->dev, IBLOCK(ip->inum, sb));
  dip = (ഘടന dinode *)bp->data + ip->inum % IPB;
  dip->type = ip->type;
  dip->major = ip->major;
  dip->minor = ip->minor;
  dip->nlink = ip->nlink;
  dip->size = ip->size;
  memmove(dip->addrs, ip->addrs, വലുപ്പം(ip->addrs));
  log_write(bp);
  brelse(bp);
}

// Find the inode with number inum on device dev
// and return the in-memory copy. Does not lock
// the inode and does not read it from disk.
സ്ഥിതം ഘടന inode *
iget(uint dev, uint inum)
{
  ഘടന inode *ip, *empty;

  acquire(&itable.lock);

  // Is the inode already in the table?
  empty = 0;
  വേണ്ടി (ip = &itable.inode[0]; ip < &itable.inode[NINODE]; ip++) {
    എങ്കിൽ (ip->ref > 0 && ip->dev == dev && ip->inum == inum) {
      ip->ref++;
      release(&itable.lock);
      തിരികെ ip;
    }
    എങ്കിൽ (empty == 0 && ip->ref == 0) // Remember empty slot.
      empty = ip;
  }

  // Recycle an inode entry.
  എങ്കിൽ (empty == 0)
    panic("iget: no inodes");

  ip = empty;
  ip->dev = dev;
  ip->inum = inum;
  ip->ref = 1;
  ip->valid = 0;
  release(&itable.lock);

  തിരികെ ip;
}

// Increment reference count for ip.
// Returns ip to enable ip = idup(ip1) idiom.
ഘടന inode *
idup(ഘടന inode *ip)
{
  acquire(&itable.lock);
  ip->ref++;
  release(&itable.lock);
  തിരികെ ip;
}

// Lock the given inode.
// Reads the inode from disk if necessary.
ശൂന്യം
ilock(ഘടന inode *ip)
{
  ഘടന buf *bp;
  ഘടന dinode *dip;

  എങ്കിൽ (ip == 0 || ip->ref < 1)
    panic("ilock");

  acquiresleep(&ip->lock);

  എങ്കിൽ (ip->valid == 0) {
    bp = bread(ip->dev, IBLOCK(ip->inum, sb));
    dip = (ഘടന dinode *)bp->data + ip->inum % IPB;
    ip->type = dip->type;
    ip->major = dip->major;
    ip->minor = dip->minor;
    ip->nlink = dip->nlink;
    ip->size = dip->size;
    memmove(ip->addrs, dip->addrs, വലുപ്പം(ip->addrs));
    brelse(bp);
    ip->valid = 1;
    എങ്കിൽ (ip->type == 0)
      panic("ilock: no type");
  }
}

// Unlock the given inode.
ശൂന്യം
iunlock(ഘടന inode *ip)
{
  എങ്കിൽ (ip == 0 || !holdingsleep(&ip->lock) || ip->ref < 1)
    panic("iunlock");

  releasesleep(&ip->lock);
}

// Mark the on-disk inode free.
സ്ഥിതം ശൂന്യം
ifree(uint dev, uint inum)
{
  ഘടന buf *bp = bread(dev, IBLOCK(inum, sb));
  ഘടന dinode *dip = (ഘടന dinode *)bp->data + inum % IPB;
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
ശൂന്യം
iput(ഘടന inode *ip)
{
  acquire(&itable.lock);

  // Last reference of an unlinked inode?  Capture dev/inum before ref--,
  // since once ref hits 0, ip may be recycled by a concurrent iget()
  // for a different inum.
  പൂർണ്ണസംഖ്യ last = (ip->ref == 1 && ip->valid && ip->nlink == 0);
  uint dev = ip->dev, inum = ip->inum;

  എങ്കിൽ (last) {
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

  എങ്കിൽ (last)
    ifree(dev, inum); // now clear type on disk: inum becomes allocatable
}

// Common idiom: unlock, then put.
ശൂന്യം
iunlockput(ഘടന inode *ip)
{
  iunlock(ip);
  iput(ip);
}

ശൂന്യം
ireclaim(പൂർണ്ണസംഖ്യ dev)
{
  വേണ്ടി (പൂർണ്ണസംഖ്യ inum = 1; inum < sb.ninodes; inum++) {
    ഘടന inode *ip = 0;
    ഘടന buf *bp = bread(dev, IBLOCK(inum, sb));
    ഘടന dinode *dip = (ഘടന dinode *)bp->data + inum % IPB;
    എങ്കിൽ (dip->type != 0 && dip->nlink == 0) { // is an orphaned inode
      printk("ireclaim: orphaned inode %d\n", inum);
      ip = iget(dev, inum);
    }
    brelse(bp);
    എങ്കിൽ (ip) {
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
സ്ഥിതം uint
bmap(ഘടന inode *ip, uint bn)
{
  uint addr, *a;
  ഘടന buf *bp;

  എങ്കിൽ (bn < NDIRECT) {
    എങ്കിൽ ((addr = ip->addrs[bn]) == 0) {
      addr = balloc(ip->dev);
      എങ്കിൽ (addr == 0)
        തിരികെ 0;
      ip->addrs[bn] = addr;
    }
    തിരികെ addr;
  }
  bn -= NDIRECT;

  എങ്കിൽ (bn < NINDIRECT) {
    // Load indirect block, allocating if necessary.
    എങ്കിൽ ((addr = ip->addrs[NDIRECT]) == 0) {
      addr = balloc(ip->dev);
      എങ്കിൽ (addr == 0)
        തിരികെ 0;
      ip->addrs[NDIRECT] = addr;
    }
    bp = bread(ip->dev, addr);
    a = (uint *)bp->data;
    എങ്കിൽ ((addr = a[bn]) == 0) {
      addr = balloc(ip->dev);
      എങ്കിൽ (addr) {
        a[bn] = addr;
        log_write(bp);
      }
    }
    brelse(bp);
    തിരികെ addr;
  }

  panic("bmap: out of range");
}

// Truncate inode (discard contents).
// Caller must hold ip->lock.
ശൂന്യം
itrunc(ഘടന inode *ip)
{
  പൂർണ്ണസംഖ്യ i, j;
  ഘടന buf *bp;
  uint *a;

  വേണ്ടി (i = 0; i < NDIRECT; i++) {
    എങ്കിൽ (ip->addrs[i]) {
      bfree(ip->dev, ip->addrs[i]);
      ip->addrs[i] = 0;
    }
  }

  എങ്കിൽ (ip->addrs[NDIRECT]) {
    bp = bread(ip->dev, ip->addrs[NDIRECT]);
    a = (uint *)bp->data;
    വേണ്ടി (j = 0; j < NINDIRECT; j++) {
      എങ്കിൽ (a[j])
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
ശൂന്യം
stati(ഘടന inode *ip, ഘടന stat *st)
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
പൂർണ്ണസംഖ്യ
readi(ഘടന inode *ip, പൂർണ്ണസംഖ്യ user_dst, uint64 dst, uint off, uint n)
{
  uint tot, m;
  ഘടന buf *bp;

  എങ്കിൽ (off > ip->size || off + n < off)
    തിരികെ 0;
  എങ്കിൽ (off + n > ip->size)
    n = ip->size - off;

  വേണ്ടി (tot = 0; tot < n; tot += m, off += m, dst += m) {
    uint addr = bmap(ip, off / BSIZE);
    എങ്കിൽ (addr == 0)
      നിർത്തുക;
    bp = bread(ip->dev, addr);
    m = min(n - tot, BSIZE - off % BSIZE);
    എങ്കിൽ (either_copyout(user_dst, dst, bp->data + (off % BSIZE), m) == -1) {
      brelse(bp);
      tot = -1;
      നിർത്തുക;
    }
    brelse(bp);
  }
  തിരികെ tot;
}

// Write data to inode.
// Caller must hold ip->lock.
// If user_src==1, then src is a user virtual address;
// otherwise, src is a kernel address.
// Returns the number of bytes successfully written.
// If the return value is less than the requested n,
// there was an error of some kind.
പൂർണ്ണസംഖ്യ
writei(ഘടന inode *ip, പൂർണ്ണസംഖ്യ user_src, uint64 src, uint off, uint n)
{
  uint tot, m;
  ഘടന buf *bp;

  എങ്കിൽ (off > ip->size || off + n < off)
    തിരികെ -1;
  എങ്കിൽ (off + n > MAXFILE * BSIZE)
    തിരികെ -1;

  വേണ്ടി (tot = 0; tot < n; tot += m, off += m, src += m) {
    uint addr = bmap(ip, off / BSIZE);
    എങ്കിൽ (addr == 0)
      നിർത്തുക;
    bp = bread(ip->dev, addr);
    m = min(n - tot, BSIZE - off % BSIZE);
    എങ്കിൽ (either_copyin(bp->data + (off % BSIZE), user_src, src, m) == -1) {
      // Might have partially updated the block, so we need to log it.
      log_write(bp);
      brelse(bp);
      നിർത്തുക;
    }
    log_write(bp);
    brelse(bp);
  }

  എങ്കിൽ (off > ip->size)
    ip->size = off;

  // write the i-node back to disk even if the size didn't change
  // because the loop above might have called bmap() and added a new
  // block to ip->addrs[].
  iupdate(ip);

  തിരികെ tot;
}

// Directories

പൂർണ്ണസംഖ്യ
namecmp(സ്ഥിരം അക്ഷരം *s, സ്ഥിരം അക്ഷരം *t)
{
  തിരികെ strncmp(s, t, DIRSIZ);
}

// Look for a directory entry in a directory.
// If found, set *poff to byte offset of entry.
ഘടന inode *
dirlookup(ഘടന inode *dp, അക്ഷരം *name, uint *poff)
{
  uint off, inum;
  ഘടന dirent de;

  എങ്കിൽ (dp->type != T_DIR)
    panic("dirlookup not DIR");

  വേണ്ടി (off = 0; off < dp->size; off += വലുപ്പം(de)) {
    എങ്കിൽ (readi(dp, 0, (uint64)&de, off, വലുപ്പം(de)) != വലുപ്പം(de))
      panic("dirlookup read");
    എങ്കിൽ (de.inum == 0)
      തുടരുക;
    എങ്കിൽ (namecmp(name, de.name) == 0) {
      // entry matches path element
      എങ്കിൽ (poff)
        *poff = off;
      inum = de.inum;
      തിരികെ iget(dp->dev, inum);
    }
  }

  തിരികെ 0;
}

// Write a new directory entry (name, inum) into the directory dp.
// Returns 0 on success, -1 on failure (e.g. out of disk blocks).
പൂർണ്ണസംഖ്യ
dirlink(ഘടന inode *dp, അക്ഷരം *name, uint inum)
{
  പൂർണ്ണസംഖ്യ off;
  ഘടന dirent de;
  ഘടന inode *ip;

  // Check that name is not present.
  എങ്കിൽ ((ip = dirlookup(dp, name, 0)) != 0) {
    iput(ip);
    തിരികെ -1;
  }

  // Look for an empty dirent.
  വേണ്ടി (off = 0; off < dp->size; off += വലുപ്പം(de)) {
    എങ്കിൽ (readi(dp, 0, (uint64)&de, off, വലുപ്പം(de)) != വലുപ്പം(de))
      panic("dirlink read");
    എങ്കിൽ (de.inum == 0)
      നിർത്തുക;
  }

  strncpy(de.name, name, DIRSIZ);
  de.inum = inum;
  എങ്കിൽ (writei(dp, 0, (uint64)&de, off, വലുപ്പം(de)) != വലുപ്പം(de))
    തിരികെ -1;

  തിരികെ 0;
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
സ്ഥിതം അക്ഷരം *
skipelem(അക്ഷരം *path, അക്ഷരം *name)
{
  അക്ഷരം *s;
  പൂർണ്ണസംഖ്യ len;

  ആകുമ്പോൾ (*path == '/')
    path++;
  എങ്കിൽ (*path == 0)
    തിരികെ 0;
  s = path;
  ആകുമ്പോൾ (*path != '/' && *path != 0)
    path++;
  len = path - s;
  എങ്കിൽ (len >= DIRSIZ)
    memmove(name, s, DIRSIZ);
  അല്ലെങ്കിൽ {
    memmove(name, s, len);
    name[len] = 0;
  }
  ആകുമ്പോൾ (*path == '/')
    path++;
  തിരികെ path;
}

// Look up and return the inode for a path name.
// If parent != 0, return the inode for the parent and copy the final
// path element into name, which must have room for DIRSIZ bytes.
// Must be called inside a transaction since it calls iput().
സ്ഥിതം ഘടന inode *
namex(അക്ഷരം *path, പൂർണ്ണസംഖ്യ nameiparent, അക്ഷരം *name)
{
  ഘടന inode *ip, *next;

  എങ്കിൽ (*path == '/')
    ip = iget(ROOTDEV, ROOTINO);
  അല്ലെങ്കിൽ
    ip = idup(myproc()->cwd);

  ആകുമ്പോൾ ((path = skipelem(path, name)) != 0) {
    ilock(ip);
    എങ്കിൽ (ip->type != T_DIR) {
      iunlockput(ip);
      തിരികെ 0;
    }
    എങ്കിൽ (ip->nlink == 0) {
      iunlockput(ip);
      തിരികെ 0;
    }
    എങ്കിൽ (nameiparent && *path == '\0') {
      // Stop one level early.
      iunlock(ip);
      തിരികെ ip;
    }
    എങ്കിൽ ((next = dirlookup(ip, name, 0)) == 0) {
      iunlockput(ip);
      തിരികെ 0;
    }
    iunlockput(ip);
    ip = next;
  }
  എങ്കിൽ (nameiparent) {
    iput(ip);
    തിരികെ 0;
  }
  തിരികെ ip;
}

ഘടന inode *
namei(അക്ഷരം *path)
{
  അക്ഷരം name[DIRSIZ];
  തിരികെ namex(path, 0, name);
}

ഘടന inode *
nameiparent(അക്ഷരം *path, അക്ഷരം *name)
{
  തിരികെ namex(path, 1, name);
}
