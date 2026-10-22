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

#تعریف_کرو min(a, b) ((a) < (b) ? (a) : (b))
// there should be one superblock per disk device, but we run with
// only one device
ساخت superblock sb;

// Read the super block.
ساکن خالی
readsb(صحیح dev, ساخت superblock *sb)
{
  ساخت buf *bp;

  bp = bread(dev, 1);
  memmove(sb, bp->ڈیٹا, حجم(*sb));
  brelse(bp);
}

// Init fs
خالی
fsinit(صحیح dev)
{
  readsb(dev, &sb);
  اگر (sb.magic != FSMAGIC)
    panic("invalid file system");
  initlog(dev, &sb);
  ireclaim(dev);
}

// Zero a block.
ساکن خالی
bzero(صحیح dev, صحیح bno)
{
  ساخت buf *bp;

  bp = bread(dev, bno);
  memset(bp->ڈیٹا, 0, BSIZE);
  log_write(bp);
  brelse(bp);
}

// Blocks.

// Allocate a zeroed disk block.
// returns 0 if out of disk space.
ساکن uint
balloc(uint dev)
{
  صحیح b, bi, m;
  ساخت buf *bp;

  bp = 0;
  برائے (b = 0; b < sb.سائز; b += BPB) {
    bp = bread(dev, BBLOCK(b, sb));
    برائے (bi = 0; bi < BPB && b + bi < sb.سائز; bi++) {
      m = 1 << (bi % 8);
      اگر ((bp->ڈیٹا[bi / 8] & m) == 0) { // Is block free?
        bp->ڈیٹا[bi / 8] |= m;           // Mark block in use.
        log_write(bp);
        brelse(bp);
        bzero(dev, b + bi);
        واپس b + bi;
      }
    }
    brelse(bp);
  }
  printk("balloc: out of blocks\n");
  واپس 0;
}

