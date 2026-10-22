#ଅନ୍ତର୍ଭାଵ <stdio.h>
#ଅନ୍ତର୍ଭାଵ <unistd.h>
#ଅନ୍ତର୍ଭାଵ <stdlib.h>
#ଅନ୍ତର୍ଭାଵ <string.h>
#ଅନ୍ତର୍ଭାଵ <fcntl.h>
#ଅନ୍ତର୍ଭାଵ <assert.h>

#define stat xv6_stat // avoid clash with host struct stat
#ଅନ୍ତର୍ଭାଵ "kernel/types.h"
#ଅନ୍ତର୍ଭାଵ "kernel/fs.h"
#ଅନ୍ତର୍ଭାଵ "kernel/stat.h"
#ଅନ୍ତର୍ଭାଵ "kernel/param.h"

#ifndef static_assert
#define static_assert(a, b)                                                    \
  କର {                                                                         \
    ବାଛ (0)                                                                 \
    ସ୍ଥିତି 0:                                                                    \
    ସ୍ଥିତି (a):;                                                                 \
  } ଯେତେବେଳେ (0)
#endif

#define NINODES 200

// Disk layout:
// [ boot block | sb block | log | inode blocks | free bit map | data blocks ]

ପୂର୍ଣ୍ଣସଂଖ୍ୟା nbitmap = FSSIZE / BPB + 1;
ପୂର୍ଣ୍ଣସଂଖ୍ୟା ninodeblocks = NINODES / IPB + 1;
ପୂର୍ଣ୍ଣସଂଖ୍ୟା nlog = LOGBLOCKS + 1; // Header followed by LOGBLOCKS data blocks.
ପୂର୍ଣ୍ଣସଂଖ୍ୟା nmeta;   // Number of meta blocks (boot, sb, nlog, inode, bitmap)
ପୂର୍ଣ୍ଣସଂଖ୍ୟା nblocks; // Number of data blocks

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fsfd;
ସଂରଚନା superblock sb;
ଅକ୍ଷର zeroes[BSIZE];
uint freeinode = 1;
uint freeblock;

