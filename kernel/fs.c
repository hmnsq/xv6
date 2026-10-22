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

#அந்தர்பாவ "types.h"
#அந்தர்பாவ "riscv.h"
#அந்தர்பாவ "defs.h"
#அந்தர்பாவ "param.h"
#அந்தர்பாவ "stat.h"
#அந்தர்பாவ "spinlock.h"
#அந்தர்பாவ "proc.h"
#அந்தர்பாவ "sleeplock.h"
#அந்தர்பாவ "fs.h"
#அந்தர்பாவ "buf.h"
#அந்தர்பாவ "file.h"

#define min(a, b) ((a) < (b) ? (a) : (b))
// there should be one superblock per disk device, but we run with
// only one device
கட்டமைப்பு superblock sb;

// Read the super block.
நிலையான வெறுமை
readsb(முழு dev, கட்டமைப்பு superblock *sb)
{
  கட்டமைப்பு buf *bp;

  bp = bread(dev, 1);
  memmove(sb, bp->data, அளவு(*sb));
  brelse(bp);
}

// Init fs
வெறுமை
fsinit(முழு dev)
{
  readsb(dev, &sb);
  எனில் (sb.magic != FSMAGIC)
    panic("invalid file system");
  initlog(dev, &sb);
  ireclaim(dev);
}

// Zero a block.
நிலையான வெறுமை
bzero(முழு dev, முழு bno)
{
  கட்டமைப்பு buf *bp;

  bp = bread(dev, bno);
  memset(bp->data, 0, BSIZE);
  log_write(bp);
  brelse(bp);
}

// Blocks.

// Allocate a zeroed disk block.
// returns 0 if out of disk space.
நிலையான uint
balloc(uint dev)
{
  முழு b, bi, m;
  கட்டமைப்பு buf *bp;

  bp = 0;
  க்கு (b = 0; b < sb.size; b += BPB) {
    bp = bread(dev, BBLOCK(b, sb));
    க்கு (bi = 0; bi < BPB && b + bi < sb.size; bi++) {
      m = 1 << (bi % 8);
      எனில் ((bp->data[bi / 8] & m) == 0) { // Is block free?
        bp->data[bi / 8] |= m;           // Mark block in use.
        log_write(bp);
        brelse(bp);
        bzero(dev, b + bi);
        திரும்பு b + bi;
      }
    }
    brelse(bp);
  }
  printk("balloc: out of blocks\n");
  திரும்பு 0;
}

