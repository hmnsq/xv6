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

#समावेश "types.h"
#समावेश "riscv.h"
#समावेश "defs.h"
#समावेश "param.h"
#समावेश "stat.h"
#समावेश "spinlock.h"
#समावेश "proc.h"
#समावेश "sleeplock.h"
#समावेश "fs.h"
#समावेश "buf.h"
#समावेश "file.h"

#घोषणा min(a, b) ((a) < (b) ? (a) : (b))
// there should be one superblock per disk device, but we run with
// only one device
काष्ठा superblock sb;

// Read the super block.
अटल व्योम
readsb(पूर्णांक dev, काष्ठा superblock *sb)
{
  काष्ठा buf *bp;

  bp = bread(dev, 1);
  स_हटाओ(sb, bp->तथ्य, माप(*sb));
  brelse(bp);
}

// Init fs
व्योम
fsinit(पूर्णांक dev)
{
  readsb(dev, &sb);
  अगर (sb.magic != FSMAGIC)
    panic("invalid file system");
  initlog(dev, &sb);
  ireclaim(dev);
}

// Zero a block.
अटल व्योम
bzero(पूर्णांक dev, पूर्णांक bno)
{
  काष्ठा buf *bp;

  bp = bread(dev, bno);
  स_रखो(bp->तथ्य, 0, BSIZE);
  log_write(bp);
  brelse(bp);
}

// Blocks.

// Allocate a zeroed disk block.
// returns 0 if out of disk space.
अटल uint
balloc(uint dev)
{
  पूर्णांक b, bi, m;
  काष्ठा buf *bp;

  bp = 0;
  क्रम (b = 0; b < sb.आकार; b += BPB) {
    bp = bread(dev, BBLOCK(b, sb));
    क्रम (bi = 0; bi < BPB && b + bi < sb.आकार; bi++) {
      m = 1 << (bi % 8);
      अगर ((bp->तथ्य[bi / 8] & m) == 0) { // Is block free?
        bp->तथ्य[bi / 8] |= m;           // Mark block in use.
        log_write(bp);
        brelse(bp);
        bzero(dev, b + bi);
        वापस b + bi;
      }
    }
    brelse(bp);
  }
  printk("balloc: out of blocks\n");
  वापस 0;
}

