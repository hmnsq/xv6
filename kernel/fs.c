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

#अन्तर्भाव "types.h"
#अन्तर्भाव "riscv.h"
#अन्तर्भाव "defs.h"
#अन्तर्भाव "param.h"
#अन्तर्भाव "stat.h"
#अन्तर्भाव "spinlock.h"
#अन्तर्भाव "proc.h"
#अन्तर्भाव "sleeplock.h"
#अन्तर्भाव "fs.h"
#अन्तर्भाव "buf.h"
#अन्तर्भाव "file.h"

#define min(a, b) ((a) < (b) ? (a) : (b))
// there should be one superblock per disk device, but we run with
// only one device
संरचना superblock sb;

// Read the super block.
स्थिर शून्य
readsb(पूर्णांक dev, संरचना superblock *sb)
{
  संरचना buf *bp;

  bp = bread(dev, 1);
  स_हटाओ(sb, bp->data, आकार(*sb));
  brelse(bp);
}

// Init fs
शून्य
fsinit(पूर्णांक dev)
{
  readsb(dev, &sb);
  जर (sb.magic != FSMAGIC)
    panic("invalid file system");
  initlog(dev, &sb);
  ireclaim(dev);
}

// Zero a block.
स्थिर शून्य
bzero(पूर्णांक dev, पूर्णांक bno)
{
  संरचना buf *bp;

  bp = bread(dev, bno);
  स_रखो(bp->data, 0, BSIZE);
  log_write(bp);
  brelse(bp);
}

// Blocks.

// Allocate a zeroed disk block.
// returns 0 if out of disk space.
स्थिर uint
balloc(uint dev)
{
  पूर्णांक b, bi, m;
  संरचना buf *bp;

  bp = 0;
  साठी (b = 0; b < sb.size; b += BPB) {
    bp = bread(dev, BBLOCK(b, sb));
    साठी (bi = 0; bi < BPB && b + bi < sb.size; bi++) {
      m = 1 << (bi % 8);
      जर ((bp->data[bi / 8] & m) == 0) { // Is block free?
        bp->data[bi / 8] |= m;           // Mark block in use.
        log_write(bp);
        brelse(bp);
        bzero(dev, b + bi);
        परत b + bi;
      }
    }
    brelse(bp);
  }
  printk("balloc: out of blocks\n");
  परत 0;
}

