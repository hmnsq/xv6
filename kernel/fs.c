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

#ضمن "types.h"
#ضمن "riscv.h"
#ضمن "defs.h"
#ضمن "param.h"
#ضمن "stat.h"
#ضمن "spinlock.h"
#ضمن "proc.h"
#ضمن "sleeplock.h"
#ضمن "fs.h"
#ضمن "buf.h"
#ضمن "file.h"

#تعریف_کن min(a, b) ((a) < (b) ? (a) : (b))
// there should be one superblock per disk device, but we run with
// only one device
بنية superblock sb;

// Read the super block.
ساكن فراغ
readsb(صحيح dev, بنية superblock *sb)
{
  بنية buf *bp;

  bp = bread(dev, 1);
  memmove(sb, bp->داده, حجم(*sb));
  brelse(bp);
}

// Init fs
فراغ
fsinit(صحيح dev)
{
  readsb(dev, &sb);
  إذا (sb.magic != FSMAGIC)
    panic("invalid file system");
  initlog(dev, &sb);
  ireclaim(dev);
}

// Zero a block.
ساكن فراغ
bzero(صحيح dev, صحيح bno)
{
  بنية buf *bp;

  bp = bread(dev, bno);
  memset(bp->داده, 0, BSIZE);
  log_write(bp);
  brelse(bp);
}

// Blocks.

// Allocate a zeroed disk block.
// returns 0 if out of disk space.
ساكن uint
balloc(uint dev)
{
  صحيح b, bi, m;
  بنية buf *bp;

  bp = 0;
  لكل (b = 0; b < sb.اندازه; b += BPB) {
    bp = bread(dev, BBLOCK(b, sb));
    لكل (bi = 0; bi < BPB && b + bi < sb.اندازه; bi++) {
      m = 1 << (bi % 8);
      إذا ((bp->داده[bi / 8] & m) == 0) { // Is block free?
        bp->داده[bi / 8] |= m;           // Mark block in use.
        log_write(bp);
        brelse(bp);
        bzero(dev, b + bi);
        أرجع b + bi;
      }
    }
    brelse(bp);
  }
  printk("balloc: out of blocks\n");
  أرجع 0;
}

