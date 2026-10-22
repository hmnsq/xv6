#ಅನ್ತರ್ಭಾವ <stdio.h>
#ಅನ್ತರ್ಭಾವ <unistd.h>
#ಅನ್ತರ್ಭಾವ <stdlib.h>
#ಅನ್ತರ್ಭಾವ <string.h>
#ಅನ್ತರ್ಭಾವ <fcntl.h>
#ಅನ್ತರ್ಭಾವ <assert.h>

#define stat xv6_stat // avoid clash with host struct stat
#ಅನ್ತರ್ಭಾವ "kernel/types.h"
#ಅನ್ತರ್ಭಾವ "kernel/fs.h"
#ಅನ್ತರ್ಭಾವ "kernel/stat.h"
#ಅನ್ತರ್ಭಾವ "kernel/param.h"

#ifndef static_assert
#define static_assert(a, b)                                                    \
  ಕುರು {                                                                         \
    ವಿಕಲ್ಪ (0)                                                                 \
    ಪಕ್ಷ 0:                                                                    \
    ಪಕ್ಷ (a):;                                                                 \
  } ಯಾವತ್ (0)
#endif

#define NINODES 200

// Disk layout:
// [ boot block | sb block | log | inode blocks | free bit map | data blocks ]

ಪೂರ್ಣಾಙ್ಕ nbitmap = FSSIZE / BPB + 1;
ಪೂರ್ಣಾಙ್ಕ ninodeblocks = NINODES / IPB + 1;
ಪೂರ್ಣಾಙ್ಕ nlog = LOGBLOCKS + 1; // Header followed by LOGBLOCKS data blocks.
ಪೂರ್ಣಾಙ್ಕ nmeta;   // Number of meta blocks (boot, sb, nlog, inode, bitmap)
ಪೂರ್ಣಾಙ್ಕ nblocks; // Number of data blocks

ಪೂರ್ಣಾಙ್ಕ fsfd;
ಸಂರಚನಾ superblock sb;
ವರ್ಣ zeroes[BSIZE];
uint freeinode = 1;
uint freeblock;

ಶೂನ್ಯ balloc(ಪೂರ್ಣಾಙ್ಕ);
ಶೂನ್ಯ wsect(uint, ಶೂನ್ಯ *);
ಶೂನ್ಯ winode(uint, ಸಂರಚನಾ dinode *);
ಶೂನ್ಯ rinode(uint inum, ಸಂರಚನಾ dinode *ip);
ಶೂನ್ಯ rsect(uint sec, ಶೂನ್ಯ *buf);
uint ialloc(ushort type);
ಶೂನ್ಯ iappend(uint inum, ಶೂನ್ಯ *p, ಪೂರ್ಣಾಙ್ಕ n);
ಶೂನ್ಯ die(ಧ್ರುವ ವರ್ಣ *);

// convert to riscv byte order
ushort
xshort(ushort x)
{
  ushort y;
  uchar *a = (uchar *)&y;
  a[0] = x;
  a[1] = x >> 8;
  ಪ್ರತ್ಯಾಗಮ y;
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
  ಪ್ರತ್ಯಾಗಮ y;
}