// Free a disk block.
ساکن خالی
bfree(صحیح dev, uint b)
{
  ساخت buf *bp;
  صحیح bi, m;

  bp = bread(dev, BBLOCK(b, sb));
  bi = b % BPB;
  m = 1 << (bi % 8);
  اگر ((bp->ڈیٹا[bi / 8] & m) == 0)
    panic("freeing free block");
  bp->ڈیٹا[bi / 8] &= ~m;
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

ساخت {
  ساخت spinlock تالا;
  ساخت inode inode[NINODE];
} itable;

خالی
iinit()
{
  صحیح i = 0;

  initlock(&itable.تالا, "itable");
  برائے (i = 0; i < NINODE; i++) {
    initsleeplock(&itable.inode[i].تالا, "inode");
  }
}

ساکن ساخت inode *iget(uint dev, uint inum);

// Allocate an inode on device dev.
// Mark it as allocated by  giving it type type.
// Returns an unlocked but allocated and referenced inode,
// or NULL if there is no free inode.
ساخت inode *
ialloc(uint dev, مختصر type)
{
  صحیح inum;
  ساخت buf *bp;
  ساخت dinode *dip;

  برائے (inum = 1; inum < sb.ninodes; inum++) {
    bp = bread(dev, IBLOCK(inum, sb));
    dip = (ساخت dinode *)bp->ڈیٹا + inum % IPB;
    اگر (dip->type == 0) { // a free inode
      memset(dip, 0, حجم(*dip));
      dip->type = type;
      log_write(bp); // mark it allocated on the disk
      brelse(bp);
      واپس iget(dev, inum);
    }
    brelse(bp);
  }
  printk("ialloc: no inodes\n");
  واپس 0;
}

// Copy a modified in-memory inode to disk.
// Must be called after every change to an ip->xxx field
// that lives on disk.
// Caller must hold ip->lock.
خالی
iupdate(ساخت inode *ip)
{
  ساخت buf *bp;
  ساخت dinode *dip;

  bp = bread(ip->dev, IBLOCK(ip->inum, sb));
  dip = (ساخت dinode *)bp->ڈیٹا + ip->inum % IPB;
  dip->type = ip->type;
  dip->major = ip->major;
  dip->minor = ip->minor;
  dip->nlink = ip->nlink;
  dip->سائز = ip->سائز;
  memmove(dip->addrs, ip->addrs, حجم(ip->addrs));
  log_write(bp);
  brelse(bp);
}

// Find the inode with number inum on device dev
// and return the in-memory copy. Does not lock
// the inode and does not read it from disk.
ساکن ساخت inode *
iget(uint dev, uint inum)
{
  ساخت inode *ip, *خالی_خانے;

  acquire(&itable.تالا);

  // Is the inode already in the table?
  خالی_خانے = 0;
  برائے (ip = &itable.inode[0]; ip < &itable.inode[NINODE]; ip++) {
    اگر (ip->ref > 0 && ip->dev == dev && ip->inum == inum) {
      ip->ref++;
      release(&itable.تالا);
      واپس ip;
    }
    اگر (خالی_خانے == 0 && ip->ref == 0) // Remember empty slot.
      خالی_خانے = ip;
  }

  // Recycle an inode entry.
  اگر (خالی_خانے == 0)
    panic("iget: no inodes");

  ip = خالی_خانے;
  ip->dev = dev;
  ip->inum = inum;
  ip->ref = 1;
  ip->valid = 0;
  release(&itable.تالا);

  واپس ip;
}

// Increment reference count for ip.
// Returns ip to enable ip = idup(ip1) idiom.
ساخت inode *
idup(ساخت inode *ip)
{
  acquire(&itable.تالا);
  ip->ref++;
  release(&itable.تالا);
  واپس ip;
}

// Lock the given inode.
// Reads the inode from disk if necessary.
خالی
ilock(ساخت inode *ip)
{
  ساخت buf *bp;
  ساخت dinode *dip;

  اگر (ip == 0 || ip->ref < 1)
    panic("ilock");

  acquiresleep(&ip->تالا);

  اگر (ip->valid == 0) {
    bp = bread(ip->dev, IBLOCK(ip->inum, sb));
    dip = (ساخت dinode *)bp->ڈیٹا + ip->inum % IPB;
    ip->type = dip->type;
    ip->major = dip->major;
    ip->minor = dip->minor;
    ip->nlink = dip->nlink;
    ip->سائز = dip->سائز;
    memmove(ip->addrs, dip->addrs, حجم(ip->addrs));
    brelse(bp);
    ip->valid = 1;
    اگر (ip->type == 0)
      panic("ilock: no type");
  }
}

// Unlock the given inode.
خالی
iunlock(ساخت inode *ip)
{
  اگر (ip == 0 || !holdingsleep(&ip->تالا) || ip->ref < 1)
    panic("iunlock");

  releasesleep(&ip->تالا);
}

// Mark the on-disk inode free.
ساکن خالی
ifree(uint dev, uint inum)
{
  ساخت buf *bp = bread(dev, IBLOCK(inum, sb));
  ساخت dinode *dip = (ساخت dinode *)bp->ڈیٹا + inum % IPB;
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
خالی
iput(ساخت inode *ip)
{
  acquire(&itable.تالا);

  // Last reference of an unlinked inode?  Capture dev/inum before ref--,
  // since once ref hits 0, ip may be recycled by a concurrent iget()
  // for a different inum.
  صحیح آخری = (ip->ref == 1 && ip->valid && ip->nlink == 0);
  uint dev = ip->dev, inum = ip->inum;

  اگر (آخری) {
    // ip->ref == 1 means no other process can have ip locked.
    acquiresleep(&ip->تالا);
    release(&itable.تالا);

    itrunc(ip); // free the data blocks (type stays nonzero on disk)
    ip->valid = 0;

    releasesleep(&ip->تالا);

    acquire(&itable.تالا);
  }

  ip->ref--;
  release(&itable.تالا);

  اگر (آخری)
    ifree(dev, inum); // now clear type on disk: inum becomes allocatable
}

// Common idiom: unlock, then put.
خالی
iunlockput(ساخت inode *ip)
{
  iunlock(ip);
  iput(ip);
}

خالی
ireclaim(صحیح dev)
{
  برائے (صحیح inum = 1; inum < sb.ninodes; inum++) {
    ساخت inode *ip = 0;
    ساخت buf *bp = bread(dev, IBLOCK(inum, sb));
    ساخت dinode *dip = (ساخت dinode *)bp->ڈیٹا + inum % IPB;
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
      انتہا_op();
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
ساکن uint
bmap(ساخت inode *ip, uint bn)
{
  uint addr, *a;
  ساخت buf *bp;

  اگر (bn < NDIRECT) {
    اگر ((addr = ip->addrs[bn]) == 0) {
      addr = balloc(ip->dev);
      اگر (addr == 0)
        واپس 0;
      ip->addrs[bn] = addr;
    }
    واپس addr;
  }
  bn -= NDIRECT;

  اگر (bn < NINDIRECT) {
    // Load indirect block, allocating if necessary.
    اگر ((addr = ip->addrs[NDIRECT]) == 0) {
      addr = balloc(ip->dev);
      اگر (addr == 0)
        واپس 0;
      ip->addrs[NDIRECT] = addr;
    }
    bp = bread(ip->dev, addr);
    a = (uint *)bp->ڈیٹا;
    اگر ((addr = a[bn]) == 0) {
      addr = balloc(ip->dev);
      اگر (addr) {
        a[bn] = addr;
        log_write(bp);
      }
    }
    brelse(bp);
    واپس addr;
  }

  panic("bmap: out of range");
}

// Truncate inode (discard contents).
// Caller must hold ip->lock.
خالی
itrunc(ساخت inode *ip)
{
  صحیح i, j;
  ساخت buf *bp;
  uint *a;

  برائے (i = 0; i < NDIRECT; i++) {
    اگر (ip->addrs[i]) {
      bfree(ip->dev, ip->addrs[i]);
      ip->addrs[i] = 0;
    }
  }

  اگر (ip->addrs[NDIRECT]) {
    bp = bread(ip->dev, ip->addrs[NDIRECT]);
    a = (uint *)bp->ڈیٹا;
    برائے (j = 0; j < NINDIRECT; j++) {
      اگر (a[j])
        bfree(ip->dev, a[j]);
    }
    brelse(bp);
    bfree(ip->dev, ip->addrs[NDIRECT]);
    ip->addrs[NDIRECT] = 0;
  }

  ip->سائز = 0;
  iupdate(ip);
}

// Copy stat information from inode.
// Caller must hold ip->lock.
خالی
stati(ساخت inode *ip, ساخت stat *st)
{
  st->dev = ip->dev;
  st->ino = ip->inum;
  st->type = ip->type;
  st->nlink = ip->nlink;
  st->سائز = ip->سائز;
}

// Read data from inode.
// Caller must hold ip->lock.
// If user_dst==1, then dst is a user virtual address;
// otherwise, dst is a kernel address.
صحیح
readi(ساخت inode *ip, صحیح user_dst, uint64 dst, uint off, uint n)
{
  uint tot, m;
  ساخت buf *bp;

  اگر (off > ip->سائز || off + n < off)
    واپس 0;
  اگر (off + n > ip->سائز)
    n = ip->سائز - off;

  برائے (tot = 0; tot < n; tot += m, off += m, dst += m) {
    uint addr = bmap(ip, off / BSIZE);
    اگر (addr == 0)
      توڑو;
    bp = bread(ip->dev, addr);
    m = min(n - tot, BSIZE - off % BSIZE);
    اگر (either_copyout(user_dst, dst, bp->ڈیٹا + (off % BSIZE), m) == -1) {
      brelse(bp);
      tot = -1;
      توڑو;
    }
    brelse(bp);
  }
  واپس tot;
}

// Write data to inode.
// Caller must hold ip->lock.
// If user_src==1, then src is a user virtual address;
// otherwise, src is a kernel address.
// Returns the number of bytes successfully written.
// If the return value is less than the requested n,
// there was an error of some kind.
صحیح
writei(ساخت inode *ip, صحیح user_src, uint64 src, uint off, uint n)
{
  uint tot, m;
  ساخت buf *bp;

  اگر (off > ip->سائز || off + n < off)
    واپس -1;
  اگر (off + n > MAXFILE * BSIZE)
    واپس -1;

  برائے (tot = 0; tot < n; tot += m, off += m, src += m) {
    uint addr = bmap(ip, off / BSIZE);
    اگر (addr == 0)
      توڑو;
    bp = bread(ip->dev, addr);
    m = min(n - tot, BSIZE - off % BSIZE);
    اگر (either_copyin(bp->ڈیٹا + (off % BSIZE), user_src, src, m) == -1) {
      // Might have partially updated the block, so we need to log it.
      log_write(bp);
      brelse(bp);
      توڑو;
    }
    log_write(bp);
    brelse(bp);
  }

  اگر (off > ip->سائز)
    ip->سائز = off;

  // write the i-node back to disk even if the size didn't change
  // because the loop above might have called bmap() and added a new
  // block to ip->addrs[].
  iupdate(ip);

  واپس tot;
}

// Directories

صحیح
namecmp(ثابت حرف *s, ثابت حرف *t)
{
  واپس لڑی_موازنہ(s, t, DIRSIZ);
}

// Look for a directory entry in a directory.
// If found, set *poff to byte offset of entry.
ساخت inode *
dirlookup(ساخت inode *dp, حرف *name, uint *poff)
{
  uint off, inum;
  ساخت dirent de;

  اگر (dp->type != T_DIR)
    panic("dirlookup not DIR");

  برائے (off = 0; off < dp->سائز; off += حجم(de)) {
    اگر (readi(dp, 0, (uint64)&de, off, حجم(de)) != حجم(de))
      panic("dirlookup read");
    اگر (de.inum == 0)
      جاری;
    اگر (namecmp(name, de.name) == 0) {
      // entry matches path element
      اگر (poff)
        *poff = off;
      inum = de.inum;
      واپس iget(dp->dev, inum);
    }
  }

  واپس 0;
}

// Write a new directory entry (name, inum) into the directory dp.
// Returns 0 on success, -1 on failure (e.g. out of disk blocks).
صحیح
dirlink(ساخت inode *dp, حرف *name, uint inum)
{
  صحیح off;
  ساخت dirent de;
  ساخت inode *ip;

  // Check that name is not present.
  اگر ((ip = dirlookup(dp, name, 0)) != 0) {
    iput(ip);
    واپس -1;
  }

  // Look for an empty dirent.
  برائے (off = 0; off < dp->سائز; off += حجم(de)) {
    اگر (readi(dp, 0, (uint64)&de, off, حجم(de)) != حجم(de))
      panic("dirlink read");
    اگر (de.inum == 0)
      توڑو;
  }

  strncpy(de.name, name, DIRSIZ);
  de.inum = inum;
  اگر (writei(dp, 0, (uint64)&de, off, حجم(de)) != حجم(de))
    واپس -1;

  واپس 0;
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
ساکن حرف *
skipelem(حرف *راستہ, حرف *name)
{
  حرف *s;
  صحیح len;

  جب (*راستہ == '/')
    راستہ++;
  اگر (*راستہ == 0)
    واپس 0;
  s = راستہ;
  جب (*راستہ != '/' && *راستہ != 0)
    راستہ++;
  len = راستہ - s;
  اگر (len >= DIRSIZ)
    memmove(name, s, DIRSIZ);
  ورنہ {
    memmove(name, s, len);
    name[len] = 0;
  }
  جب (*راستہ == '/')
    راستہ++;
  واپس راستہ;
}

// Look up and return the inode for a path name.
// If parent != 0, return the inode for the parent and copy the final
// path element into name, which must have room for DIRSIZ bytes.
// Must be called inside a transaction since it calls iput().
ساکن ساخت inode *
namex(حرف *راستہ, صحیح nameiparent, حرف *name)
{
  ساخت inode *ip, *اگلا;

  اگر (*راستہ == '/')
    ip = iget(ROOTDEV, ROOTINO);
  ورنہ
    ip = idup(myproc()->cwd);

  جب ((راستہ = skipelem(راستہ, name)) != 0) {
    ilock(ip);
    اگر (ip->type != T_DIR) {
      iunlockput(ip);
      واپس 0;
    }
    اگر (ip->nlink == 0) {
      iunlockput(ip);
      واپس 0;
    }
    اگر (nameiparent && *راستہ == '\0') {
      // Stop one level early.
      iunlock(ip);
      واپس ip;
    }
    اگر ((اگلا = dirlookup(ip, name, 0)) == 0) {
      iunlockput(ip);
      واپس 0;
    }
    iunlockput(ip);
    ip = اگلا;
  }
  اگر (nameiparent) {
    iput(ip);
    واپس 0;
  }
  واپس ip;
}

ساخت inode *
namei(حرف *راستہ)
{
  حرف name[DIRSIZ];
  واپس namex(راستہ, 0, name);
}

ساخت inode *
nameiparent(حرف *راستہ, حرف *name)
{
  واپس namex(راستہ, 1, name);
}