// Free a disk block.
ساكن فراغ
bfree(صحيح dev, uint b)
{
  بنية buf *bp;
  صحيح bi, m;

  bp = bread(dev, BBLOCK(b, sb));
  bi = b % BPB;
  m = 1 << (bi % 8);
  إذا ((bp->داده[bi / 8] & m) == 0)
    panic("freeing free block");
  bp->داده[bi / 8] &= ~m;
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

بنية {
  بنية spinlock قفل;
  بنية inode inode[NINODE];
} itable;

فراغ
iinit()
{
  صحيح i = 0;

  initlock(&itable.قفل, "itable");
  لكل (i = 0; i < NINODE; i++) {
    initsleeplock(&itable.inode[i].قفل, "inode");
  }
}

ساكن بنية inode *iget(uint dev, uint inum);

// Allocate an inode on device dev.
// Mark it as allocated by  giving it type type.
// Returns an unlocked but allocated and referenced inode,
// or NULL if there is no free inode.
بنية inode *
ialloc(uint dev, کوتاه type)
{
  صحيح inum;
  بنية buf *bp;
  بنية dinode *dip;

  لكل (inum = 1; inum < sb.ninodes; inum++) {
    bp = bread(dev, IBLOCK(inum, sb));
    dip = (بنية dinode *)bp->داده + inum % IPB;
    إذا (dip->type == 0) { // a free inode
      memset(dip, 0, حجم(*dip));
      dip->type = type;
      log_write(bp); // mark it allocated on the disk
      brelse(bp);
      أرجع iget(dev, inum);
    }
    brelse(bp);
  }
  printk("ialloc: no inodes\n");
  أرجع 0;
}

// Copy a modified in-memory inode to disk.
// Must be called after every change to an ip->xxx field
// that lives on disk.
// Caller must hold ip->lock.
فراغ
iupdate(بنية inode *ip)
{
  بنية buf *bp;
  بنية dinode *dip;

  bp = bread(ip->dev, IBLOCK(ip->inum, sb));
  dip = (بنية dinode *)bp->داده + ip->inum % IPB;
  dip->type = ip->type;
  dip->major = ip->major;
  dip->minor = ip->minor;
  dip->nlink = ip->nlink;
  dip->اندازه = ip->اندازه;
  memmove(dip->addrs, ip->addrs, حجم(ip->addrs));
  log_write(bp);
  brelse(bp);
}

// Find the inode with number inum on device dev
// and return the in-memory copy. Does not lock
// the inode and does not read it from disk.
ساكن بنية inode *
iget(uint dev, uint inum)
{
  بنية inode *ip, *خالی;

  acquire(&itable.قفل);

  // Is the inode already in the table?
  خالی = 0;
  لكل (ip = &itable.inode[0]; ip < &itable.inode[NINODE]; ip++) {
    إذا (ip->ref > 0 && ip->dev == dev && ip->inum == inum) {
      ip->ref++;
      release(&itable.قفل);
      أرجع ip;
    }
    إذا (خالی == 0 && ip->ref == 0) // Remember empty slot.
      خالی = ip;
  }

  // Recycle an inode entry.
  إذا (خالی == 0)
    panic("iget: no inodes");

  ip = خالی;
  ip->dev = dev;
  ip->inum = inum;
  ip->ref = 1;
  ip->valid = 0;
  release(&itable.قفل);

  أرجع ip;
}

// Increment reference count for ip.
// Returns ip to enable ip = idup(ip1) idiom.
بنية inode *
idup(بنية inode *ip)
{
  acquire(&itable.قفل);
  ip->ref++;
  release(&itable.قفل);
  أرجع ip;
}

// Lock the given inode.
// Reads the inode from disk if necessary.
فراغ
ilock(بنية inode *ip)
{
  بنية buf *bp;
  بنية dinode *dip;

  إذا (ip == 0 || ip->ref < 1)
    panic("ilock");

  acquiresleep(&ip->قفل);

  إذا (ip->valid == 0) {
    bp = bread(ip->dev, IBLOCK(ip->inum, sb));
    dip = (بنية dinode *)bp->داده + ip->inum % IPB;
    ip->type = dip->type;
    ip->major = dip->major;
    ip->minor = dip->minor;
    ip->nlink = dip->nlink;
    ip->اندازه = dip->اندازه;
    memmove(ip->addrs, dip->addrs, حجم(ip->addrs));
    brelse(bp);
    ip->valid = 1;
    إذا (ip->type == 0)
      panic("ilock: no type");
  }
}

// Unlock the given inode.
فراغ
iunlock(بنية inode *ip)
{
  إذا (ip == 0 || !holdingsleep(&ip->قفل) || ip->ref < 1)
    panic("iunlock");

  releasesleep(&ip->قفل);
}

// Mark the on-disk inode free.
ساكن فراغ
ifree(uint dev, uint inum)
{
  بنية buf *bp = bread(dev, IBLOCK(inum, sb));
  بنية dinode *dip = (بنية dinode *)bp->داده + inum % IPB;
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
فراغ
iput(بنية inode *ip)
{
  acquire(&itable.قفل);

  // Last reference of an unlinked inode?  Capture dev/inum before ref--,
  // since once ref hits 0, ip may be recycled by a concurrent iget()
  // for a different inum.
  صحيح آخرین = (ip->ref == 1 && ip->valid && ip->nlink == 0);
  uint dev = ip->dev, inum = ip->inum;

  إذا (آخرین) {
    // ip->ref == 1 means no other process can have ip locked.
    acquiresleep(&ip->قفل);
    release(&itable.قفل);

    itrunc(ip); // free the data blocks (type stays nonzero on disk)
    ip->valid = 0;

    releasesleep(&ip->قفل);

    acquire(&itable.قفل);
  }

  ip->ref--;
  release(&itable.قفل);

  إذا (آخرین)
    ifree(dev, inum); // now clear type on disk: inum becomes allocatable
}

// Common idiom: unlock, then put.
فراغ
iunlockput(بنية inode *ip)
{
  iunlock(ip);
  iput(ip);
}

فراغ
ireclaim(صحيح dev)
{
  لكل (صحيح inum = 1; inum < sb.ninodes; inum++) {
    بنية inode *ip = 0;
    بنية buf *bp = bread(dev, IBLOCK(inum, sb));
    بنية dinode *dip = (بنية dinode *)bp->داده + inum % IPB;
    إذا (dip->type != 0 && dip->nlink == 0) { // is an orphaned inode
      printk("ireclaim: orphaned inode %d\n", inum);
      ip = iget(dev, inum);
    }
    brelse(bp);
    إذا (ip) {
      begin_op();
      ilock(ip);
      iunlock(ip);
      iput(ip);
      انتها_op();
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
ساكن uint
bmap(بنية inode *ip, uint bn)
{
  uint addr, *a;
  بنية buf *bp;

  إذا (bn < NDIRECT) {
    إذا ((addr = ip->addrs[bn]) == 0) {
      addr = balloc(ip->dev);
      إذا (addr == 0)
        أرجع 0;
      ip->addrs[bn] = addr;
    }
    أرجع addr;
  }
  bn -= NDIRECT;

  إذا (bn < NINDIRECT) {
    // Load indirect block, allocating if necessary.
    إذا ((addr = ip->addrs[NDIRECT]) == 0) {
      addr = balloc(ip->dev);
      إذا (addr == 0)
        أرجع 0;
      ip->addrs[NDIRECT] = addr;
    }
    bp = bread(ip->dev, addr);
    a = (uint *)bp->داده;
    إذا ((addr = a[bn]) == 0) {
      addr = balloc(ip->dev);
      إذا (addr) {
        a[bn] = addr;
        log_write(bp);
      }
    }
    brelse(bp);
    أرجع addr;
  }

  panic("bmap: out of range");
}

// Truncate inode (discard contents).
// Caller must hold ip->lock.
فراغ
itrunc(بنية inode *ip)
{
  صحيح i, j;
  بنية buf *bp;
  uint *a;

  لكل (i = 0; i < NDIRECT; i++) {
    إذا (ip->addrs[i]) {
      bfree(ip->dev, ip->addrs[i]);
      ip->addrs[i] = 0;
    }
  }

  إذا (ip->addrs[NDIRECT]) {
    bp = bread(ip->dev, ip->addrs[NDIRECT]);
    a = (uint *)bp->داده;
    لكل (j = 0; j < NINDIRECT; j++) {
      إذا (a[j])
        bfree(ip->dev, a[j]);
    }
    brelse(bp);
    bfree(ip->dev, ip->addrs[NDIRECT]);
    ip->addrs[NDIRECT] = 0;
  }

  ip->اندازه = 0;
  iupdate(ip);
}

// Copy stat information from inode.
// Caller must hold ip->lock.
فراغ
stati(بنية inode *ip, بنية stat *st)
{
  st->dev = ip->dev;
  st->ino = ip->inum;
  st->type = ip->type;
  st->nlink = ip->nlink;
  st->اندازه = ip->اندازه;
}

// Read data from inode.
// Caller must hold ip->lock.
// If user_dst==1, then dst is a user virtual address;
// otherwise, dst is a kernel address.
صحيح
readi(بنية inode *ip, صحيح user_dst, uint64 dst, uint off, uint n)
{
  uint tot, m;
  بنية buf *bp;

  إذا (off > ip->اندازه || off + n < off)
    أرجع 0;
  إذا (off + n > ip->اندازه)
    n = ip->اندازه - off;

  لكل (tot = 0; tot < n; tot += m, off += m, dst += m) {
    uint addr = bmap(ip, off / BSIZE);
    إذا (addr == 0)
      اكسر;
    bp = bread(ip->dev, addr);
    m = min(n - tot, BSIZE - off % BSIZE);
    إذا (either_copyout(user_dst, dst, bp->داده + (off % BSIZE), m) == -1) {
      brelse(bp);
      tot = -1;
      اكسر;
    }
    brelse(bp);
  }
  أرجع tot;
}

// Write data to inode.
// Caller must hold ip->lock.
// If user_src==1, then src is a user virtual address;
// otherwise, src is a kernel address.
// Returns the number of bytes successfully written.
// If the return value is less than the requested n,
// there was an error of some kind.
صحيح
writei(بنية inode *ip, صحيح user_src, uint64 src, uint off, uint n)
{
  uint tot, m;
  بنية buf *bp;

  إذا (off > ip->اندازه || off + n < off)
    أرجع -1;
  إذا (off + n > MAXFILE * BSIZE)
    أرجع -1;

  لكل (tot = 0; tot < n; tot += m, off += m, src += m) {
    uint addr = bmap(ip, off / BSIZE);
    إذا (addr == 0)
      اكسر;
    bp = bread(ip->dev, addr);
    m = min(n - tot, BSIZE - off % BSIZE);
    إذا (either_copyin(bp->داده + (off % BSIZE), user_src, src, m) == -1) {
      // Might have partially updated the block, so we need to log it.
      log_write(bp);
      brelse(bp);
      اكسر;
    }
    log_write(bp);
    brelse(bp);
  }

  إذا (off > ip->اندازه)
    ip->اندازه = off;

  // write the i-node back to disk even if the size didn't change
  // because the loop above might have called bmap() and added a new
  // block to ip->addrs[].
  iupdate(ip);

  أرجع tot;
}

// Directories

صحيح
namecmp(ثابت حرف *s, ثابت حرف *t)
{
  أرجع مقایسه_رشته(s, t, DIRSIZ);
}

// Look for a directory entry in a directory.
// If found, set *poff to byte offset of entry.
بنية inode *
dirlookup(بنية inode *dp, حرف *name, uint *poff)
{
  uint off, inum;
  بنية dirent de;

  إذا (dp->type != T_DIR)
    panic("dirlookup not DIR");

  لكل (off = 0; off < dp->اندازه; off += حجم(de)) {
    إذا (readi(dp, 0, (uint64)&de, off, حجم(de)) != حجم(de))
      panic("dirlookup read");
    إذا (de.inum == 0)
      استمر;
    إذا (namecmp(name, de.name) == 0) {
      // entry matches path element
      إذا (poff)
        *poff = off;
      inum = de.inum;
      أرجع iget(dp->dev, inum);
    }
  }

  أرجع 0;
}

// Write a new directory entry (name, inum) into the directory dp.
// Returns 0 on success, -1 on failure (e.g. out of disk blocks).
صحيح
dirlink(بنية inode *dp, حرف *name, uint inum)
{
  صحيح off;
  بنية dirent de;
  بنية inode *ip;

  // Check that name is not present.
  إذا ((ip = dirlookup(dp, name, 0)) != 0) {
    iput(ip);
    أرجع -1;
  }

  // Look for an empty dirent.
  لكل (off = 0; off < dp->اندازه; off += حجم(de)) {
    إذا (readi(dp, 0, (uint64)&de, off, حجم(de)) != حجم(de))
      panic("dirlink read");
    إذا (de.inum == 0)
      اكسر;
  }

  strncpy(de.name, name, DIRSIZ);
  de.inum = inum;
  إذا (writei(dp, 0, (uint64)&de, off, حجم(de)) != حجم(de))
    أرجع -1;

  أرجع 0;
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
ساكن حرف *
skipelem(حرف *مسیر, حرف *name)
{
  حرف *s;
  صحيح len;

  طالما (*مسیر == '/')
    مسیر++;
  إذا (*مسیر == 0)
    أرجع 0;
  s = مسیر;
  طالما (*مسیر != '/' && *مسیر != 0)
    مسیر++;
  len = مسیر - s;
  إذا (len >= DIRSIZ)
    memmove(name, s, DIRSIZ);
  وإلا {
    memmove(name, s, len);
    name[len] = 0;
  }
  طالما (*مسیر == '/')
    مسیر++;
  أرجع مسیر;
}

// Look up and return the inode for a path name.
// If parent != 0, return the inode for the parent and copy the final
// path element into name, which must have room for DIRSIZ bytes.
// Must be called inside a transaction since it calls iput().
ساكن بنية inode *
namex(حرف *مسیر, صحيح nameiparent, حرف *name)
{
  بنية inode *ip, *بعدی;

  إذا (*مسیر == '/')
    ip = iget(ROOTDEV, ROOTINO);
  وإلا
    ip = idup(myproc()->cwd);

  طالما ((مسیر = skipelem(مسیر, name)) != 0) {
    ilock(ip);
    إذا (ip->type != T_DIR) {
      iunlockput(ip);
      أرجع 0;
    }
    إذا (ip->nlink == 0) {
      iunlockput(ip);
      أرجع 0;
    }
    إذا (nameiparent && *مسیر == '\0') {
      // Stop one level early.
      iunlock(ip);
      أرجع ip;
    }
    إذا ((بعدی = dirlookup(ip, name, 0)) == 0) {
      iunlockput(ip);
      أرجع 0;
    }
    iunlockput(ip);
    ip = بعدی;
  }
  إذا (nameiparent) {
    iput(ip);
    أرجع 0;
  }
  أرجع ip;
}

بنية inode *
namei(حرف *مسیر)
{
  حرف name[DIRSIZ];
  أرجع namex(مسیر, 0, name);
}

بنية inode *
nameiparent(حرف *مسیر, حرف *name)
{
  أرجع namex(مسیر, 1, name);
}