ಪೂರ್ಣಾಙ್ಕ
main(ಪೂರ್ಣಾಙ್ಕ argc, ವರ್ಣ *argv[])
{
  ಪೂರ್ಣಾಙ್ಕ i, cc, fd;
  uint rootino, inum, off;
  ಸಂರಚನಾ dirent de;
  ವರ್ಣ buf[BSIZE];
  ಸಂರಚನಾ dinode din;

  static_assert(ಪರಿಮಾಣ(ಪೂರ್ಣಾಙ್ಕ) == 4, "Integers must be 4 bytes!");

  ಯದಿ (argc < 2) {
    fprintf(stderr, "Usage: mkfs fs.img files...\n");
    exit(1);
  }

  ಪ್ರತಿಜ್ಞಾ((BSIZE % ಪರಿಮಾಣ(ಸಂರಚನಾ dinode)) == 0);
  ಪ್ರತಿಜ್ಞಾ((BSIZE % ಪರಿಮಾಣ(ಸಂರಚನಾ dirent)) == 0);

  fsfd = open(argv[1], O_RDWR | O_CREAT | O_TRUNC, 0666);
  ಯದಿ (fsfd < 0)
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

  ಪ್ರತಿ (i = 0; i < FSSIZE; i++)
    wsect(i, zeroes);

  memset(buf, 0, ಪರಿಮಾಣ(buf));
  memmove(buf, &sb, ಪರಿಮಾಣ(sb));
  wsect(1, buf);

  rootino = ialloc(T_DIR);
  ಪ್ರತಿಜ್ಞಾ(rootino == ROOTINO);

  bzero(&de, ಪರಿಮಾಣ(de));
  de.inum = xshort(rootino);
  strcpy(de.name, ".");
  iappend(rootino, &de, ಪರಿಮಾಣ(de));

  bzero(&de, ಪರಿಮಾಣ(de));
  de.inum = xshort(rootino);
  strcpy(de.name, "..");
  iappend(rootino, &de, ಪರಿಮಾಣ(de));

  ಪ್ರತಿ (i = 2; i < argc; i++) {
    // get rid of "user/"
    ವರ್ಣ *shortname;
    ಯದಿ (strncmp(argv[i], "user/", 5) == 0)
      shortname = argv[i] + 5;
    ಅನ್ಯಥಾ
      shortname = argv[i];

    ಪ್ರತಿಜ್ಞಾ(index(shortname, '/') == 0);

    ಯದಿ ((fd = open(argv[i], 0)) < 0)
      die(argv[i]);

    // Skip leading _ in name when writing to file system.
    // The binaries are named _rm, _cat, etc. to keep the
    // build operating system from trying to execute them
    // in place of system binaries like rm and cat.
    ಯದಿ (shortname[0] == '_')
      shortname += 1;

    ಪ್ರತಿಜ್ಞಾ(strlen(shortname) <= DIRSIZ);

    inum = ialloc(T_FILE);

    bzero(&de, ಪರಿಮಾಣ(de));
    de.inum = xshort(inum);
    strncpy(de.name, shortname, DIRSIZ);
    iappend(rootino, &de, ಪರಿಮಾಣ(de));

    ಯಾವತ್ ((cc = ಪಠ(fd, buf, ಪರಿಮಾಣ(buf))) > 0)
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

ಶೂನ್ಯ
wsect(uint sec, ಶೂನ್ಯ *buf)
{
  ಯದಿ (lseek(fsfd, sec * BSIZE, 0) != sec * BSIZE)
    die("lseek");
  ಯದಿ (ಲಿಖ(fsfd, buf, BSIZE) != BSIZE)
    die("write");
}

ಶೂನ್ಯ
winode(uint inum, ಸಂರಚನಾ dinode *ip)
{
  ವರ್ಣ buf[BSIZE];
  uint bn;
  ಸಂರಚನಾ dinode *dip;

  bn = IBLOCK(inum, sb);
  rsect(bn, buf);
  dip = ((ಸಂರಚನಾ dinode *)buf) + (inum % IPB);
  *dip = *ip;
  wsect(bn, buf);
}

ಶೂನ್ಯ
rinode(uint inum, ಸಂರಚನಾ dinode *ip)
{
  ವರ್ಣ buf[BSIZE];
  uint bn;
  ಸಂರಚನಾ dinode *dip;

  bn = IBLOCK(inum, sb);
  rsect(bn, buf);
  dip = ((ಸಂರಚನಾ dinode *)buf) + (inum % IPB);
  *ip = *dip;
}

ಶೂನ್ಯ
rsect(uint sec, ಶೂನ್ಯ *buf)
{
  ಯದಿ (lseek(fsfd, sec * BSIZE, 0) != sec * BSIZE)
    die("lseek");
  ಯದಿ (ಪಠ(fsfd, buf, BSIZE) != BSIZE)
    die("read");
}

uint
ialloc(ushort type)
{
  uint inum = freeinode++;
  ಸಂರಚನಾ dinode din;

  bzero(&din, ಪರಿಮಾಣ(din));
  din.type = xshort(type);
  din.nlink = xshort(1);
  din.size = xint(0);
  winode(inum, &din);
  ಪ್ರತ್ಯಾಗಮ inum;
}

ಶೂನ್ಯ
balloc(ಪೂರ್ಣಾಙ್ಕ used)
{
  uchar buf[BSIZE];
  ಪೂರ್ಣಾಙ್ಕ i;

  printf("balloc: first %d blocks have been allocated\n", used);
  ಪ್ರತಿಜ್ಞಾ(used < BPB);
  bzero(buf, BSIZE);
  ಪ್ರತಿ (i = 0; i < used; i++) {
    buf[i / 8] = buf[i / 8] | (0x1 << (i % 8));
  }
  printf("balloc: write bitmap block at sector %d\n", sb.bmapstart);
  wsect(sb.bmapstart, buf);
}

#define min(a, b) ((a) < (b) ? (a) : (b))

ಶೂನ್ಯ
iappend(uint inum, ಶೂನ್ಯ *xp, ಪೂರ್ಣಾಙ್ಕ n)
{
  ವರ್ಣ *p = (ವರ್ಣ *)xp;
  uint fbn, off, n1;
  ಸಂರಚನಾ dinode din;
  ವರ್ಣ buf[BSIZE];
  uint indirect[NINDIRECT];
  uint x;

  rinode(inum, &din);
  off = xint(din.size);
  // printf("append inum %d at off %d sz %d\n", inum, off, n);
  ಯಾವತ್ (n > 0) {
    fbn = off / BSIZE;
    ಪ್ರತಿಜ್ಞಾ(fbn < MAXFILE);
    ಯದಿ (fbn < NDIRECT) {
      ಯದಿ (xint(din.addrs[fbn]) == 0) {
        din.addrs[fbn] = xint(freeblock++);
      }
      x = xint(din.addrs[fbn]);
    } ಅನ್ಯಥಾ {
      ಯದಿ (xint(din.addrs[NDIRECT]) == 0) {
        din.addrs[NDIRECT] = xint(freeblock++);
      }
      rsect(xint(din.addrs[NDIRECT]), (ವರ್ಣ *)indirect);
      ಯದಿ (indirect[fbn - NDIRECT] == 0) {
        indirect[fbn - NDIRECT] = xint(freeblock++);
        wsect(xint(din.addrs[NDIRECT]), (ವರ್ಣ *)indirect);
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

ಶೂನ್ಯ
die(ಧ್ರುವ ವರ್ಣ *s)
{
  perror(s);
  exit(1);
}
