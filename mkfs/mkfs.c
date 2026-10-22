#شامل <stdio.h>
#شامل <unistd.h>
#شامل <stdlib.h>
#شامل <string.h>
#شامل <fcntl.h>
#شامل <assert.h>

#define stat xv6_stat // avoid clash with host struct stat
#شامل "kernel/types.h"
#شامل "kernel/fs.h"
#شامل "kernel/stat.h"
#شامل "kernel/param.h"

#ifndef static_assert
#define static_assert(a, b)                                                    \
  انجامبده {                                                                         \
    انتخاب (0)                                                                 \
    حالت 0:                                                                    \
    حالت (a):;                                                                 \
  } وقتی (0)
#endif

#define NINODES 200

// Disk layout:
// [ boot block | sb block | log | inode blocks | free bit map | data blocks ]

صحیح nbitmap = FSSIZE / BPB + 1;
صحیح ninodeblocks = NINODES / IPB + 1;
صحیح nlog = LOGBLOCKS + 1; // Header followed by LOGBLOCKS data blocks.
صحیح nmeta;   // Number of meta blocks (boot, sb, nlog, inode, bitmap)
صحیح nblocks; // Number of data blocks

صحیح fsfd;
ساختار superblock sb;
نویسه zeroes[BSIZE];
uint freeinode = 1;
uint freeblock;

تهی balloc(صحیح);
تهی wsect(uint, تهی *);
تهی winode(uint, ساختار dinode *);
تهی rinode(uint inum, ساختار dinode *ip);
تهی rsect(uint sec, تهی *buf);
uint ialloc(ushort type);
تهی iappend(uint inum, تهی *p, صحیح n);
تهی die(ثابت نویسه *);

// convert to riscv byte order
ushort
xshort(ushort x)
{
  ushort y;
  uchar *a = (uchar *)&y;
  a[0] = x;
  a[1] = x >> 8;
  بازگشت y;
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
  بازگشت y;
}

صحیح
اصلی(صحیح argc, نویسه *argv[])
{
  صحیح i, cc, fd;
  uint rootino, inum, off;
  ساختار dirent de;
  نویسه buf[BSIZE];
  ساختار dinode din;

  static_assert(اندازه(صحیح) == 4, "Integers must be 4 bytes!");

  اگر (argc < 2) {
    fprintf(stderr, "Usage: mkfs fs.img files...\n");
    exit(1);
  }

  أكد((BSIZE % اندازه(ساختار dinode)) == 0);
  أكد((BSIZE % اندازه(ساختار dirent)) == 0);

  fsfd = open(argv[1], O_RDWR | O_CREAT | O_TRUNC, 0666);
  اگر (fsfd < 0)
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

  برای (i = 0; i < FSSIZE; i++)
    wsect(i, zeroes);

  memset(buf, 0, اندازه(buf));
  memmove(buf, &sb, اندازه(sb));
  wsect(1, buf);

  rootino = ialloc(T_DIR);
  أكد(rootino == ROOTINO);

  bzero(&de, اندازه(de));
  de.inum = xshort(rootino);
  strcpy(de.name, ".");
  iappend(rootino, &de, اندازه(de));

  bzero(&de, اندازه(de));
  de.inum = xshort(rootino);
  strcpy(de.name, "..");
  iappend(rootino, &de, اندازه(de));

  برای (i = 2; i < argc; i++) {
    // get rid of "user/"
    نویسه *shortname;
    اگر (strncmp(argv[i], "user/", 5) == 0)
      shortname = argv[i] + 5;
    وگرنه
      shortname = argv[i];

    أكد(index(shortname, '/') == 0);

    اگر ((fd = open(argv[i], 0)) < 0)
      die(argv[i]);

    // Skip leading _ in name when writing to file system.
    // The binaries are named _rm, _cat, etc. to keep the
    // build operating system from trying to execute them
    // in place of system binaries like rm and cat.
    اگر (shortname[0] == '_')
      shortname += 1;

    أكد(strlen(shortname) <= DIRSIZ);

    inum = ialloc(T_FILE);

    bzero(&de, اندازه(de));
    de.inum = xshort(inum);
    strncpy(de.name, shortname, DIRSIZ);
    iappend(rootino, &de, اندازه(de));

    وقتی ((cc = اقرأ(fd, buf, اندازه(buf))) > 0)
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

تهی
wsect(uint sec, تهی *buf)
{
  اگر (lseek(fsfd, sec * BSIZE, 0) != sec * BSIZE)
    die("lseek");
  اگر (اكتب(fsfd, buf, BSIZE) != BSIZE)
    die("write");
}

تهی
winode(uint inum, ساختار dinode *ip)
{
  نویسه buf[BSIZE];
  uint bn;
  ساختار dinode *dip;

  bn = IBLOCK(inum, sb);
  rsect(bn, buf);
  dip = ((ساختار dinode *)buf) + (inum % IPB);
  *dip = *ip;
  wsect(bn, buf);
}

تهی
rinode(uint inum, ساختار dinode *ip)
{
  نویسه buf[BSIZE];
  uint bn;
  ساختار dinode *dip;

  bn = IBLOCK(inum, sb);
  rsect(bn, buf);
  dip = ((ساختار dinode *)buf) + (inum % IPB);
  *ip = *dip;
}

تهی
rsect(uint sec, تهی *buf)
{
  اگر (lseek(fsfd, sec * BSIZE, 0) != sec * BSIZE)
    die("lseek");
  اگر (اقرأ(fsfd, buf, BSIZE) != BSIZE)
    die("read");
}

uint
ialloc(ushort type)
{
  uint inum = freeinode++;
  ساختار dinode din;

  bzero(&din, اندازه(din));
  din.type = xshort(type);
  din.nlink = xshort(1);
  din.size = xint(0);
  winode(inum, &din);
  بازگشت inum;
}

تهی
balloc(صحیح used)
{
  uchar buf[BSIZE];
  صحیح i;

  printf("balloc: first %d blocks have been allocated\n", used);
  أكد(used < BPB);
  bzero(buf, BSIZE);
  برای (i = 0; i < used; i++) {
    buf[i / 8] = buf[i / 8] | (0x1 << (i % 8));
  }
  printf("balloc: write bitmap block at sector %d\n", sb.bmapstart);
  wsect(sb.bmapstart, buf);
}

#define min(a, b) ((a) < (b) ? (a) : (b))

تهی
iappend(uint inum, تهی *xp, صحیح n)
{
  نویسه *p = (نویسه *)xp;
  uint fbn, off, n1;
  ساختار dinode din;
  نویسه buf[BSIZE];
  uint indirect[NINDIRECT];
  uint x;

  rinode(inum, &din);
  off = xint(din.size);
  // printf("append inum %d at off %d sz %d\n", inum, off, n);
  وقتی (n > 0) {
    fbn = off / BSIZE;
    أكد(fbn < MAXFILE);
    اگر (fbn < NDIRECT) {
      اگر (xint(din.addrs[fbn]) == 0) {
        din.addrs[fbn] = xint(freeblock++);
      }
      x = xint(din.addrs[fbn]);
    } وگرنه {
      اگر (xint(din.addrs[NDIRECT]) == 0) {
        din.addrs[NDIRECT] = xint(freeblock++);
      }
      rsect(xint(din.addrs[NDIRECT]), (نویسه *)indirect);
      اگر (indirect[fbn - NDIRECT] == 0) {
        indirect[fbn - NDIRECT] = xint(freeblock++);
        wsect(xint(din.addrs[NDIRECT]), (نویسه *)indirect);
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

تهی
die(ثابت نویسه *s)
{
  perror(s);
  exit(1);
}
