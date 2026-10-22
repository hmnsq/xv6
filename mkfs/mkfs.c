#অন্তর্ভাব <stdio.h>
#অন্তর্ভাব <unistd.h>
#অন্তর্ভাব <stdlib.h>
#অন্তর্ভাব <string.h>
#অন্তর্ভাব <fcntl.h>
#অন্তর্ভাব <assert.h>

#define stat xv6_stat // avoid clash with host struct stat
#অন্তর্ভাব "kernel/types.h"
#অন্তর্ভাব "kernel/fs.h"
#অন্তর্ভাব "kernel/stat.h"
#অন্তর্ভাব "kernel/param.h"

#ifndef static_assert
#define static_assert(a, b)                                                    \
  করো {                                                                         \
    নির্বাচন (0)                                                                 \
    ক্ষেত্র 0:                                                                    \
    ক্ষেত্র (a):;                                                                 \
  } যখন (0)
#endif

#define NINODES 200

// Disk layout:
// [ boot block | sb block | log | inode blocks | free bit map | data blocks ]

পূর্ণসংখ্যা nbitmap = FSSIZE / BPB + 1;
পূর্ণসংখ্যা ninodeblocks = NINODES / IPB + 1;
পূর্ণসংখ্যা nlog = LOGBLOCKS + 1; // Header followed by LOGBLOCKS data blocks.
পূর্ণসংখ্যা nmeta;   // Number of meta blocks (boot, sb, nlog, inode, bitmap)
পূর্ণসংখ্যা nblocks; // Number of data blocks

পূর্ণসংখ্যা fsfd;
কাঠামো superblock sb;
অক্ষর zeroes[BSIZE];
uint freeinode = 1;
uint freeblock;

শূন্য balloc(পূর্ণসংখ্যা);
শূন্য wsect(uint, শূন্য *);
শূন্য winode(uint, কাঠামো dinode *);
শূন্য rinode(uint inum, কাঠামো dinode *ip);
শূন্য rsect(uint sec, শূন্য *buf);
uint ialloc(ushort type);
শূন্য iappend(uint inum, শূন্য *p, পূর্ণসংখ্যা n);
শূন্য die(ধ্রুবক অক্ষর *);

// convert to riscv byte order
ushort
xshort(ushort x)
{
  ushort y;
  uchar *a = (uchar *)&y;
  a[0] = x;
  a[1] = x >> 8;
  ফেরত y;
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
  ফেরত y;
}

পূর্ণসংখ্যা
প্রধান(পূর্ণসংখ্যা argc, অক্ষর *argv[])
{
  পূর্ণসংখ্যা i, cc, fd;
  uint rootino, inum, off;
  কাঠামো dirent de;
  অক্ষর buf[BSIZE];
  কাঠামো dinode din;

  static_assert(আকার(পূর্ণসংখ্যা) == 4, "Integers must be 4 bytes!");

  যদি (argc < 2) {
    fprintf(stderr, "Usage: mkfs fs.img files...\n");
    exit(1);
  }

  প্রতিজ্ঞা((BSIZE % আকার(কাঠামো dinode)) == 0);
  প্রতিজ্ঞা((BSIZE % আকার(কাঠামো dirent)) == 0);

  fsfd = open(argv[1], O_RDWR | O_CREAT | O_TRUNC, 0666);
  যদি (fsfd < 0)
    die(argv[1]);

  // 1 fs block = 1 disk sector
  nmeta = 2 + nlog + ninodeblocks + nbitmap;
  nblocks = FSSIZE - nmeta;

  sb.magic = FSMAGIC;
  sb.আকার_2 = xint(FSSIZE);
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

  জন্য (i = 0; i < FSSIZE; i++)
    wsect(i, zeroes);

  memset(buf, 0, আকার(buf));
  memmove(buf, &sb, আকার(sb));
  wsect(1, buf);

  rootino = ialloc(T_DIR);
  প্রতিজ্ঞা(rootino == ROOTINO);

  bzero(&de, আকার(de));
  de.inum = xshort(rootino);
  strcpy(de.name, ".");
  iappend(rootino, &de, আকার(de));

  bzero(&de, আকার(de));
  de.inum = xshort(rootino);
  strcpy(de.name, "..");
  iappend(rootino, &de, আকার(de));

  জন্য (i = 2; i < argc; i++) {
    // get rid of "user/"
    অক্ষর *shortname;
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

    bzero(&de, আকার(de));
    de.inum = xshort(inum);
    strncpy(de.name, shortname, DIRSIZ);
    iappend(rootino, &de, আকার(de));

    যখন ((cc = পঠ(fd, buf, আকার(buf))) > 0)
      iappend(inum, buf, cc);

    close(fd);
  }

  // fix size of root inode dir
  rinode(rootino, &din);
  off = xint(din.আকার_2);
  off = ((off / BSIZE) + 1) * BSIZE;
  din.আকার_2 = xint(off);
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
winode(uint inum, কাঠামো dinode *ip)
{
  অক্ষর buf[BSIZE];
  uint bn;
  কাঠামো dinode *dip;

  bn = IBLOCK(inum, sb);
  rsect(bn, buf);
  dip = ((কাঠামো dinode *)buf) + (inum % IPB);
  *dip = *ip;
  wsect(bn, buf);
}

শূন্য
rinode(uint inum, কাঠামো dinode *ip)
{
  অক্ষর buf[BSIZE];
  uint bn;
  কাঠামো dinode *dip;

  bn = IBLOCK(inum, sb);
  rsect(bn, buf);
  dip = ((কাঠামো dinode *)buf) + (inum % IPB);
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
  কাঠামো dinode din;

  bzero(&din, আকার(din));
  din.type = xshort(type);
  din.nlink = xshort(1);
  din.আকার_2 = xint(0);
  winode(inum, &din);
  ফেরত inum;
}

শূন্য
balloc(পূর্ণসংখ্যা used)
{
  uchar buf[BSIZE];
  পূর্ণসংখ্যা i;

  printf("balloc: first %d blocks have been allocated\n", used);
  প্রতিজ্ঞা(used < BPB);
  bzero(buf, BSIZE);
  জন্য (i = 0; i < used; i++) {
    buf[i / 8] = buf[i / 8] | (0x1 << (i % 8));
  }
  printf("balloc: write bitmap block at sector %d\n", sb.bmapstart);
  wsect(sb.bmapstart, buf);
}

#define min(a, b) ((a) < (b) ? (a) : (b))

শূন্য
iappend(uint inum, শূন্য *xp, পূর্ণসংখ্যা n)
{
  অক্ষর *p = (অক্ষর *)xp;
  uint fbn, off, n1;
  কাঠামো dinode din;
  অক্ষর buf[BSIZE];
  uint indirect[NINDIRECT];
  uint x;

  rinode(inum, &din);
  off = xint(din.আকার_2);
  // printf("append inum %d at off %d sz %d\n", inum, off, n);
  যখন (n > 0) {
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
      rsect(xint(din.addrs[NDIRECT]), (অক্ষর *)indirect);
      যদি (indirect[fbn - NDIRECT] == 0) {
        indirect[fbn - NDIRECT] = xint(freeblock++);
        wsect(xint(din.addrs[NDIRECT]), (অক্ষর *)indirect);
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
  din.আকার_2 = xint(off);
  winode(inum, &din);
}

শূন্য
die(ধ্রুবক অক্ষর *s)
{
  perror(s);
  exit(1);
}
