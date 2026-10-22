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

#परिभाषय min(a, b) ((a) < (b) ? (a) : (b))
// there should be one superblock per disk device, but we run with
// only one device
संरचना superblock sb;

// Read the super block.
स्थिर शून्य
readsb(पूर्णाङ्क dev, संरचना superblock *sb)
{
  संरचना buf *bp;

  bp = bread(dev, 1);
  memmove(sb, bp->दत्त, परिमाण(*sb));
  brelse(bp);
}

// Init fs
शून्य
fsinit(पूर्णाङ्क dev)
{
  readsb(dev, &sb);
  यदि (sb.magic != FSMAGIC)
    panic("invalid file system");
  initlog(dev, &sb);
  ireclaim(dev);
}

// Zero a block.
स्थिर शून्य
bzero(पूर्णाङ्क dev, पूर्णाङ्क bno)
{
  संरचना buf *bp;

  bp = bread(dev, bno);
  स्मृतिं_पूरय(bp->दत्त, 0, BSIZE);
  log_write(bp);
  brelse(bp);
}

// Blocks.

// Allocate a zeroed disk block.
// returns 0 if out of disk space.
स्थिर uint
balloc(uint dev)
{
  पूर्णाङ्क b, bi, m;
  संरचना buf *bp;

  bp = 0;
  प्रति (b = 0; b < sb.आकार; b += BPB) {
    bp = bread(dev, BBLOCK(b, sb));
    प्रति (bi = 0; bi < BPB && b + bi < sb.आकार; bi++) {
      m = 1 << (bi % 8);
      यदि ((bp->दत्त[bi / 8] & m) == 0) { // Is block free?
        bp->दत्त[bi / 8] |= m;           // Mark block in use.
        log_write(bp);
        brelse(bp);
        bzero(dev, b + bi);
        प्रत्यागम b + bi;
      }
    }
    brelse(bp);
  }
  printk("balloc: out of blocks\n");
  प्रत्यागम 0;
}

// Free a disk block.
स्थिर शून्य
bfree(पूर्णाङ्क dev, uint b)
{
  संरचना buf *bp;
  पूर्णाङ्क bi, m;

  bp = bread(dev, BBLOCK(b, sb));
  bi = b % BPB;
  m = 1 << (bi % 8);
  यदि ((bp->दत्त[bi / 8] & m) == 0)
    panic("freeing free block");
  bp->दत्त[bi / 8] &= ~m;
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
  संरचना spinlock तालकम्;
  संरचना inode inode[NINODE];
} itable;

शून्य
iinit()
{
  पूर्णाङ्क i = 0;

  initlock(&itable.तालकम्, "itable");
  प्रति (i = 0; i < NINODE; i++) {
    initsleeplock(&itable.inode[i].तालकम्, "inode");
  }
}

स्थिर संरचना inode *iget(uint dev, uint inum);

// Allocate an inode on device dev.
// Mark it as allocated by  giving it type type.
// Returns an unlocked but allocated and referenced inode,
// or NULL if there is no free inode.
संरचना inode *
ialloc(uint dev, ह्रस्व type)
{
  पूर्णाङ्क inum;
  संरचना buf *bp;
  संरचना dinode *dip;

  प्रति (inum = 1; inum < sb.ninodes; inum++) {
    bp = bread(dev, IBLOCK(inum, sb));
    dip = (संरचना dinode *)bp->दत्त + inum % IPB;
    यदि (dip->type == 0) { // a free inode
      स्मृतिं_पूरय(dip, 0, परिमाण(*dip));
      dip->type = type;
      log_write(bp); // mark it allocated on the disk
      brelse(bp);
      प्रत्यागम iget(dev, inum);
    }
    brelse(bp);
  }
  printk("ialloc: no inodes\n");
  प्रत्यागम 0;
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
  dip = (संरचना dinode *)bp->दत्त + ip->inum % IPB;
  dip->type = ip->type;
  dip->major = ip->major;
  dip->minor = ip->minor;
  dip->nlink = ip->nlink;
  dip->आकार = ip->आकार;
  memmove(dip->addrs, ip->addrs, परिमाण(ip->addrs));
  log_write(bp);
  brelse(bp);
}

