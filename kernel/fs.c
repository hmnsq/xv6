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

#అన్తర్భావ "types.h"
#అన్తర్భావ "riscv.h"
#అన్తర్భావ "defs.h"
#అన్తర్భావ "param.h"
#అన్తర్భావ "stat.h"
#అన్తర్భావ "spinlock.h"
#అన్తర్భావ "proc.h"
#అన్తర్భావ "sleeplock.h"
#అన్తర్భావ "fs.h"
#అన్తర్భావ "buf.h"
#అన్తర్భావ "file.h"

#నిర్వచించు min(a, b) ((a) < (b) ? (a) : (b))
// there should be one superblock per disk device, but we run with
// only one device
నిర్మాణం superblock sb;

// Read the super block.
స్థిర శూన్య
readsb(పూర్ణాంకం dev, నిర్మాణం superblock *sb)
{
  నిర్మాణం buf *bp;

  bp = bread(dev, 1);
  memmove(sb, bp->దత్తాంశం, పరిమాణం(*sb));
  brelse(bp);
}

// Init fs
శూన్య
fsinit(పూర్ణాంకం dev)
{
  readsb(dev, &sb);
  ఒకవేళ (sb.magic != FSMAGIC)
    panic("invalid file system");
  initlog(dev, &sb);
  ireclaim(dev);
}

// Zero a block.
స్థిర శూన్య
bzero(పూర్ణాంకం dev, పూర్ణాంకం bno)
{
  నిర్మాణం buf *bp;

  bp = bread(dev, bno);
  memset(bp->దత్తాంశం, 0, BSIZE);
  log_write(bp);
  brelse(bp);
}

// Blocks.

// Allocate a zeroed disk block.
// returns 0 if out of disk space.
స్థిర uint
balloc(uint dev)
{
  పూర్ణాంకం b, bi, m;
  నిర్మాణం buf *bp;

  bp = 0;
  కొరకు (b = 0; b < sb.కొలత; b += BPB) {
    bp = bread(dev, BBLOCK(b, sb));
    కొరకు (bi = 0; bi < BPB && b + bi < sb.కొలత; bi++) {
      m = 1 << (bi % 8);
      ఒకవేళ ((bp->దత్తాంశం[bi / 8] & m) == 0) { // Is block free?
        bp->దత్తాంశం[bi / 8] |= m;           // Mark block in use.
        log_write(bp);
        brelse(bp);
        bzero(dev, b + bi);
        తిరిగి b + bi;
      }
    }
    brelse(bp);
  }
  printk("balloc: out of blocks\n");
  తిరిగి 0;
}

