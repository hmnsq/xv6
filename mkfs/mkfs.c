#অন্তর্ভাৱ <stdio.h>
#অন্তর্ভাৱ <unistd.h>
#অন্তর্ভাৱ <stdlib.h>
#অন্তর্ভাৱ <string.h>
#অন্তর্ভাৱ <fcntl.h>
#অন্তর্ভাৱ <assert.h>

#define stat xv6_stat // avoid clash with host struct stat
#অন্তর্ভাৱ "kernel/types.h"
#অন্তর্ভাৱ "kernel/fs.h"
#অন্তর্ভাৱ "kernel/stat.h"
#অন্তর্ভাৱ "kernel/param.h"

#ifndef static_assert
#define static_assert(a, b)                                                    \
  কৰা {                                                                         \
    ৱিকল্প (0)                                                                 \
    পক্ষ 0:                                                                    \
    পক্ষ (a):;                                                                 \
  } যেতিয়ালৈ (0)
#endif

#define NINODES 200

// Disk layout:
// [ boot block | sb block | log | inode blocks | free bit map | data blocks ]

পূর্ণাঙ্ক nbitmap = FSSIZE / BPB + 1;
পূর্ণাঙ্ক ninodeblocks = NINODES / IPB + 1;
পূর্ণাঙ্ক nlog = LOGBLOCKS + 1; // Header followed by LOGBLOCKS data blocks.
পূর্ণাঙ্ক nmeta;   // Number of meta blocks (boot, sb, nlog, inode, bitmap)
পূর্ণাঙ্ক nblocks; // Number of data blocks

পূর্ণাঙ্ক fsfd;
সংরচনা superblock sb;
ৱর্ণ zeroes[BSIZE];
uint freeinode = 1;
uint freeblock;

শূন্য balloc(পূর্ণাঙ্ক);
শূন্য wsect(uint, শূন্য *);
শূন্য winode(uint, সংরচনা dinode *);
শূন্য rinode(uint inum, সংরচনা dinode *ip);
শূন্য rsect(uint sec, শূন্য *buf);
uint ialloc(ushort type);
শূন্য iappend(uint inum, শূন্য *p, পূর্ণাঙ্ক n);
শূন্য die(ধ্রুৱ ৱর্ণ *);

// convert to riscv byte order
ushort
xshort(ushort x)
{
  ushort y;
  uchar *a = (uchar *)&y;
  a[0] = x;
  a[1] = x >> 8;
  ঘূৰাই y;
}

uint
xint(uint x)
{
  uint y;
  uchar *a = (uchar *)&y;
  a[0] = x;
  a[1] = x >> 8;
  a[2] = x >> 16;
  a[3] = x >> 24;
  ঘূৰাই y;
}

পূর্ণাঙ্ক
main(পূর্ণাঙ্ক argc, ৱর্ণ *argv[])
{
  পূর্ণাঙ্ক i, cc, fd;
  uint rootino, inum, off;
  সংরচনা dirent de;
  ৱর্ণ buf[BSIZE];
  সংরচনা dinode din;

  static_assert(পরিমাণ(পূর্ণাঙ্ক) == 4, "Integers must be 4 bytes!");

  যদি (argc < 2) {
    fprintf(stderr, "Usage: mkfs fs.img files...\n");
    exit(1);
  }

  প্রতিজ্ঞা((BSIZE % পরিমাণ(সংরচনা dinode)) == 0);
  প্রতিজ্ঞা((BSIZE % পরিমাণ(সংরচনা dirent)) == 0);

  fsfd = open(argv[1], O_RDWR | O_CREAT | O_TRUNC, 0666);
  যদি (fsfd < 0)
    die(argv[1]);

  // 1 fs block = 1 disk sector
  nmeta = 2 + nlog + ninodeblocks + nbitmap;
  nblocks = FSSIZE - nmeta;

  sb.magic = FSMAGIC;
  sb.size = xint(FSSIZE);
  sb.nblocks = xint(nblocks);
  sb.ninodes = xint(NINODES);
  sb.nlog = xint(nlog);
  sb.logstart = xint(2);
  sb.inodestart = xint(2 + nlog);
  sb.bmapstart = xint(2 + nlog + ninodeblocks);

  printf(
    "nmeta %d (boot, super, log blocks %u, inode blocks %u, bitmap blocks %u) blocks %d total %d\n",
    nmeta, nlog, ninodeblocks, nbitmap, nblocks, FSSIZE);

  freeblock = nmeta; // the first free block that we can allocate

  বাবে (i = 0; i < FSSIZE; i++)
    wsect(i, zeroes);

  memset(buf, 0, পরিমাণ(buf));
  memmove(buf, &sb, পরিমাণ(sb));
  wsect(1, buf);

  rootino = ialloc(T_DIR);
  প্রতিজ্ঞা(rootino == ROOTINO);

  bzero(&de, পরিমাণ(de));
  de.inum = xshort(rootino);
  strcpy(de.name, ".");
  iappend(rootino, &de, পরিমাণ(de));

  bzero(&de, পরিমাণ(de));
  de.inum = xshort(rootino);
  strcpy(de.name, "..");
  iappend(rootino, &de, পরিমাণ(de));

  বাবে (i = 2; i < argc; i++) {
    // get rid of "user/"
    ৱর্ণ *shortname;
    যদি (strncmp(argv[i], "user/", 5) == 0)
      shortname = argv[i] + 5;
    অন্যথা
      shortname = argv[i];

    প্রতিজ্ঞা(index(shortname, '/') == 0);

    যদি ((fd = open(argv[i], 0)) < 0)
      die(argv[i]);

    // Skip leading _ in name when writing to file system.
    // The binaries are named _rm, _cat, etc. to keep the
    // build operating system from trying to execute them
    // in place of system binaries like rm and cat.
    যদি (shortname[0] == '_')
      shortname += 1;

    প্রতিজ্ঞা(strlen(shortname) <= DIRSIZ);

    inum = ialloc(T_FILE);

    bzero(&de, পরিমাণ(de));
    de.inum = xshort(inum);
    strncpy(de.name, shortname, DIRSIZ);
    iappend(rootino, &de, পরিমাণ(de));

    যেতিয়ালৈ ((cc = পঠ(fd, buf, পরিমাণ(buf))) > 0)
      iappend(inum, buf, cc);

    close(fd);
  }

  // fix size of root inode dir
  rinode(rootino, &din);
  off = xint(din.size);
  off = ((off / BSIZE) + 1) * BSIZE;
  din.size = xint(off);
  winode(rootino, &din);

  balloc(freeblock);

  exit(0);
}