// Find the inode with number inum on device dev
// and return the in-memory copy. Does not lock
// the inode and does not read it from disk.
स्थिर संरचना inode *
iget(uint dev, uint inum)
{
  संरचना inode *ip, *रिक्तानि;

  acquire(&itable.तालकम्);

  // Is the inode already in the table?
  रिक्तानि = 0;
  प्रति (ip = &itable.inode[0]; ip < &itable.inode[NINODE]; ip++) {
    यदि (ip->ref > 0 && ip->dev == dev && ip->inum == inum) {
      ip->ref++;
      release(&itable.तालकम्);
      प्रत्यागम ip;
    }
    यदि (रिक्तानि == 0 && ip->ref == 0) // Remember empty slot.
      रिक्तानि = ip;
  }

  // Recycle an inode entry.
  यदि (रिक्तानि == 0)
    panic("iget: no inodes");

  ip = रिक्तानि;
  ip->dev = dev;
  ip->inum = inum;
  ip->ref = 1;
  ip->valid = 0;
  release(&itable.तालकम्);

  प्रत्यागम ip;
}

// Increment reference count for ip.
// Returns ip to enable ip = idup(ip1) idiom.
संरचना inode *
idup(संरचना inode *ip)
{
  acquire(&itable.तालकम्);
  ip->ref++;
  release(&itable.तालकम्);
  प्रत्यागम ip;
}

// Lock the given inode.
// Reads the inode from disk if necessary.
शून्य
ilock(संरचना inode *ip)
{
  संरचना buf *bp;
  संरचना dinode *dip;

  यदि (ip == 0 || ip->ref < 1)
    panic("ilock");

  acquiresleep(&ip->तालकम्);

  यदि (ip->valid == 0) {
    bp = bread(ip->dev, IBLOCK(ip->inum, sb));
    dip = (संरचना dinode *)bp->दत्त + ip->inum % IPB;
    ip->type = dip->type;
    ip->major = dip->major;
    ip->minor = dip->minor;
    ip->nlink = dip->nlink;
    ip->आकार = dip->आकार;
    memmove(ip->addrs, dip->addrs, परिमाण(ip->addrs));
    brelse(bp);
    ip->valid = 1;
    यदि (ip->type == 0)
      panic("ilock: no type");
  }
}

// Unlock the given inode.
शून्य
iunlock(संरचना inode *ip)
{
  यदि (ip == 0 || !holdingsleep(&ip->तालकम्) || ip->ref < 1)
    panic("iunlock");

  releasesleep(&ip->तालकम्);
}