// Free a disk block.
अटल व्योम
bfree(पूर्णांक dev, uint b)
{
  काष्ठा buf *bp;
  पूर्णांक bi, m;

  bp = bread(dev, BBLOCK(b, sb));
  bi = b % BPB;
  m = 1 << (bi % 8);
  अगर ((bp->तथ्य[bi / 8] & m) == 0)
    panic("freeing free block");
  bp->तथ्य[bi / 8] &= ~m;
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

काष्ठा {
  काष्ठा spinlock lock;
  काष्ठा inode inode[NINODE];
} itable;

व्योम
iinit()
{
  पूर्णांक i = 0;

  initlock(&itable.lock, "itable");
  क्रम (i = 0; i < NINODE; i++) {
    initsleeplock(&itable.inode[i].lock, "inode");
  }
}

अटल काष्ठा inode *iget(uint dev, uint inum);

// Allocate an inode on device dev.
// Mark it as allocated by  giving it type type.
// Returns an unlocked but allocated and referenced inode,
// or NULL if there is no free inode.
काष्ठा inode *
ialloc(uint dev, लघु type)
{
  पूर्णांक inum;
  काष्ठा buf *bp;
  काष्ठा dinode *dip;

  क्रम (inum = 1; inum < sb.ninodes; inum++) {
    bp = bread(dev, IBLOCK(inum, sb));
    dip = (काष्ठा dinode *)bp->तथ्य + inum % IPB;
    अगर (dip->type == 0) { // a free inode
      स_रखो(dip, 0, माप(*dip));
      dip->type = type;
      log_write(bp); // mark it allocated on the disk
      brelse(bp);
      वापस iget(dev, inum);
    }
    brelse(bp);
  }
  printk("ialloc: no inodes\n");
  वापस 0;
}

// Copy a modified in-memory inode to disk.
// Must be called after every change to an ip->xxx field
// that lives on disk.
// Caller must hold ip->lock.
व्योम
iupdate(काष्ठा inode *ip)
{
  काष्ठा buf *bp;
  काष्ठा dinode *dip;

  bp = bread(ip->dev, IBLOCK(ip->inum, sb));
  dip = (काष्ठा dinode *)bp->तथ्य + ip->inum % IPB;
  dip->type = ip->type;
  dip->major = ip->major;
  dip->minor = ip->minor;
  dip->nlink = ip->nlink;
  dip->आकार = ip->आकार;
  स_हटाओ(dip->addrs, ip->addrs, माप(ip->addrs));
  log_write(bp);
  brelse(bp);
}

// Find the inode with number inum on device dev
// and return the in-memory copy. Does not lock
// the inode and does not read it from disk.
अटल काष्ठा inode *
iget(uint dev, uint inum)
{
  काष्ठा inode *ip, *empty;

  acquire(&itable.lock);

  // Is the inode already in the table?
  empty = 0;
  क्रम (ip = &itable.inode[0]; ip < &itable.inode[NINODE]; ip++) {
    अगर (ip->ref > 0 && ip->dev == dev && ip->inum == inum) {
      ip->ref++;
      release(&itable.lock);
      वापस ip;
    }
    अगर (empty == 0 && ip->ref == 0) // Remember empty slot.
      empty = ip;
  }

  // Recycle an inode entry.
  अगर (empty == 0)
    panic("iget: no inodes");

  ip = empty;
  ip->dev = dev;
  ip->inum = inum;
  ip->ref = 1;
  ip->valid = 0;
  release(&itable.lock);

  वापस ip;
}

// Increment reference count for ip.
// Returns ip to enable ip = idup(ip1) idiom.
काष्ठा inode *
idup(काष्ठा inode *ip)
{
  acquire(&itable.lock);
  ip->ref++;
  release(&itable.lock);
  वापस ip;
}

// Lock the given inode.
// Reads the inode from disk if necessary.
व्योम
ilock(काष्ठा inode *ip)
{
  काष्ठा buf *bp;
  काष्ठा dinode *dip;

  अगर (ip == 0 || ip->ref < 1)
    panic("ilock");

  acquiresleep(&ip->lock);

  अगर (ip->valid == 0) {
    bp = bread(ip->dev, IBLOCK(ip->inum, sb));
    dip = (काष्ठा dinode *)bp->तथ्य + ip->inum % IPB;
    ip->type = dip->type;
    ip->major = dip->major;
    ip->minor = dip->minor;
    ip->nlink = dip->nlink;
    ip->आकार = dip->आकार;
    स_हटाओ(ip->addrs, dip->addrs, माप(ip->addrs));
    brelse(bp);
    ip->valid = 1;
    अगर (ip->type == 0)
      panic("ilock: no type");
  }
}

// Unlock the given inode.
व्योम
iunlock(काष्ठा inode *ip)
{
  अगर (ip == 0 || !holdingsleep(&ip->lock) || ip->ref < 1)
    panic("iunlock");

  releasesleep(&ip->lock);
}

// Mark the on-disk inode free.
अटल व्योम
ifree(uint dev, uint inum)
{
  काष्ठा buf *bp = bread(dev, IBLOCK(inum, sb));
  काष्ठा dinode *dip = (काष्ठा dinode *)bp->तथ्य + inum % IPB;
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
व्योम
iput(काष्ठा inode *ip)
{
  acquire(&itable.lock);

  // Last reference of an unlinked inode?  Capture dev/inum before ref--,
  // since once ref hits 0, ip may be recycled by a concurrent iget()
  // for a different inum.
  पूर्णांक last = (ip->ref == 1 && ip->valid && ip->nlink == 0);
  uint dev = ip->dev, inum = ip->inum;

  अगर (last) {
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

  अगर (last)
    ifree(dev, inum); // now clear type on disk: inum becomes allocatable
}

// Common idiom: unlock, then put.
व्योम
iunlockput(काष्ठा inode *ip)
{
  iunlock(ip);
  iput(ip);
}

व्योम
ireclaim(पूर्णांक dev)
{
  क्रम (पूर्णांक inum = 1; inum < sb.ninodes; inum++) {
    काष्ठा inode *ip = 0;
    काष्ठा buf *bp = bread(dev, IBLOCK(inum, sb));
    काष्ठा dinode *dip = (काष्ठा dinode *)bp->तथ्य + inum % IPB;
    अगर (dip->type != 0 && dip->nlink == 0) { // is an orphaned inode
      printk("ireclaim: orphaned inode %d\n", inum);
      ip = iget(dev, inum);
    }
    brelse(bp);
    अगर (ip) {
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
अटल uint
bmap(काष्ठा inode *ip, uint bn)
{
  uint addr, *a;
  काष्ठा buf *bp;

  अगर (bn < NDIRECT) {
    अगर ((addr = ip->addrs[bn]) == 0) {
      addr = balloc(ip->dev);
      अगर (addr == 0)
        वापस 0;
      ip->addrs[bn] = addr;
    }
    वापस addr;
  }
  bn -= NDIRECT;

  अगर (bn < NINDIRECT) {
    // Load indirect block, allocating if necessary.
    अगर ((addr = ip->addrs[NDIRECT]) == 0) {
      addr = balloc(ip->dev);
      अगर (addr == 0)
        वापस 0;
      ip->addrs[NDIRECT] = addr;
    }
    bp = bread(ip->dev, addr);
    a = (uint *)bp->तथ्य;
    अगर ((addr = a[bn]) == 0) {
      addr = balloc(ip->dev);
      अगर (addr) {
        a[bn] = addr;
        log_write(bp);
      }
    }
    brelse(bp);
    वापस addr;
  }

  panic("bmap: out of range");
}

// Truncate inode (discard contents).
// Caller must hold ip->lock.
व्योम
itrunc(काष्ठा inode *ip)
{
  पूर्णांक i, j;
  काष्ठा buf *bp;
  uint *a;

  क्रम (i = 0; i < NDIRECT; i++) {
    अगर (ip->addrs[i]) {
      bfree(ip->dev, ip->addrs[i]);
      ip->addrs[i] = 0;
    }
  }

  अगर (ip->addrs[NDIRECT]) {
    bp = bread(ip->dev, ip->addrs[NDIRECT]);
    a = (uint *)bp->तथ्य;
    क्रम (j = 0; j < NINDIRECT; j++) {
      अगर (a[j])
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
व्योम
stati(काष्ठा inode *ip, काष्ठा stat *st)
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
पूर्णांक
readi(काष्ठा inode *ip, पूर्णांक user_dst, uint64 dst, uint off, uint n)
{
  uint tot, m;
  काष्ठा buf *bp;

  अगर (off > ip->आकार || off + n < off)
    वापस 0;
  अगर (off + n > ip->आकार)
    n = ip->आकार - off;

  क्रम (tot = 0; tot < n; tot += m, off += m, dst += m) {
    uint addr = bmap(ip, off / BSIZE);
    अगर (addr == 0)
      अवरोध;
    bp = bread(ip->dev, addr);
    m = min(n - tot, BSIZE - off % BSIZE);
    अगर (either_copyout(user_dst, dst, bp->तथ्य + (off % BSIZE), m) == -1) {
      brelse(bp);
      tot = -1;
      अवरोध;
    }
    brelse(bp);
  }
  वापस tot;
}

// Write data to inode.
// Caller must hold ip->lock.
// If user_src==1, then src is a user virtual address;
// otherwise, src is a kernel address.
// Returns the number of bytes successfully written.
// If the return value is less than the requested n,
// there was an error of some kind.
पूर्णांक
writei(काष्ठा inode *ip, पूर्णांक user_src, uint64 src, uint off, uint n)
{
  uint tot, m;
  काष्ठा buf *bp;

  अगर (off > ip->आकार || off + n < off)
    वापस -1;
  अगर (off + n > MAXFILE * BSIZE)
    वापस -1;

  क्रम (tot = 0; tot < n; tot += m, off += m, src += m) {
    uint addr = bmap(ip, off / BSIZE);
    अगर (addr == 0)
      अवरोध;
    bp = bread(ip->dev, addr);
    m = min(n - tot, BSIZE - off % BSIZE);
    अगर (either_copyin(bp->तथ्य + (off % BSIZE), user_src, src, m) == -1) {
      // Might have partially updated the block, so we need to log it.
      log_write(bp);
      brelse(bp);
      अवरोध;
    }
    log_write(bp);
    brelse(bp);
  }

  अगर (off > ip->आकार)
    ip->आकार = off;

  // write the i-node back to disk even if the size didn't change
  // because the loop above might have called bmap() and added a new
  // block to ip->addrs[].
  iupdate(ip);

  वापस tot;
}

// Directories

पूर्णांक
namecmp(स्थिर अक्षर *s, स्थिर अक्षर *t)
{
  वापस म_भेदन(s, t, DIRSIZ);
}

// Look for a directory entry in a directory.
// If found, set *poff to byte offset of entry.
काष्ठा inode *
dirlookup(काष्ठा inode *dp, अक्षर *name, uint *poff)
{
  uint off, inum;
  काष्ठा dirent de;

  अगर (dp->type != T_DIR)
    panic("dirlookup not DIR");

  क्रम (off = 0; off < dp->आकार; off += माप(de)) {
    अगर (readi(dp, 0, (uint64)&de, off, माप(de)) != माप(de))
      panic("dirlookup read");
    अगर (de.inum == 0)
      जारी;
    अगर (namecmp(name, de.name) == 0) {
      // entry matches path element
      अगर (poff)
        *poff = off;
      inum = de.inum;
      वापस iget(dp->dev, inum);
    }
  }

  वापस 0;
}

// Write a new directory entry (name, inum) into the directory dp.
// Returns 0 on success, -1 on failure (e.g. out of disk blocks).
पूर्णांक
dirlink(काष्ठा inode *dp, अक्षर *name, uint inum)
{
  पूर्णांक off;
  काष्ठा dirent de;
  काष्ठा inode *ip;

  // Check that name is not present.
  अगर ((ip = dirlookup(dp, name, 0)) != 0) {
    iput(ip);
    वापस -1;
  }

  // Look for an empty dirent.
  क्रम (off = 0; off < dp->आकार; off += माप(de)) {
    अगर (readi(dp, 0, (uint64)&de, off, माप(de)) != माप(de))
      panic("dirlink read");
    अगर (de.inum == 0)
      अवरोध;
  }

  म_नकलन(de.name, name, DIRSIZ);
  de.inum = inum;
  अगर (writei(dp, 0, (uint64)&de, off, माप(de)) != माप(de))
    वापस -1;

  वापस 0;
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
अटल अक्षर *
skipelem(अक्षर *path, अक्षर *name)
{
  अक्षर *s;
  पूर्णांक len;

  जबतक (*path == '/')
    path++;
  अगर (*path == 0)
    वापस 0;
  s = path;
  जबतक (*path != '/' && *path != 0)
    path++;
  len = path - s;
  अगर (len >= DIRSIZ)
    स_हटाओ(name, s, DIRSIZ);
  अन्यथा {
    स_हटाओ(name, s, len);
    name[len] = 0;
  }
  जबतक (*path == '/')
    path++;
  वापस path;
}

// Look up and return the inode for a path name.
// If parent != 0, return the inode for the parent and copy the final
// path element into name, which must have room for DIRSIZ bytes.
// Must be called inside a transaction since it calls iput().
अटल काष्ठा inode *
namex(अक्षर *path, पूर्णांक nameiparent, अक्षर *name)
{
  काष्ठा inode *ip, *अग्र;

  अगर (*path == '/')
    ip = iget(ROOTDEV, ROOTINO);
  अन्यथा
    ip = idup(myproc()->cwd);

  जबतक ((path = skipelem(path, name)) != 0) {
    ilock(ip);
    अगर (ip->type != T_DIR) {
      iunlockput(ip);
      वापस 0;
    }
    अगर (ip->nlink == 0) {
      iunlockput(ip);
      वापस 0;
    }
    अगर (nameiparent && *path == '\0') {
      // Stop one level early.
      iunlock(ip);
      वापस ip;
    }
    अगर ((अग्र = dirlookup(ip, name, 0)) == 0) {
      iunlockput(ip);
      वापस 0;
    }
    iunlockput(ip);
    ip = अग्र;
  }
  अगर (nameiparent) {
    iput(ip);
    वापस 0;
  }
  वापस ip;
}

काष्ठा inode *
namei(अक्षर *path)
{
  अक्षर name[DIRSIZ];
  वापस namex(path, 0, name);
}

काष्ठा inode *
nameiparent(अक्षर *path, अक्षर *name)
{
  वापस namex(path, 1, name);
}