ଶୂନ୍ୟ balloc(ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
ଶୂନ୍ୟ wsect(uint, ଶୂନ୍ୟ *);
ଶୂନ୍ୟ winode(uint, ସଂରଚନା dinode *);
ଶୂନ୍ୟ rinode(uint inum, ସଂରଚନା dinode *ip);
ଶୂନ୍ୟ rsect(uint sec, ଶୂନ୍ୟ *buf);
uint ialloc(ushort type);
ଶୂନ୍ୟ iappend(uint inum, ଶୂନ୍ୟ *p, ପୂର୍ଣ୍ଣସଂଖ୍ୟା n);
ଶୂନ୍ୟ die(ଅଚଳ ଅକ୍ଷର *);

// convert to riscv byte order
ushort
xshort(ushort x)
{
  ushort y;
  uchar *a = (uchar *)&y;
  a[0] = x;
  a[1] = x >> 8;
  ଫେରନ୍ତୁ y;
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
  ଫେରନ୍ତୁ y;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ମୁଖ୍ୟ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା argc, ଅକ୍ଷର *argv[])
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା i, cc, fd;
  uint rootino, inum, off;
  ସଂରଚନା dirent de;
  ଅକ୍ଷର buf[BSIZE];
  ସଂରଚନା dinode din;

  static_assert(ଆକାର(ପୂର୍ଣ୍ଣସଂଖ୍ୟା) == 4, "Integers must be 4 bytes!");

  ଯଦି (argc < 2) {
    fprintf(stderr, "Usage: mkfs fs.img files...\n");
    exit(1);
  }

  ପ୍ରତିଜ୍ଞା((BSIZE % ଆକାର(ସଂରଚନା dinode)) == 0);
  ପ୍ରତିଜ୍ଞା((BSIZE % ଆକାର(ସଂରଚନା dirent)) == 0);

  fsfd = open(argv[1], O_RDWR | O_CREAT | O_TRUNC, 0666);
  ଯଦି (fsfd < 0)
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

  ପାଇଁ (i = 0; i < FSSIZE; i++)
    wsect(i, zeroes);

  memset(buf, 0, ଆକାର(buf));
  memmove(buf, &sb, ଆକାର(sb));
  wsect(1, buf);

  rootino = ialloc(T_DIR);
  ପ୍ରତିଜ୍ଞା(rootino == ROOTINO);

  bzero(&de, ଆକାର(de));
  de.inum = xshort(rootino);
  strcpy(de.name, ".");
  iappend(rootino, &de, ଆକାର(de));

  bzero(&de, ଆକାର(de));
  de.inum = xshort(rootino);
  strcpy(de.name, "..");
  iappend(rootino, &de, ଆକାର(de));

  ପାଇଁ (i = 2; i < argc; i++) {
    // get rid of "user/"
    ଅକ୍ଷର *shortname;
    ଯଦି (strncmp(argv[i], "user/", 5) == 0)
      shortname = argv[i] + 5;
    ଅନ୍ୟଥା
      shortname = argv[i];

    ପ୍ରତିଜ୍ଞା(index(shortname, '/') == 0);

    ଯଦି ((fd = open(argv[i], 0)) < 0)
      die(argv[i]);

    // Skip leading _ in name when writing to file system.
    // The binaries are named _rm, _cat, etc. to keep the
    // build operating system from trying to execute them
    // in place of system binaries like rm and cat.
    ଯଦି (shortname[0] == '_')
      shortname += 1;

    ପ୍ରତିଜ୍ଞା(strlen(shortname) <= DIRSIZ);

    inum = ialloc(T_FILE);

    bzero(&de, ଆକାର(de));
    de.inum = xshort(inum);
    strncpy(de.name, shortname, DIRSIZ);
    iappend(rootino, &de, ଆକାର(de));

    ଯେତେବେଳେ ((cc = ପଠ(fd, buf, ଆକାର(buf))) > 0)
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

ଶୂନ୍ୟ
wsect(uint sec, ଶୂନ୍ୟ *buf)
{
  ଯଦି (lseek(fsfd, sec * BSIZE, 0) != sec * BSIZE)
    die("lseek");
  ଯଦି (ଲିଖ(fsfd, buf, BSIZE) != BSIZE)
    die("write");
}

ଶୂନ୍ୟ
winode(uint inum, ସଂରଚନା dinode *ip)
{
  ଅକ୍ଷର buf[BSIZE];
  uint bn;
  ସଂରଚନା dinode *dip;

  bn = IBLOCK(inum, sb);
  rsect(bn, buf);
  dip = ((ସଂରଚନା dinode *)buf) + (inum % IPB);
  *dip = *ip;
  wsect(bn, buf);
}

ଶୂନ୍ୟ
rinode(uint inum, ସଂରଚନା dinode *ip)
{
  ଅକ୍ଷର buf[BSIZE];
  uint bn;
  ସଂରଚନା dinode *dip;

  bn = IBLOCK(inum, sb);
  rsect(bn, buf);
  dip = ((ସଂରଚନା dinode *)buf) + (inum % IPB);
  *ip = *dip;
}

ଶୂନ୍ୟ
rsect(uint sec, ଶୂନ୍ୟ *buf)
{
  ଯଦି (lseek(fsfd, sec * BSIZE, 0) != sec * BSIZE)
    die("lseek");
  ଯଦି (ପଠ(fsfd, buf, BSIZE) != BSIZE)
    die("read");
}

uint
ialloc(ushort type)
{
  uint inum = freeinode++;
  ସଂରଚନା dinode din;

  bzero(&din, ଆକାର(din));
  din.type = xshort(type);
  din.nlink = xshort(1);
  din.size = xint(0);
  winode(inum, &din);
  ଫେରନ୍ତୁ inum;
}

ଶୂନ୍ୟ
balloc(ପୂର୍ଣ୍ଣସଂଖ୍ୟା used)
{
  uchar buf[BSIZE];
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

  printf("balloc: first %d blocks have been allocated\n", used);
  ପ୍ରତିଜ୍ଞା(used < BPB);
  bzero(buf, BSIZE);
  ପାଇଁ (i = 0; i < used; i++) {
    buf[i / 8] = buf[i / 8] | (0x1 << (i % 8));
  }
  printf("balloc: write bitmap block at sector %d\n", sb.bmapstart);
  wsect(sb.bmapstart, buf);
}

#define min(a, b) ((a) < (b) ? (a) : (b))

ଶୂନ୍ୟ
iappend(uint inum, ଶୂନ୍ୟ *xp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା n)
{
  ଅକ୍ଷର *p = (ଅକ୍ଷର *)xp;
  uint fbn, off, n1;
  ସଂରଚନା dinode din;
  ଅକ୍ଷର buf[BSIZE];
  uint indirect[NINDIRECT];
  uint x;

  rinode(inum, &din);
  off = xint(din.size);
  // printf("append inum %d at off %d sz %d\n", inum, off, n);
  ଯେତେବେଳେ (n > 0) {
    fbn = off / BSIZE;
    ପ୍ରତିଜ୍ଞା(fbn < MAXFILE);
    ଯଦି (fbn < NDIRECT) {
      ଯଦି (xint(din.addrs[fbn]) == 0) {
        din.addrs[fbn] = xint(freeblock++);
      }
      x = xint(din.addrs[fbn]);
    } ଅନ୍ୟଥା {
      ଯଦି (xint(din.addrs[NDIRECT]) == 0) {
        din.addrs[NDIRECT] = xint(freeblock++);
      }
      rsect(xint(din.addrs[NDIRECT]), (ଅକ୍ଷର *)indirect);
      ଯଦି (indirect[fbn - NDIRECT] == 0) {
        indirect[fbn - NDIRECT] = xint(freeblock++);
        wsect(xint(din.addrs[NDIRECT]), (ଅକ୍ଷର *)indirect);
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

ଶୂନ୍ୟ
die(ଅଚଳ ଅକ୍ଷର *s)
{
  perror(s);
  exit(1);
}