// Mark the on-disk inode free.
स्थिर शून्य
ifree(uint dev, uint inum)
{
  संरचना buf *bp = bread(dev, IBLOCK(inum, sb));
  संरचना dinode *dip = (संरचना dinode *)bp->दत्त + inum % IPB;
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
  acquire(&itable.तालकम्);

  // Last reference of an unlinked inode?  Capture dev/inum before ref--,
  // since once ref hits 0, ip may be recycled by a concurrent iget()
  // for a different inum.
  पूर्णाङ्क चरमः = (ip->ref == 1 && ip->valid && ip->nlink == 0);
  uint dev = ip->dev, inum = ip->inum;

  यदि (चरमः) {
    // ip->ref == 1 means no other process can have ip locked.
    acquiresleep(&ip->तालकम्);
    release(&itable.तालकम्);

    itrunc(ip); // free the data blocks (type stays nonzero on disk)
    ip->valid = 0;

    releasesleep(&ip->तालकम्);

    acquire(&itable.तालकम्);
  }

  ip->ref--;
  release(&itable.तालकम्);

  यदि (चरमः)
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
ireclaim(पूर्णाङ्क dev)
{
  प्रति (पूर्णाङ्क inum = 1; inum < sb.ninodes; inum++) {
    संरचना inode *ip = 0;
    संरचना buf *bp = bread(dev, IBLOCK(inum, sb));
    संरचना dinode *dip = (संरचना dinode *)bp->दत्त + inum % IPB;
    यदि (dip->type != 0 && dip->nlink == 0) { // is an orphaned inode
      printk("ireclaim: orphaned inode %d\n", inum);
      ip = iget(dev, inum);
    }
    brelse(bp);
    यदि (ip) {
      begin_op();
      ilock(ip);
      iunlock(ip);
      iput(ip);
      अन्तः_op();
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

  यदि (bn < NDIRECT) {
    यदि ((addr = ip->addrs[bn]) == 0) {
      addr = balloc(ip->dev);
      यदि (addr == 0)
        प्रत्यागम 0;
      ip->addrs[bn] = addr;
    }
    प्रत्यागम addr;
  }
  bn -= NDIRECT;

  यदि (bn < NINDIRECT) {
    // Load indirect block, allocating if necessary.
    यदि ((addr = ip->addrs[NDIRECT]) == 0) {
      addr = balloc(ip->dev);
      यदि (addr == 0)
        प्रत्यागम 0;
      ip->addrs[NDIRECT] = addr;
    }
    bp = bread(ip->dev, addr);
    a = (uint *)bp->दत्त;
    यदि ((addr = a[bn]) == 0) {
      addr = balloc(ip->dev);
      यदि (addr) {
        a[bn] = addr;
        log_write(bp);
      }
    }
    brelse(bp);
    प्रत्यागम addr;
  }

  panic("bmap: out of range");
}

// Truncate inode (discard contents).
// Caller must hold ip->lock.
शून्य
itrunc(संरचना inode *ip)
{
  पूर्णाङ्क i, j;
  संरचना buf *bp;
  uint *a;

  प्रति (i = 0; i < NDIRECT; i++) {
    यदि (ip->addrs[i]) {
      bfree(ip->dev, ip->addrs[i]);
      ip->addrs[i] = 0;
    }
  }

  यदि (ip->addrs[NDIRECT]) {
    bp = bread(ip->dev, ip->addrs[NDIRECT]);
    a = (uint *)bp->दत्त;
    प्रति (j = 0; j < NINDIRECT; j++) {
      यदि (a[j])
        bfree(ip->dev, a[j]);
    }
    brelse(bp);
    bfree(ip->dev, ip->addrs[NDIRECT]);
    ip->addrs[NDIRECT] = 0;
  }

  ip->आकार = 0;
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
  st->आकार = ip->आकार;
}

// Read data from inode.
// Caller must hold ip->lock.
// If user_dst==1, then dst is a user virtual address;
// otherwise, dst is a kernel address.
पूर्णाङ्क
readi(संरचना inode *ip, पूर्णाङ्क user_dst, uint64 dst, uint off, uint n)
{
  uint tot, m;
  संरचना buf *bp;

  यदि (off > ip->आकार || off + n < off)
    प्रत्यागम 0;
  यदि (off + n > ip->आकार)
    n = ip->आकार - off;

  प्रति (tot = 0; tot < n; tot += m, off += m, dst += m) {
    uint addr = bmap(ip, off / BSIZE);
    यदि (addr == 0)
      भङ्ग;
    bp = bread(ip->dev, addr);
    m = min(n - tot, BSIZE - off % BSIZE);
    यदि (either_copyout(user_dst, dst, bp->दत्त + (off % BSIZE), m) == -1) {
      brelse(bp);
      tot = -1;
      भङ्ग;
    }
    brelse(bp);
  }
  प्रत्यागम tot;
}

// Write data to inode.
// Caller must hold ip->lock.
// If user_src==1, then src is a user virtual address;
// otherwise, src is a kernel address.
// Returns the number of bytes successfully written.
// If the return value is less than the requested n,
// there was an error of some kind.
पूर्णाङ्क
writei(संरचना inode *ip, पूर्णाङ्क user_src, uint64 src, uint off, uint n)
{
  uint tot, m;
  संरचना buf *bp;

  यदि (off > ip->आकार || off + n < off)
    प्रत्यागम -1;
  यदि (off + n > MAXFILE * BSIZE)
    प्रत्यागम -1;

  प्रति (tot = 0; tot < n; tot += m, off += m, src += m) {
    uint addr = bmap(ip, off / BSIZE);
    यदि (addr == 0)
      भङ्ग;
    bp = bread(ip->dev, addr);
    m = min(n - tot, BSIZE - off % BSIZE);
    यदि (either_copyin(bp->दत्त + (off % BSIZE), user_src, src, m) == -1) {
      // Might have partially updated the block, so we need to log it.
      log_write(bp);
      brelse(bp);
      भङ्ग;
    }
    log_write(bp);
    brelse(bp);
  }

  यदि (off > ip->आकार)
    ip->आकार = off;

  // write the i-node back to disk even if the size didn't change
  // because the loop above might have called bmap() and added a new
  // block to ip->addrs[].
  iupdate(ip);

  प्रत्यागम tot;
}

// Directories

पूर्णाङ्क
namecmp(ध्रुव वर्ण *s, ध्रुव वर्ण *t)
{
  प्रत्यागम वाक्ये_तुलय(s, t, DIRSIZ);
}

// Look for a directory entry in a directory.
// If found, set *poff to byte offset of entry.
संरचना inode *
dirlookup(संरचना inode *dp, वर्ण *name, uint *poff)
{
  uint off, inum;
  संरचना dirent de;

  यदि (dp->type != T_DIR)
    panic("dirlookup not DIR");

  प्रति (off = 0; off < dp->आकार; off += परिमाण(de)) {
    यदि (readi(dp, 0, (uint64)&de, off, परिमाण(de)) != परिमाण(de))
      panic("dirlookup read");
    यदि (de.inum == 0)
      अनुवर्त;
    यदि (namecmp(name, de.name) == 0) {
      // entry matches path element
      यदि (poff)
        *poff = off;
      inum = de.inum;
      प्रत्यागम iget(dp->dev, inum);
    }
  }

  प्रत्यागम 0;
}

// Write a new directory entry (name, inum) into the directory dp.
// Returns 0 on success, -1 on failure (e.g. out of disk blocks).
पूर्णाङ्क
dirlink(संरचना inode *dp, वर्ण *name, uint inum)
{
  पूर्णाङ्क off;
  संरचना dirent de;
  संरचना inode *ip;

  // Check that name is not present.
  यदि ((ip = dirlookup(dp, name, 0)) != 0) {
    iput(ip);
    प्रत्यागम -1;
  }

  // Look for an empty dirent.
  प्रति (off = 0; off < dp->आकार; off += परिमाण(de)) {
    यदि (readi(dp, 0, (uint64)&de, off, परिमाण(de)) != परिमाण(de))
      panic("dirlink read");
    यदि (de.inum == 0)
      भङ्ग;
  }

  strncpy(de.name, name, DIRSIZ);
  de.inum = inum;
  यदि (writei(dp, 0, (uint64)&de, off, परिमाण(de)) != परिमाण(de))
    प्रत्यागम -1;

  प्रत्यागम 0;
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
स्थिर वर्ण *
skipelem(वर्ण *मार्गः, वर्ण *name)
{
  वर्ण *s;
  पूर्णाङ्क len;

  यावत् (*मार्गः == '/')
    मार्गः++;
  यदि (*मार्गः == 0)
    प्रत्यागम 0;
  s = मार्गः;
  यावत् (*मार्गः != '/' && *मार्गः != 0)
    मार्गः++;
  len = मार्गः - s;
  यदि (len >= DIRSIZ)
    memmove(name, s, DIRSIZ);
  अन्यथा {
    memmove(name, s, len);
    name[len] = 0;
  }
  यावत् (*मार्गः == '/')
    मार्गः++;
  प्रत्यागम मार्गः;
}

// Look up and return the inode for a path name.
// If parent != 0, return the inode for the parent and copy the final
// path element into name, which must have room for DIRSIZ bytes.
// Must be called inside a transaction since it calls iput().
स्थिर संरचना inode *
namex(वर्ण *मार्गः, पूर्णाङ्क nameiparent, वर्ण *name)
{
  संरचना inode *ip, *अग्र;

  यदि (*मार्गः == '/')
    ip = iget(ROOTDEV, ROOTINO);
  अन्यथा
    ip = idup(myproc()->cwd);

  यावत् ((मार्गः = skipelem(मार्गः, name)) != 0) {
    ilock(ip);
    यदि (ip->type != T_DIR) {
      iunlockput(ip);
      प्रत्यागम 0;
    }
    यदि (ip->nlink == 0) {
      iunlockput(ip);
      प्रत्यागम 0;
    }
    यदि (nameiparent && *मार्गः == '\0') {
      // Stop one level early.
      iunlock(ip);
      प्रत्यागम ip;
    }
    यदि ((अग्र = dirlookup(ip, name, 0)) == 0) {
      iunlockput(ip);
      प्रत्यागम 0;
    }
    iunlockput(ip);
    ip = अग्र;
  }
  यदि (nameiparent) {
    iput(ip);
    प्रत्यागम 0;
  }
  प्रत्यागम ip;
}

संरचना inode *
namei(वर्ण *मार्गः)
{
  वर्ण name[DIRSIZ];
  प्रत्यागम namex(मार्गः, 0, name);
}

संरचना inode *
nameiparent(वर्ण *मार्गः, वर्ण *name)
{
  प्रत्यागम namex(मार्गः, 1, name);
}