শূন্য
wsect(uint sec, শূন্য *buf)
{
  যদি (lseek(fsfd, sec * BSIZE, 0) != sec * BSIZE)
    die("lseek");
  যদি (লিখ(fsfd, buf, BSIZE) != BSIZE)
    die("write");
}

শূন্য
winode(uint inum, সংরচনা dinode *ip)
{
  ৱর্ণ buf[BSIZE];
  uint bn;
  সংরচনা dinode *dip;

  bn = IBLOCK(inum, sb);
  rsect(bn, buf);
  dip = ((সংরচনা dinode *)buf) + (inum % IPB);
  *dip = *ip;
  wsect(bn, buf);
}

শূন্য
rinode(uint inum, সংরচনা dinode *ip)
{
  ৱর্ণ buf[BSIZE];
  uint bn;
  সংরচনা dinode *dip;

  bn = IBLOCK(inum, sb);
  rsect(bn, buf);
  dip = ((সংরচনা dinode *)buf) + (inum % IPB);
  *ip = *dip;
}

শূন্য
rsect(uint sec, শূন্য *buf)
{
  যদি (lseek(fsfd, sec * BSIZE, 0) != sec * BSIZE)
    die("lseek");
  যদি (পঠ(fsfd, buf, BSIZE) != BSIZE)
    die("read");
}

uint
ialloc(ushort type)
{
  uint inum = freeinode++;
  সংরচনা dinode din;

  bzero(&din, পরিমাণ(din));
  din.type = xshort(type);
  din.nlink = xshort(1);
  din.size = xint(0);
  winode(inum, &din);
  ঘূৰাই inum;
}

শূন্য
balloc(পূর্ণাঙ্ক used)
{
  uchar buf[BSIZE];
  পূর্ণাঙ্ক i;

  printf("balloc: first %d blocks have been allocated\n", used);
  প্রতিজ্ঞা(used < BPB);
  bzero(buf, BSIZE);
  বাবে (i = 0; i < used; i++) {
    buf[i / 8] = buf[i / 8] | (0x1 << (i % 8));
  }
  printf("balloc: write bitmap block at sector %d\n", sb.bmapstart);
  wsect(sb.bmapstart, buf);
}

#define min(a, b) ((a) < (b) ? (a) : (b))

শূন্য
iappend(uint inum, শূন্য *xp, পূর্ণাঙ্ক n)
{
  ৱর্ণ *p = (ৱর্ণ *)xp;
  uint fbn, off, n1;
  সংরচনা dinode din;
  ৱর্ণ buf[BSIZE];
  uint indirect[NINDIRECT];
  uint x;

  rinode(inum, &din);
  off = xint(din.size);
  // printf("append inum %d at off %d sz %d\n", inum, off, n);
  যেতিয়ালৈ (n > 0) {
    fbn = off / BSIZE;
    প্রতিজ্ঞা(fbn < MAXFILE);
    যদি (fbn < NDIRECT) {
      যদি (xint(din.addrs[fbn]) == 0) {
        din.addrs[fbn] = xint(freeblock++);
      }
      x = xint(din.addrs[fbn]);
    } অন্যথা {
      যদি (xint(din.addrs[NDIRECT]) == 0) {
        din.addrs[NDIRECT] = xint(freeblock++);
      }
      rsect(xint(din.addrs[NDIRECT]), (ৱর্ণ *)indirect);
      যদি (indirect[fbn - NDIRECT] == 0) {
        indirect[fbn - NDIRECT] = xint(freeblock++);
        wsect(xint(din.addrs[NDIRECT]), (ৱর্ণ *)indirect);
      }
      x = xint(indirect[fbn - NDIRECT]);
    }
    n1 = min(n, (fbn + 1) * BSIZE - off);
    rsect(x, buf);
    bcopy(p, buf + off - (fbn * BSIZE), n1);
    wsect(x, buf);
    n -= n1;
    off += n1;
    p += n1;
  }
  din.size = xint(off);
  winode(inum, &din);
}

শূন্য
die(ধ্রুৱ ৱর্ণ *s)
{
  perror(s);
  exit(1);
}