// Free a disk block.
நிலையான வெறுமை
bfree(முழு dev, uint b)
{
  கட்டமைப்பு buf *bp;
  முழு bi, m;

  bp = bread(dev, BBLOCK(b, sb));
  bi = b % BPB;
  m = 1 << (bi % 8);
  எனில் ((bp->data[bi / 8] & m) == 0)
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

கட்டமைப்பு {
  கட்டமைப்பு spinlock lock;
  கட்டமைப்பு inode inode[NINODE];
} itable;

வெறுமை
iinit()
{
  முழு i = 0;

  initlock(&itable.lock, "itable");
  க்கு (i = 0; i < NINODE; i++) {
    initsleeplock(&itable.inode[i].lock, "inode");
  }
}

நிலையான கட்டமைப்பு inode *iget(uint dev, uint inum);

// Allocate an inode on device dev.
// Mark it as allocated by  giving it type type.
// Returns an unlocked but allocated and referenced inode,
// or NULL if there is no free inode.
கட்டமைப்பு inode *
ialloc(uint dev, short type)
{
  முழு inum;
  கட்டமைப்பு buf *bp;
  கட்டமைப்பு dinode *dip;

  க்கு (inum = 1; inum < sb.ninodes; inum++) {
    bp = bread(dev, IBLOCK(inum, sb));
    dip = (கட்டமைப்பு dinode *)bp->data + inum % IPB;
    எனில் (dip->type == 0) { // a free inode
      memset(dip, 0, அளவு(*dip));
      dip->type = type;
      log_write(bp); // mark it allocated on the disk
      brelse(bp);
      திரும்பு iget(dev, inum);
    }
    brelse(bp);
  }
  printk("ialloc: no inodes\n");
  திரும்பு 0;
}

// Copy a modified in-memory inode to disk.
// Must be called after every change to an ip->xxx field
// that lives on disk.
// Caller must hold ip->lock.
வெறுமை
iupdate(கட்டமைப்பு inode *ip)
{
  கட்டமைப்பு buf *bp;
  கட்டமைப்பு dinode *dip;

  bp = bread(ip->dev, IBLOCK(ip->inum, sb));
  dip = (கட்டமைப்பு dinode *)bp->data + ip->inum % IPB;
  dip->type = ip->type;
  dip->major = ip->major;
  dip->minor = ip->minor;
  dip->nlink = ip->nlink;
  dip->size = ip->size;
  memmove(dip->addrs, ip->addrs, அளவு(ip->addrs));
  log_write(bp);
  brelse(bp);
}

// Find the inode with number inum on device dev
// and return the in-memory copy. Does not lock
// the inode and does not read it from disk.
நிலையான கட்டமைப்பு inode *
iget(uint dev, uint inum)
{
  கட்டமைப்பு inode *ip, *empty;

  acquire(&itable.lock);

  // Is the inode already in the table?
  empty = 0;
  க்கு (ip = &itable.inode[0]; ip < &itable.inode[NINODE]; ip++) {
    எனில் (ip->ref > 0 && ip->dev == dev && ip->inum == inum) {
      ip->ref++;
      release(&itable.lock);
      திரும்பு ip;
    }
    எனில் (empty == 0 && ip->ref == 0) // Remember empty slot.
      empty = ip;
  }

  // Recycle an inode entry.
  எனில் (empty == 0)
    panic("iget: no inodes");

  ip = empty;
  ip->dev = dev;
  ip->inum = inum;
  ip->ref = 1;
  ip->valid = 0;
  release(&itable.lock);

  திரும்பு ip;
}

// Increment reference count for ip.
// Returns ip to enable ip = idup(ip1) idiom.
கட்டமைப்பு inode *
idup(கட்டமைப்பு inode *ip)
{
  acquire(&itable.lock);
  ip->ref++;
  release(&itable.lock);
  திரும்பு ip;
}

// Lock the given inode.
// Reads the inode from disk if necessary.
வெறுமை
ilock(கட்டமைப்பு inode *ip)
{
  கட்டமைப்பு buf *bp;
  கட்டமைப்பு dinode *dip;

  எனில் (ip == 0 || ip->ref < 1)
    panic("ilock");

  acquiresleep(&ip->lock);

  எனில் (ip->valid == 0) {
    bp = bread(ip->dev, IBLOCK(ip->inum, sb));
    dip = (கட்டமைப்பு dinode *)bp->data + ip->inum % IPB;
    ip->type = dip->type;
    ip->major = dip->major;
    ip->minor = dip->minor;
    ip->nlink = dip->nlink;
    ip->size = dip->size;
    memmove(ip->addrs, dip->addrs, அளவு(ip->addrs));
    brelse(bp);
    ip->valid = 1;
    எனில் (ip->type == 0)
      panic("ilock: no type");
  }
}

// Unlock the given inode.
வெறுமை
iunlock(கட்டமைப்பு inode *ip)
{
  எனில் (ip == 0 || !holdingsleep(&ip->lock) || ip->ref < 1)
    panic("iunlock");

  releasesleep(&ip->lock);
}

// Mark the on-disk inode free.
நிலையான வெறுமை
ifree(uint dev, uint inum)
{
  கட்டமைப்பு buf *bp = bread(dev, IBLOCK(inum, sb));
  கட்டமைப்பு dinode *dip = (கட்டமைப்பு dinode *)bp->data + inum % IPB;
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
வெறுமை
iput(கட்டமைப்பு inode *ip)
{
  acquire(&itable.lock);

  // Last reference of an unlinked inode?  Capture dev/inum before ref--,
  // since once ref hits 0, ip may be recycled by a concurrent iget()
  // for a different inum.
  முழு last = (ip->ref == 1 && ip->valid && ip->nlink == 0);
  uint dev = ip->dev, inum = ip->inum;

  எனில் (last) {
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

  எனில் (last)
    ifree(dev, inum); // now clear type on disk: inum becomes allocatable
}

// Common idiom: unlock, then put.
வெறுமை
iunlockput(கட்டமைப்பு inode *ip)
{
  iunlock(ip);
  iput(ip);
}

வெறுமை
ireclaim(முழு dev)
{
  க்கு (முழு inum = 1; inum < sb.ninodes; inum++) {
    கட்டமைப்பு inode *ip = 0;
    கட்டமைப்பு buf *bp = bread(dev, IBLOCK(inum, sb));
    கட்டமைப்பு dinode *dip = (கட்டமைப்பு dinode *)bp->data + inum % IPB;
    எனில் (dip->type != 0 && dip->nlink == 0) { // is an orphaned inode
      printk("ireclaim: orphaned inode %d\n", inum);
      ip = iget(dev, inum);
    }
    brelse(bp);
    எனில் (ip) {
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
நிலையான uint
bmap(கட்டமைப்பு inode *ip, uint bn)
{
  uint addr, *a;
  கட்டமைப்பு buf *bp;

  எனில் (bn < NDIRECT) {
    எனில் ((addr = ip->addrs[bn]) == 0) {
      addr = balloc(ip->dev);
      எனில் (addr == 0)
        திரும்பு 0;
      ip->addrs[bn] = addr;
    }
    திரும்பு addr;
  }
  bn -= NDIRECT;

  எனில் (bn < NINDIRECT) {
    // Load indirect block, allocating if necessary.
    எனில் ((addr = ip->addrs[NDIRECT]) == 0) {
      addr = balloc(ip->dev);
      எனில் (addr == 0)
        திரும்பு 0;
      ip->addrs[NDIRECT] = addr;
    }
    bp = bread(ip->dev, addr);
    a = (uint *)bp->data;
    எனில் ((addr = a[bn]) == 0) {
      addr = balloc(ip->dev);
      எனில் (addr) {
        a[bn] = addr;
        log_write(bp);
      }
    }
    brelse(bp);
    திரும்பு addr;
  }

  panic("bmap: out of range");
}

// Truncate inode (discard contents).
// Caller must hold ip->lock.
வெறுமை
itrunc(கட்டமைப்பு inode *ip)
{
  முழு i, j;
  கட்டமைப்பு buf *bp;
  uint *a;

  க்கு (i = 0; i < NDIRECT; i++) {
    எனில் (ip->addrs[i]) {
      bfree(ip->dev, ip->addrs[i]);
      ip->addrs[i] = 0;
    }
  }

  எனில் (ip->addrs[NDIRECT]) {
    bp = bread(ip->dev, ip->addrs[NDIRECT]);
    a = (uint *)bp->data;
    க்கு (j = 0; j < NINDIRECT; j++) {
      எனில் (a[j])
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
வெறுமை
stati(கட்டமைப்பு inode *ip, கட்டமைப்பு stat *st)
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
முழு
readi(கட்டமைப்பு inode *ip, முழு user_dst, uint64 dst, uint off, uint n)
{
  uint tot, m;
  கட்டமைப்பு buf *bp;

  எனில் (off > ip->size || off + n < off)
    திரும்பு 0;
  எனில் (off + n > ip->size)
    n = ip->size - off;

  க்கு (tot = 0; tot < n; tot += m, off += m, dst += m) {
    uint addr = bmap(ip, off / BSIZE);
    எனில் (addr == 0)
      நிறுத்து;
    bp = bread(ip->dev, addr);
    m = min(n - tot, BSIZE - off % BSIZE);
    எனில் (either_copyout(user_dst, dst, bp->data + (off % BSIZE), m) == -1) {
      brelse(bp);
      tot = -1;
      நிறுத்து;
    }
    brelse(bp);
  }
  திரும்பு tot;
}

// Write data to inode.
// Caller must hold ip->lock.
// If user_src==1, then src is a user virtual address;
// otherwise, src is a kernel address.
// Returns the number of bytes successfully written.
// If the return value is less than the requested n,
// there was an error of some kind.
முழு
writei(கட்டமைப்பு inode *ip, முழு user_src, uint64 src, uint off, uint n)
{
  uint tot, m;
  கட்டமைப்பு buf *bp;

  எனில் (off > ip->size || off + n < off)
    திரும்பு -1;
  எனில் (off + n > MAXFILE * BSIZE)
    திரும்பு -1;

  க்கு (tot = 0; tot < n; tot += m, off += m, src += m) {
    uint addr = bmap(ip, off / BSIZE);
    எனில் (addr == 0)
      நிறுத்து;
    bp = bread(ip->dev, addr);
    m = min(n - tot, BSIZE - off % BSIZE);
    எனில் (either_copyin(bp->data + (off % BSIZE), user_src, src, m) == -1) {
      // Might have partially updated the block, so we need to log it.
      log_write(bp);
      brelse(bp);
      நிறுத்து;
    }
    log_write(bp);
    brelse(bp);
  }

  எனில் (off > ip->size)
    ip->size = off;

  // write the i-node back to disk even if the size didn't change
  // because the loop above might have called bmap() and added a new
  // block to ip->addrs[].
  iupdate(ip);

  திரும்பு tot;
}

// Directories

முழு
namecmp(மாறிலி எழுத்து *s, மாறிலி எழுத்து *t)
{
  திரும்பு strncmp(s, t, DIRSIZ);
}

// Look for a directory entry in a directory.
// If found, set *poff to byte offset of entry.
கட்டமைப்பு inode *
dirlookup(கட்டமைப்பு inode *dp, எழுத்து *name, uint *poff)
{
  uint off, inum;
  கட்டமைப்பு dirent de;

  எனில் (dp->type != T_DIR)
    panic("dirlookup not DIR");

  க்கு (off = 0; off < dp->size; off += அளவு(de)) {
    எனில் (readi(dp, 0, (uint64)&de, off, அளவு(de)) != அளவு(de))
      panic("dirlookup read");
    எனில் (de.inum == 0)
      தொடர்;
    எனில் (namecmp(name, de.name) == 0) {
      // entry matches path element
      எனில் (poff)
        *poff = off;
      inum = de.inum;
      திரும்பு iget(dp->dev, inum);
    }
  }

  திரும்பு 0;
}

// Write a new directory entry (name, inum) into the directory dp.
// Returns 0 on success, -1 on failure (e.g. out of disk blocks).
முழு
dirlink(கட்டமைப்பு inode *dp, எழுத்து *name, uint inum)
{
  முழு off;
  கட்டமைப்பு dirent de;
  கட்டமைப்பு inode *ip;

  // Check that name is not present.
  எனில் ((ip = dirlookup(dp, name, 0)) != 0) {
    iput(ip);
    திரும்பு -1;
  }

  // Look for an empty dirent.
  க்கு (off = 0; off < dp->size; off += அளவு(de)) {
    எனில் (readi(dp, 0, (uint64)&de, off, அளவு(de)) != அளவு(de))
      panic("dirlink read");
    எனில் (de.inum == 0)
      நிறுத்து;
  }

  strncpy(de.name, name, DIRSIZ);
  de.inum = inum;
  எனில் (writei(dp, 0, (uint64)&de, off, அளவு(de)) != அளவு(de))
    திரும்பு -1;

  திரும்பு 0;
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
நிலையான எழுத்து *
skipelem(எழுத்து *path, எழுத்து *name)
{
  எழுத்து *s;
  முழு len;

  போது (*path == '/')
    path++;
  எனில் (*path == 0)
    திரும்பு 0;
  s = path;
  போது (*path != '/' && *path != 0)
    path++;
  len = path - s;
  எனில் (len >= DIRSIZ)
    memmove(name, s, DIRSIZ);
  இல்லையெனில் {
    memmove(name, s, len);
    name[len] = 0;
  }
  போது (*path == '/')
    path++;
  திரும்பு path;
}

// Look up and return the inode for a path name.
// If parent != 0, return the inode for the parent and copy the final
// path element into name, which must have room for DIRSIZ bytes.
// Must be called inside a transaction since it calls iput().
நிலையான கட்டமைப்பு inode *
namex(எழுத்து *path, முழு nameiparent, எழுத்து *name)
{
  கட்டமைப்பு inode *ip, *next;

  எனில் (*path == '/')
    ip = iget(ROOTDEV, ROOTINO);
  இல்லையெனில்
    ip = idup(myproc()->cwd);

  போது ((path = skipelem(path, name)) != 0) {
    ilock(ip);
    எனில் (ip->type != T_DIR) {
      iunlockput(ip);
      திரும்பு 0;
    }
    எனில் (ip->nlink == 0) {
      iunlockput(ip);
      திரும்பு 0;
    }
    எனில் (nameiparent && *path == '\0') {
      // Stop one level early.
      iunlock(ip);
      திரும்பு ip;
    }
    எனில் ((next = dirlookup(ip, name, 0)) == 0) {
      iunlockput(ip);
      திரும்பு 0;
    }
    iunlockput(ip);
    ip = next;
  }
  எனில் (nameiparent) {
    iput(ip);
    திரும்பு 0;
  }
  திரும்பு ip;
}

கட்டமைப்பு inode *
namei(எழுத்து *path)
{
  எழுத்து name[DIRSIZ];
  திரும்பு namex(path, 0, name);
}

கட்டமைப்பு inode *
nameiparent(எழுத்து *path, எழுத்து *name)
{
  திரும்பு namex(path, 1, name);
}