// Free a disk block.
స్థిర శూన్య
bfree(పూర్ణాంకం dev, uint b)
{
  నిర్మాణం buf *bp;
  పూర్ణాంకం bi, m;

  bp = bread(dev, BBLOCK(b, sb));
  bi = b % BPB;
  m = 1 << (bi % 8);
  ఒకవేళ ((bp->దత్తాంశం[bi / 8] & m) == 0)
    panic("freeing free block");
  bp->దత్తాంశం[bi / 8] &= ~m;
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

నిర్మాణం {
  నిర్మాణం spinlock తాళం;
  నిర్మాణం inode inode[NINODE];
} itable;

శూన్య
iinit()
{
  పూర్ణాంకం i = 0;

  initlock(&itable.తాళం, "itable");
  కొరకు (i = 0; i < NINODE; i++) {
    initsleeplock(&itable.inode[i].తాళం, "inode");
  }
}

స్థిర నిర్మాణం inode *iget(uint dev, uint inum);

// Allocate an inode on device dev.
// Mark it as allocated by  giving it type type.
// Returns an unlocked but allocated and referenced inode,
// or NULL if there is no free inode.
నిర్మాణం inode *
ialloc(uint dev, పొట్టి type)
{
  పూర్ణాంకం inum;
  నిర్మాణం buf *bp;
  నిర్మాణం dinode *dip;

  కొరకు (inum = 1; inum < sb.ninodes; inum++) {
    bp = bread(dev, IBLOCK(inum, sb));
    dip = (నిర్మాణం dinode *)bp->దత్తాంశం + inum % IPB;
    ఒకవేళ (dip->type == 0) { // a free inode
      memset(dip, 0, పరిమాణం(*dip));
      dip->type = type;
      log_write(bp); // mark it allocated on the disk
      brelse(bp);
      తిరిగి iget(dev, inum);
    }
    brelse(bp);
  }
  printk("ialloc: no inodes\n");
  తిరిగి 0;
}

// Copy a modified in-memory inode to disk.
// Must be called after every change to an ip->xxx field
// that lives on disk.
// Caller must hold ip->lock.
శూన్య
iupdate(నిర్మాణం inode *ip)
{
  నిర్మాణం buf *bp;
  నిర్మాణం dinode *dip;

  bp = bread(ip->dev, IBLOCK(ip->inum, sb));
  dip = (నిర్మాణం dinode *)bp->దత్తాంశం + ip->inum % IPB;
  dip->type = ip->type;
  dip->major = ip->major;
  dip->minor = ip->minor;
  dip->nlink = ip->nlink;
  dip->కొలత = ip->కొలత;
  memmove(dip->addrs, ip->addrs, పరిమాణం(ip->addrs));
  log_write(bp);
  brelse(bp);
}

// Find the inode with number inum on device dev
// and return the in-memory copy. Does not lock
// the inode and does not read it from disk.
స్థిర నిర్మాణం inode *
iget(uint dev, uint inum)
{
  నిర్మాణం inode *ip, *ఖాళీలు;

  acquire(&itable.తాళం);

  // Is the inode already in the table?
  ఖాళీలు = 0;
  కొరకు (ip = &itable.inode[0]; ip < &itable.inode[NINODE]; ip++) {
    ఒకవేళ (ip->ref > 0 && ip->dev == dev && ip->inum == inum) {
      ip->ref++;
      release(&itable.తాళం);
      తిరిగి ip;
    }
    ఒకవేళ (ఖాళీలు == 0 && ip->ref == 0) // Remember empty slot.
      ఖాళీలు = ip;
  }

  // Recycle an inode entry.
  ఒకవేళ (ఖాళీలు == 0)
    panic("iget: no inodes");

  ip = ఖాళీలు;
  ip->dev = dev;
  ip->inum = inum;
  ip->ref = 1;
  ip->valid = 0;
  release(&itable.తాళం);

  తిరిగి ip;
}

// Increment reference count for ip.
// Returns ip to enable ip = idup(ip1) idiom.
నిర్మాణం inode *
idup(నిర్మాణం inode *ip)
{
  acquire(&itable.తాళం);
  ip->ref++;
  release(&itable.తాళం);
  తిరిగి ip;
}

// Lock the given inode.
// Reads the inode from disk if necessary.
శూన్య
ilock(నిర్మాణం inode *ip)
{
  నిర్మాణం buf *bp;
  నిర్మాణం dinode *dip;

  ఒకవేళ (ip == 0 || ip->ref < 1)
    panic("ilock");

  acquiresleep(&ip->తాళం);

  ఒకవేళ (ip->valid == 0) {
    bp = bread(ip->dev, IBLOCK(ip->inum, sb));
    dip = (నిర్మాణం dinode *)bp->దత్తాంశం + ip->inum % IPB;
    ip->type = dip->type;
    ip->major = dip->major;
    ip->minor = dip->minor;
    ip->nlink = dip->nlink;
    ip->కొలత = dip->కొలత;
    memmove(ip->addrs, dip->addrs, పరిమాణం(ip->addrs));
    brelse(bp);
    ip->valid = 1;
    ఒకవేళ (ip->type == 0)
      panic("ilock: no type");
  }
}

// Unlock the given inode.
శూన్య
iunlock(నిర్మాణం inode *ip)
{
  ఒకవేళ (ip == 0 || !holdingsleep(&ip->తాళం) || ip->ref < 1)
    panic("iunlock");

  releasesleep(&ip->తాళం);
}

// Mark the on-disk inode free.
స్థిర శూన్య
ifree(uint dev, uint inum)
{
  నిర్మాణం buf *bp = bread(dev, IBLOCK(inum, sb));
  నిర్మాణం dinode *dip = (నిర్మాణం dinode *)bp->దత్తాంశం + inum % IPB;
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
శూన్య
iput(నిర్మాణం inode *ip)
{
  acquire(&itable.తాళం);

  // Last reference of an unlinked inode?  Capture dev/inum before ref--,
  // since once ref hits 0, ip may be recycled by a concurrent iget()
  // for a different inum.
  పూర్ణాంకం చివరి = (ip->ref == 1 && ip->valid && ip->nlink == 0);
  uint dev = ip->dev, inum = ip->inum;

  ఒకవేళ (చివరి) {
    // ip->ref == 1 means no other process can have ip locked.
    acquiresleep(&ip->తాళం);
    release(&itable.తాళం);

    itrunc(ip); // free the data blocks (type stays nonzero on disk)
    ip->valid = 0;

    releasesleep(&ip->తాళం);

    acquire(&itable.తాళం);
  }

  ip->ref--;
  release(&itable.తాళం);

  ఒకవేళ (చివరి)
    ifree(dev, inum); // now clear type on disk: inum becomes allocatable
}

// Common idiom: unlock, then put.
శూన్య
iunlockput(నిర్మాణం inode *ip)
{
  iunlock(ip);
  iput(ip);
}

శూన్య
ireclaim(పూర్ణాంకం dev)
{
  కొరకు (పూర్ణాంకం inum = 1; inum < sb.ninodes; inum++) {
    నిర్మాణం inode *ip = 0;
    నిర్మాణం buf *bp = bread(dev, IBLOCK(inum, sb));
    నిర్మాణం dinode *dip = (నిర్మాణం dinode *)bp->దత్తాంశం + inum % IPB;
    ఒకవేళ (dip->type != 0 && dip->nlink == 0) { // is an orphaned inode
      printk("ireclaim: orphaned inode %d\n", inum);
      ip = iget(dev, inum);
    }
    brelse(bp);
    ఒకవేళ (ip) {
      begin_op();
      ilock(ip);
      iunlock(ip);
      iput(ip);
      చివర_op();
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
స్థిర uint
bmap(నిర్మాణం inode *ip, uint bn)
{
  uint addr, *a;
  నిర్మాణం buf *bp;

  ఒకవేళ (bn < NDIRECT) {
    ఒకవేళ ((addr = ip->addrs[bn]) == 0) {
      addr = balloc(ip->dev);
      ఒకవేళ (addr == 0)
        తిరిగి 0;
      ip->addrs[bn] = addr;
    }
    తిరిగి addr;
  }
  bn -= NDIRECT;

  ఒకవేళ (bn < NINDIRECT) {
    // Load indirect block, allocating if necessary.
    ఒకవేళ ((addr = ip->addrs[NDIRECT]) == 0) {
      addr = balloc(ip->dev);
      ఒకవేళ (addr == 0)
        తిరిగి 0;
      ip->addrs[NDIRECT] = addr;
    }
    bp = bread(ip->dev, addr);
    a = (uint *)bp->దత్తాంశం;
    ఒకవేళ ((addr = a[bn]) == 0) {
      addr = balloc(ip->dev);
      ఒకవేళ (addr) {
        a[bn] = addr;
        log_write(bp);
      }
    }
    brelse(bp);
    తిరిగి addr;
  }

  panic("bmap: out of range");
}

// Truncate inode (discard contents).
// Caller must hold ip->lock.
శూన్య
itrunc(నిర్మాణం inode *ip)
{
  పూర్ణాంకం i, j;
  నిర్మాణం buf *bp;
  uint *a;

  కొరకు (i = 0; i < NDIRECT; i++) {
    ఒకవేళ (ip->addrs[i]) {
      bfree(ip->dev, ip->addrs[i]);
      ip->addrs[i] = 0;
    }
  }

  ఒకవేళ (ip->addrs[NDIRECT]) {
    bp = bread(ip->dev, ip->addrs[NDIRECT]);
    a = (uint *)bp->దత్తాంశం;
    కొరకు (j = 0; j < NINDIRECT; j++) {
      ఒకవేళ (a[j])
        bfree(ip->dev, a[j]);
    }
    brelse(bp);
    bfree(ip->dev, ip->addrs[NDIRECT]);
    ip->addrs[NDIRECT] = 0;
  }

  ip->కొలత = 0;
  iupdate(ip);
}

// Copy stat information from inode.
// Caller must hold ip->lock.
శూన్య
stati(నిర్మాణం inode *ip, నిర్మాణం stat *st)
{
  st->dev = ip->dev;
  st->ino = ip->inum;
  st->type = ip->type;
  st->nlink = ip->nlink;
  st->కొలత = ip->కొలత;
}

// Read data from inode.
// Caller must hold ip->lock.
// If user_dst==1, then dst is a user virtual address;
// otherwise, dst is a kernel address.
పూర్ణాంకం
readi(నిర్మాణం inode *ip, పూర్ణాంకం user_dst, uint64 dst, uint off, uint n)
{
  uint tot, m;
  నిర్మాణం buf *bp;

  ఒకవేళ (off > ip->కొలత || off + n < off)
    తిరిగి 0;
  ఒకవేళ (off + n > ip->కొలత)
    n = ip->కొలత - off;

  కొరకు (tot = 0; tot < n; tot += m, off += m, dst += m) {
    uint addr = bmap(ip, off / BSIZE);
    ఒకవేళ (addr == 0)
      ఆపు;
    bp = bread(ip->dev, addr);
    m = min(n - tot, BSIZE - off % BSIZE);
    ఒకవేళ (either_copyout(user_dst, dst, bp->దత్తాంశం + (off % BSIZE), m) == -1) {
      brelse(bp);
      tot = -1;
      ఆపు;
    }
    brelse(bp);
  }
  తిరిగి tot;
}

// Write data to inode.
// Caller must hold ip->lock.
// If user_src==1, then src is a user virtual address;
// otherwise, src is a kernel address.
// Returns the number of bytes successfully written.
// If the return value is less than the requested n,
// there was an error of some kind.
పూర్ణాంకం
writei(నిర్మాణం inode *ip, పూర్ణాంకం user_src, uint64 src, uint off, uint n)
{
  uint tot, m;
  నిర్మాణం buf *bp;

  ఒకవేళ (off > ip->కొలత || off + n < off)
    తిరిగి -1;
  ఒకవేళ (off + n > MAXFILE * BSIZE)
    తిరిగి -1;

  కొరకు (tot = 0; tot < n; tot += m, off += m, src += m) {
    uint addr = bmap(ip, off / BSIZE);
    ఒకవేళ (addr == 0)
      ఆపు;
    bp = bread(ip->dev, addr);
    m = min(n - tot, BSIZE - off % BSIZE);
    ఒకవేళ (either_copyin(bp->దత్తాంశం + (off % BSIZE), user_src, src, m) == -1) {
      // Might have partially updated the block, so we need to log it.
      log_write(bp);
      brelse(bp);
      ఆపు;
    }
    log_write(bp);
    brelse(bp);
  }

  ఒకవేళ (off > ip->కొలత)
    ip->కొలత = off;

  // write the i-node back to disk even if the size didn't change
  // because the loop above might have called bmap() and added a new
  // block to ip->addrs[].
  iupdate(ip);

  తిరిగి tot;
}

// Directories

పూర్ణాంకం
namecmp(స్థిరరాశి అక్షరం *s, స్థిరరాశి అక్షరం *t)
{
  తిరిగి అక్షరశ్రేణి_పోలిక(s, t, DIRSIZ);
}

// Look for a directory entry in a directory.
// If found, set *poff to byte offset of entry.
నిర్మాణం inode *
dirlookup(నిర్మాణం inode *dp, అక్షరం *name, uint *poff)
{
  uint off, inum;
  నిర్మాణం dirent de;

  ఒకవేళ (dp->type != T_DIR)
    panic("dirlookup not DIR");

  కొరకు (off = 0; off < dp->కొలత; off += పరిమాణం(de)) {
    ఒకవేళ (readi(dp, 0, (uint64)&de, off, పరిమాణం(de)) != పరిమాణం(de))
      panic("dirlookup read");
    ఒకవేళ (de.inum == 0)
      కొనసాగు;
    ఒకవేళ (namecmp(name, de.name) == 0) {
      // entry matches path element
      ఒకవేళ (poff)
        *poff = off;
      inum = de.inum;
      తిరిగి iget(dp->dev, inum);
    }
  }

  తిరిగి 0;
}

// Write a new directory entry (name, inum) into the directory dp.
// Returns 0 on success, -1 on failure (e.g. out of disk blocks).
పూర్ణాంకం
dirlink(నిర్మాణం inode *dp, అక్షరం *name, uint inum)
{
  పూర్ణాంకం off;
  నిర్మాణం dirent de;
  నిర్మాణం inode *ip;

  // Check that name is not present.
  ఒకవేళ ((ip = dirlookup(dp, name, 0)) != 0) {
    iput(ip);
    తిరిగి -1;
  }

  // Look for an empty dirent.
  కొరకు (off = 0; off < dp->కొలత; off += పరిమాణం(de)) {
    ఒకవేళ (readi(dp, 0, (uint64)&de, off, పరిమాణం(de)) != పరిమాణం(de))
      panic("dirlink read");
    ఒకవేళ (de.inum == 0)
      ఆపు;
  }

  strncpy(de.name, name, DIRSIZ);
  de.inum = inum;
  ఒకవేళ (writei(dp, 0, (uint64)&de, off, పరిమాణం(de)) != పరిమాణం(de))
    తిరిగి -1;

  తిరిగి 0;
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
స్థిర అక్షరం *
skipelem(అక్షరం *మార్గం, అక్షరం *name)
{
  అక్షరం *s;
  పూర్ణాంకం len;

  అయితే (*మార్గం == '/')
    మార్గం++;
  ఒకవేళ (*మార్గం == 0)
    తిరిగి 0;
  s = మార్గం;
  అయితే (*మార్గం != '/' && *మార్గం != 0)
    మార్గం++;
  len = మార్గం - s;
  ఒకవేళ (len >= DIRSIZ)
    memmove(name, s, DIRSIZ);
  లేకపోతే {
    memmove(name, s, len);
    name[len] = 0;
  }
  అయితే (*మార్గం == '/')
    మార్గం++;
  తిరిగి మార్గం;
}

// Look up and return the inode for a path name.
// If parent != 0, return the inode for the parent and copy the final
// path element into name, which must have room for DIRSIZ bytes.
// Must be called inside a transaction since it calls iput().
స్థిర నిర్మాణం inode *
namex(అక్షరం *మార్గం, పూర్ణాంకం nameiparent, అక్షరం *name)
{
  నిర్మాణం inode *ip, *తదుపరి;

  ఒకవేళ (*మార్గం == '/')
    ip = iget(ROOTDEV, ROOTINO);
  లేకపోతే
    ip = idup(myproc()->cwd);

  అయితే ((మార్గం = skipelem(మార్గం, name)) != 0) {
    ilock(ip);
    ఒకవేళ (ip->type != T_DIR) {
      iunlockput(ip);
      తిరిగి 0;
    }
    ఒకవేళ (ip->nlink == 0) {
      iunlockput(ip);
      తిరిగి 0;
    }
    ఒకవేళ (nameiparent && *మార్గం == '\0') {
      // Stop one level early.
      iunlock(ip);
      తిరిగి ip;
    }
    ఒకవేళ ((తదుపరి = dirlookup(ip, name, 0)) == 0) {
      iunlockput(ip);
      తిరిగి 0;
    }
    iunlockput(ip);
    ip = తదుపరి;
  }
  ఒకవేళ (nameiparent) {
    iput(ip);
    తిరిగి 0;
  }
  తిరిగి ip;
}

నిర్మాణం inode *
namei(అక్షరం *మార్గం)
{
  అక్షరం name[DIRSIZ];
  తిరిగి namex(మార్గం, 0, name);
}

నిర్మాణం inode *
nameiparent(అక్షరం *మార్గం, అక్షరం *name)
{
  తిరిగి namex(మార్గం, 1, name);
}