// Free a disk block.
स्थिर शून्य
bfree(पूर्णांक dev, uint b)
{
  संरचना buf *bp;
  पूर्णांक bi, m;

  bp = bread(dev, BBLOCK(b, sb));
  bi = b % BPB;
  m = 1 << (bi % 8);
  जर ((bp->data[bi / 8] & m) == 0)
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

संरचना {
  संरचना spinlock lock;
  संरचना inode inode[NINODE];
} itable;

शून्य
iinit()
{
  पूर्णांक i = 0;

  initlock(&itable.lock, "itable");
  साठी (i = 0; i < NINODE; i++) {
    initsleeplock(&itable.inode[i].lock, "inode");
  }
}

स्थिर संरचना inode *iget(uint dev, uint inum);

// Allocate an inode on device dev.
// Mark it as allocated by  giving it type type.
// Returns an unlocked but allocated and referenced inode,
// or NULL if there is no free inode.
संरचना inode *
ialloc(uint dev, लघु type)
{
  पूर्णांक inum;
  संरचना buf *bp;
  संरचना dinode *dip;

  साठी (inum = 1; inum < sb.ninodes; inum++) {
    bp = bread(dev, IBLOCK(inum, sb));
    dip = (संरचना dinode *)bp->data + inum % IPB;
    जर (dip->type == 0) { // a free inode
      स_रखो(dip, 0, आकार(*dip));
      dip->type = type;
      log_write(bp); // mark it allocated on the disk
      brelse(bp);
      परत iget(dev, inum);
    }
    brelse(bp);
  }
  printk("ialloc: no inodes\n");
  परत 0;
}

// Copy a modified in-memory inode to disk.
// Must be called after every change to an ip->xxx field
// that lives on disk.
// Caller must hold ip->lock.
शून्य
iupdate(संरचना inode *ip)
{
  संरचना buf *bp;
  संरचना dinode *dip;

  bp = bread(ip->dev, IBLOCK(ip->inum, sb));
  dip = (संरचना dinode *)bp->data + ip->inum % IPB;
  dip->type = ip->type;
  dip->major = ip->major;
  dip->minor = ip->minor;
  dip->nlink = ip->nlink;
  dip->size = ip->size;
  स_हटाओ(dip->addrs, ip->addrs, आकार(ip->addrs));
  log_write(bp);
  brelse(bp);
}

// Find the inode with number inum on device dev
// and return the in-memory copy. Does not lock
// the inode and does not read it from disk.
स्थिर संरचना inode *
iget(uint dev, uint inum)
{
  संरचना inode *ip, *empty;

  acquire(&itable.lock);

  // Is the inode already in the table?
  empty = 0;
  साठी (ip = &itable.inode[0]; ip < &itable.inode[NINODE]; ip++) {
    जर (ip->ref > 0 && ip->dev == dev && ip->inum == inum) {
      ip->ref++;
      release(&itable.lock);
      परत ip;
    }
    जर (empty == 0 && ip->ref == 0) // Remember empty slot.
      empty = ip;
  }

  // Recycle an inode entry.
  जर (empty == 0)
    panic("iget: no inodes");

  ip = empty;
  ip->dev = dev;
  ip->inum = inum;
  ip->ref = 1;
  ip->valid = 0;
  release(&itable.lock);

  परत ip;
}

// Increment reference count for ip.
// Returns ip to enable ip = idup(ip1) idiom.
संरचना inode *
idup(संरचना inode *ip)
{
  acquire(&itable.lock);
  ip->ref++;
  release(&itable.lock);
  परत ip;
}

// Lock the given inode.
// Reads the inode from disk if necessary.
शून्य
ilock(संरचना inode *ip)
{
  संरचना buf *bp;
  संरचना dinode *dip;

  जर (ip == 0 || ip->ref < 1)
    panic("ilock");

  acquiresleep(&ip->lock);

  जर (ip->valid == 0) {
    bp = bread(ip->dev, IBLOCK(ip->inum, sb));
    dip = (संरचना dinode *)bp->data + ip->inum % IPB;
    ip->type = dip->type;
    ip->major = dip->major;
    ip->minor = dip->minor;
    ip->nlink = dip->nlink;
    ip->size = dip->size;
    स_हटाओ(ip->addrs, dip->addrs, आकार(ip->addrs));
    brelse(bp);
    ip->valid = 1;
    जर (ip->type == 0)
      panic("ilock: no type");
  }
}

// Unlock the given inode.
शून्य
iunlock(संरचना inode *ip)
{
  जर (ip == 0 || !holdingsleep(&ip->lock) || ip->ref < 1)
    panic("iunlock");

  releasesleep(&ip->lock);
}

// Mark the on-disk inode free.
स्थिर शून्य
ifree(uint dev, uint inum)
{
  संरचना buf *bp = bread(dev, IBLOCK(inum, sb));
  संरचना dinode *dip = (संरचना dinode *)bp->data + inum % IPB;
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
शून्य
iput(संरचना inode *ip)
{
  acquire(&itable.lock);

  // Last reference of an unlinked inode?  Capture dev/inum before ref--,
  // since once ref hits 0, ip may be recycled by a concurrent iget()
  // for a different inum.
  पूर्णांक last = (ip->ref == 1 && ip->valid && ip->nlink == 0);
  uint dev = ip->dev, inum = ip->inum;

  जर (last) {
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

  जर (last)
    ifree(dev, inum); // now clear type on disk: inum becomes allocatable
}

// Common idiom: unlock, then put.
शून्य
iunlockput(संरचना inode *ip)
{
  iunlock(ip);
  iput(ip);
}

शून्य
ireclaim(पूर्णांक dev)
{
  साठी (पूर्णांक inum = 1; inum < sb.ninodes; inum++) {
    संरचना inode *ip = 0;
    संरचना buf *bp = bread(dev, IBLOCK(inum, sb));
    संरचना dinode *dip = (संरचना dinode *)bp->data + inum % IPB;
    जर (dip->type != 0 && dip->nlink == 0) { // is an orphaned inode
      printk("ireclaim: orphaned inode %d\n", inum);
      ip = iget(dev, inum);
    }
    brelse(bp);
    जर (ip) {
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
स्थिर uint
bmap(संरचना inode *ip, uint bn)
{
  uint addr, *a;
  संरचना buf *bp;

  जर (bn < NDIRECT) {
    जर ((addr = ip->addrs[bn]) == 0) {
      addr = balloc(ip->dev);
      जर (addr == 0)
        परत 0;
      ip->addrs[bn] = addr;
    }
    परत addr;
  }
  bn -= NDIRECT;

  जर (bn < NINDIRECT) {
    // Load indirect block, allocating if necessary.
    जर ((addr = ip->addrs[NDIRECT]) == 0) {
      addr = balloc(ip->dev);
      जर (addr == 0)
        परत 0;
      ip->addrs[NDIRECT] = addr;
    }
    bp = bread(ip->dev, addr);
    a = (uint *)bp->data;
    जर ((addr = a[bn]) == 0) {
      addr = balloc(ip->dev);
      जर (addr) {
        a[bn] = addr;
        log_write(bp);
      }
    }
    brelse(bp);
    परत addr;
  }

  panic("bmap: out of range");
}

// Truncate inode (discard contents).
// Caller must hold ip->lock.
शून्य
itrunc(संरचना inode *ip)
{
  पूर्णांक i, j;
  संरचना buf *bp;
  uint *a;

  साठी (i = 0; i < NDIRECT; i++) {
    जर (ip->addrs[i]) {
      bfree(ip->dev, ip->addrs[i]);
      ip->addrs[i] = 0;
    }
  }

  जर (ip->addrs[NDIRECT]) {
    bp = bread(ip->dev, ip->addrs[NDIRECT]);
    a = (uint *)bp->data;
    साठी (j = 0; j < NINDIRECT; j++) {
      जर (a[j])
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
शून्य
stati(संरचना inode *ip, संरचना stat *st)
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
पूर्णांक
readi(संरचना inode *ip, पूर्णांक user_dst, uint64 dst, uint off, uint n)
{
  uint tot, m;
  संरचना buf *bp;

  जर (off > ip->size || off + n < off)
    परत 0;
  जर (off + n > ip->size)
    n = ip->size - off;

  साठी (tot = 0; tot < n; tot += m, off += m, dst += m) {
    uint addr = bmap(ip, off / BSIZE);
    जर (addr == 0)
      थांबा;
    bp = bread(ip->dev, addr);
    m = min(n - tot, BSIZE - off % BSIZE);
    जर (either_copyout(user_dst, dst, bp->data + (off % BSIZE), m) == -1) {
      brelse(bp);
      tot = -1;
      थांबा;
    }
    brelse(bp);
  }
  परत tot;
}

// Write data to inode.
// Caller must hold ip->lock.
// If user_src==1, then src is a user virtual address;
// otherwise, src is a kernel address.
// Returns the number of bytes successfully written.
// If the return value is less than the requested n,
// there was an error of some kind.
पूर्णांक
writei(संरचना inode *ip, पूर्णांक user_src, uint64 src, uint off, uint n)
{
  uint tot, m;
  संरचना buf *bp;

  जर (off > ip->size || off + n < off)
    परत -1;
  जर (off + n > MAXFILE * BSIZE)
    परत -1;

  साठी (tot = 0; tot < n; tot += m, off += m, src += m) {
    uint addr = bmap(ip, off / BSIZE);
    जर (addr == 0)
      थांबा;
    bp = bread(ip->dev, addr);
    m = min(n - tot, BSIZE - off % BSIZE);
    जर (either_copyin(bp->data + (off % BSIZE), user_src, src, m) == -1) {
      // Might have partially updated the block, so we need to log it.
      log_write(bp);
      brelse(bp);
      थांबा;
    }
    log_write(bp);
    brelse(bp);
  }

  जर (off > ip->size)
    ip->size = off;

  // write the i-node back to disk even if the size didn't change
  // because the loop above might have called bmap() and added a new
  // block to ip->addrs[].
  iupdate(ip);

  परत tot;
}

// Directories

पूर्णांक
namecmp(स्थिरांक अक्षर *s, स्थिरांक अक्षर *t)
{
  परत म_भेदन(s, t, DIRSIZ);
}

// Look for a directory entry in a directory.
// If found, set *poff to byte offset of entry.
संरचना inode *
dirlookup(संरचना inode *dp, अक्षर *name, uint *poff)
{
  uint off, inum;
  संरचना dirent de;

  जर (dp->type != T_DIR)
    panic("dirlookup not DIR");

  साठी (off = 0; off < dp->size; off += आकार(de)) {
    जर (readi(dp, 0, (uint64)&de, off, आकार(de)) != आकार(de))
      panic("dirlookup read");
    जर (de.inum == 0)
      सुरूठेवा;
    जर (namecmp(name, de.name) == 0) {
      // entry matches path element
      जर (poff)
        *poff = off;
      inum = de.inum;
      परत iget(dp->dev, inum);
    }
  }

  परत 0;
}

// Write a new directory entry (name, inum) into the directory dp.
// Returns 0 on success, -1 on failure (e.g. out of disk blocks).
पूर्णांक
dirlink(संरचना inode *dp, अक्षर *name, uint inum)
{
  पूर्णांक off;
  संरचना dirent de;
  संरचना inode *ip;

  // Check that name is not present.
  जर ((ip = dirlookup(dp, name, 0)) != 0) {
    iput(ip);
    परत -1;
  }

  // Look for an empty dirent.
  साठी (off = 0; off < dp->size; off += आकार(de)) {
    जर (readi(dp, 0, (uint64)&de, off, आकार(de)) != आकार(de))
      panic("dirlink read");
    जर (de.inum == 0)
      थांबा;
  }

  म_नकलन(de.name, name, DIRSIZ);
  de.inum = inum;
  जर (writei(dp, 0, (uint64)&de, off, आकार(de)) != आकार(de))
    परत -1;

  परत 0;
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
स्थिर अक्षर *
skipelem(अक्षर *path, अक्षर *name)
{
  अक्षर *s;
  पूर्णांक len;

  पर्यंत (*path == '/')
    path++;
  जर (*path == 0)
    परत 0;
  s = path;
  पर्यंत (*path != '/' && *path != 0)
    path++;
  len = path - s;
  जर (len >= DIRSIZ)
    स_हटाओ(name, s, DIRSIZ);
  अन्यथा {
    स_हटाओ(name, s, len);
    name[len] = 0;
  }
  पर्यंत (*path == '/')
    path++;
  परत path;
}

// Look up and return the inode for a path name.
// If parent != 0, return the inode for the parent and copy the final
// path element into name, which must have room for DIRSIZ bytes.
// Must be called inside a transaction since it calls iput().
स्थिर संरचना inode *
namex(अक्षर *path, पूर्णांक nameiparent, अक्षर *name)
{
  संरचना inode *ip, *next;

  जर (*path == '/')
    ip = iget(ROOTDEV, ROOTINO);
  अन्यथा
    ip = idup(myproc()->cwd);

  पर्यंत ((path = skipelem(path, name)) != 0) {
    ilock(ip);
    जर (ip->type != T_DIR) {
      iunlockput(ip);
      परत 0;
    }
    जर (ip->nlink == 0) {
      iunlockput(ip);
      परत 0;
    }
    जर (nameiparent && *path == '\0') {
      // Stop one level early.
      iunlock(ip);
      परत ip;
    }
    जर ((next = dirlookup(ip, name, 0)) == 0) {
      iunlockput(ip);
      परत 0;
    }
    iunlockput(ip);
    ip = next;
  }
  जर (nameiparent) {
    iput(ip);
    परत 0;
  }
  परत ip;
}

संरचना inode *
namei(अक्षर *path)
{
  अक्षर name[DIRSIZ];
  परत namex(path, 0, name);
}

संरचना inode *
nameiparent(अक्षर *path, अक्षर *name)
{
  परत namex(path, 1, name);
}
