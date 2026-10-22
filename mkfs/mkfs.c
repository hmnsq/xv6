#ಅನ್ತರ್ಭಾವ <stdio.h>
#ಅನ್ತರ್ಭಾವ <unistd.h>
#ಅನ್ತರ್ಭಾವ <stdlib.h>
#ಅನ್ತರ್ಭಾವ <string.h>
#ಅನ್ತರ್ಭಾವ <fcntl.h>
#ಅನ್ತರ್ಭಾವ <assert.h>

#ವ್ಯಾಖ್ಯಾನಿಸು stat xv6_stat // avoid clash with host struct stat
#ಅನ್ತರ್ಭಾವ "kernel/types.h"
#ಅನ್ತರ್ಭಾವ "kernel/fs.h"
#ಅನ್ತರ್ಭಾವ "kernel/stat.h"
#ಅನ್ತರ್ಭಾವ "kernel/param.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ static_assert
#ವ್ಯಾಖ್ಯಾನಿಸು static_assert(a, b)                                                    \
  ಮಾಡು {                                                                         \
    ಆಯ್ಕೆ (0)                                                                 \
    ಸ್ಥಿತಿ 0:                                                                    \
    ಸ್ಥಿತಿ (a):;                                                                 \
  } ಆಗ (0)
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿಸು NINODES 200

// Disk layout:
// [ boot block | sb block | log | inode blocks | free bit map | data blocks ]

ಪೂರ್ಣಾಂಕ nbitmap = FSSIZE / BPB + 1;
ಪೂರ್ಣಾಂಕ ninodeblocks = NINODES / IPB + 1;
ಪೂರ್ಣಾಂಕ nlog = LOGBLOCKS + 1; // Header followed by LOGBLOCKS data blocks.
ಪೂರ್ಣಾಂಕ nmeta;   // Number of meta blocks (boot, sb, nlog, inode, bitmap)
ಪೂರ್ಣಾಂಕ nblocks; // Number of data blocks

ಪೂರ್ಣಾಂಕ fsfd;
ರಚನೆ superblock sb;
ಅಕ್ಷರ zeroes[BSIZE];
uint freeinode = 1;
uint freeblock;

ಶೂನ್ಯ balloc(ಪೂರ್ಣಾಂಕ);
ಶೂನ್ಯ wsect(uint, ಶೂನ್ಯ *);
ಶೂನ್ಯ winode(uint, ರಚನೆ dinode *);
ಶೂನ್ಯ rinode(uint inum, ರಚನೆ dinode *ip);
ಶೂನ್ಯ rsect(uint sec, ಶೂನ್ಯ *buf);
uint ialloc(ushort type);
ಶೂನ್ಯ iappend(uint inum, ಶೂನ್ಯ *p, ಪೂರ್ಣಾಂಕ n);
ಶೂನ್ಯ die(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *);

// convert to riscv byte order
ushort
xshort(ushort x)
{
  ushort y;
  uchar *a = (uchar *)&y;
  a[0] = x;
  a[1] = x >> 8;
  ಹಿಂತಿರುಗಿ y;
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
  ಹಿಂತಿರುಗಿ y;
}

ಪೂರ್ಣಾಂಕ
ಮುಖ್ಯ(ಪೂರ್ಣಾಂಕ argc, ಅಕ್ಷರ *argv[])
{
  ಪೂರ್ಣಾಂಕ i, cc, fd;
  uint rootino, inum, off;
  ರಚನೆ dirent de;
  ಅಕ್ಷರ buf[BSIZE];
  ರಚನೆ dinode din;

  static_assert(ಗಾತ್ರ(ಪೂರ್ಣಾಂಕ) == 4, "Integers must be 4 bytes!");

  ಆದರೆ (argc < 2) {
    fprintf(stderr, "Usage: mkfs fs.img files...\n");
    ನಿರ್ಗಮಿಸು(1);
  }

  ಪ್ರತಿಜ್ಞಾ((BSIZE % ಗಾತ್ರ(ರಚನೆ dinode)) == 0);
  ಪ್ರತಿಜ್ಞಾ((BSIZE % ಗಾತ್ರ(ರಚನೆ dirent)) == 0);

  fsfd = open(argv[1], O_RDWR | O_CREAT | O_TRUNC, 0666);
  ಆದರೆ (fsfd < 0)
    die(argv[1]);

  // 1 fs block = 1 disk sector
  nmeta = 2 + nlog + ninodeblocks + nbitmap;
  nblocks = FSSIZE - nmeta;

  sb.magic = FSMAGIC;
  sb.ಗಾತ್ರ_2 = xint(FSSIZE);
  sb.nblocks = xint(nblocks);
  sb.ninodes = xint(NINODES);
  sb.nlog = xint(nlog);
  sb.logstart = xint(2);
  sb.inodestart = xint(2 + nlog);
  sb.bmapstart = xint(2 + nlog + ninodeblocks);

  ಬರೆ(
    "nmeta %d (boot, super, log blocks %u, inode blocks %u, bitmap blocks %u) blocks %d total %d\n",
    nmeta, nlog, ninodeblocks, nbitmap, nblocks, FSSIZE);

  freeblock = nmeta; // the first free block that we can allocate

  ಗಾಗಿ (i = 0; i < FSSIZE; i++)
    wsect(i, zeroes);

  memset(buf, 0, ಗಾತ್ರ(buf));
  memmove(buf, &sb, ಗಾತ್ರ(sb));
  wsect(1, buf);

  rootino = ialloc(T_DIR);
  ಪ್ರತಿಜ್ಞಾ(rootino == ROOTINO);

  bzero(&de, ಗಾತ್ರ(de));
  de.inum = xshort(rootino);
  strcpy(de.name, ".");
  iappend(rootino, &de, ಗಾತ್ರ(de));

  bzero(&de, ಗಾತ್ರ(de));
  de.inum = xshort(rootino);
  strcpy(de.name, "..");
  iappend(rootino, &de, ಗಾತ್ರ(de));

  ಗಾಗಿ (i = 2; i < argc; i++) {
    // get rid of "user/"
    ಅಕ್ಷರ *shortname;
    ಆದರೆ (ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(argv[i], "user/", 5) == 0)
      shortname = argv[i] + 5;
    ಇಲ್ಲದಿದ್ದರೆ
      shortname = argv[i];

    ಪ್ರತಿಜ್ಞಾ(index(shortname, '/') == 0);

    ಆದರೆ ((fd = open(argv[i], 0)) < 0)
      die(argv[i]);

    // Skip leading _ in name when writing to file system.
    // The binaries are named _rm, _cat, etc. to keep the
    // build operating system from trying to execute them
    // in place of system binaries like rm and cat.
    ಆದರೆ (shortname[0] == '_')
      shortname += 1;

    ಪ್ರತಿಜ್ಞಾ(ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(shortname) <= DIRSIZ);

    inum = ialloc(T_FILE);

    bzero(&de, ಗಾತ್ರ(de));
    de.inum = xshort(inum);
    strncpy(de.name, shortname, DIRSIZ);
    iappend(rootino, &de, ಗಾತ್ರ(de));

    ಆಗ ((cc = ಪಠ(fd, buf, ಗಾತ್ರ(buf))) > 0)
      iappend(inum, buf, cc);

    close(fd);
  }

  // fix size of root inode dir
  rinode(rootino, &din);
  off = xint(din.ಗಾತ್ರ_2);
  off = ((off / BSIZE) + 1) * BSIZE;
  din.ಗಾತ್ರ_2 = xint(off);
  winode(rootino, &din);

  balloc(freeblock);

  ನಿರ್ಗಮಿಸು(0);
}

ಶೂನ್ಯ
wsect(uint sec, ಶೂನ್ಯ *buf)
{
  ಆದರೆ (lseek(fsfd, sec * BSIZE, 0) != sec * BSIZE)
    die("lseek");
  ಆದರೆ (ಲಿಖ(fsfd, buf, BSIZE) != BSIZE)
    die("write");
}

ಶೂನ್ಯ
winode(uint inum, ರಚನೆ dinode *ip)
{
  ಅಕ್ಷರ buf[BSIZE];
  uint bn;
  ರಚನೆ dinode *dip;

  bn = IBLOCK(inum, sb);
  rsect(bn, buf);
  dip = ((ರಚನೆ dinode *)buf) + (inum % IPB);
  *dip = *ip;
  wsect(bn, buf);
}

ಶೂನ್ಯ
rinode(uint inum, ರಚನೆ dinode *ip)
{
  ಅಕ್ಷರ buf[BSIZE];
  uint bn;
  ರಚನೆ dinode *dip;

  bn = IBLOCK(inum, sb);
  rsect(bn, buf);
  dip = ((ರಚನೆ dinode *)buf) + (inum % IPB);
  *ip = *dip;
}

ಶೂನ್ಯ
rsect(uint sec, ಶೂನ್ಯ *buf)
{
  ಆದರೆ (lseek(fsfd, sec * BSIZE, 0) != sec * BSIZE)
    die("lseek");
  ಆದರೆ (ಪಠ(fsfd, buf, BSIZE) != BSIZE)
    die("read");
}

uint
ialloc(ushort type)
{
  uint inum = freeinode++;
  ರಚನೆ dinode din;

  bzero(&din, ಗಾತ್ರ(din));
  din.type = xshort(type);
  din.nlink = xshort(1);
  din.ಗಾತ್ರ_2 = xint(0);
  winode(inum, &din);
  ಹಿಂತಿರುಗಿ inum;
}

ಶೂನ್ಯ
balloc(ಪೂರ್ಣಾಂಕ ಬಳಸಿದ_2)
{
  uchar buf[BSIZE];
  ಪೂರ್ಣಾಂಕ i;

  ಬರೆ("balloc: first %d blocks have been allocated\n", ಬಳಸಿದ_2);
  ಪ್ರತಿಜ್ಞಾ(ಬಳಸಿದ_2 < BPB);
  bzero(buf, BSIZE);
  ಗಾಗಿ (i = 0; i < ಬಳಸಿದ_2; i++) {
    buf[i / 8] = buf[i / 8] | (0x1 << (i % 8));
  }
  ಬರೆ("balloc: write bitmap block at sector %d\n", sb.bmapstart);
  wsect(sb.bmapstart, buf);
}

#ವ್ಯಾಖ್ಯಾನಿಸು min(a, b) ((a) < (b) ? (a) : (b))

ಶೂನ್ಯ
iappend(uint inum, ಶೂನ್ಯ *xp, ಪೂರ್ಣಾಂಕ n)
{
  ಅಕ್ಷರ *p = (ಅಕ್ಷರ *)xp;
  uint fbn, off, n1;
  ರಚನೆ dinode din;
  ಅಕ್ಷರ buf[BSIZE];
  uint ಪರೋಕ್ಷ[NINDIRECT];
  uint x;

  rinode(inum, &din);
  off = xint(din.ಗಾತ್ರ_2);
  // printf("append inum %d at off %d sz %d\n", inum, off, n);
  ಆಗ (n > 0) {
    fbn = off / BSIZE;
    ಪ್ರತಿಜ್ಞಾ(fbn < MAXFILE);
    ಆದರೆ (fbn < NDIRECT) {
      ಆದರೆ (xint(din.addrs[fbn]) == 0) {
        din.addrs[fbn] = xint(freeblock++);
      }
      x = xint(din.addrs[fbn]);
    } ಇಲ್ಲದಿದ್ದರೆ {
      ಆದರೆ (xint(din.addrs[NDIRECT]) == 0) {
        din.addrs[NDIRECT] = xint(freeblock++);
      }
      rsect(xint(din.addrs[NDIRECT]), (ಅಕ್ಷರ *)ಪರೋಕ್ಷ);
      ಆದರೆ (ಪರೋಕ್ಷ[fbn - NDIRECT] == 0) {
        ಪರೋಕ್ಷ[fbn - NDIRECT] = xint(freeblock++);
        wsect(xint(din.addrs[NDIRECT]), (ಅಕ್ಷರ *)ಪರೋಕ್ಷ);
      }
      x = xint(ಪರೋಕ್ಷ[fbn - NDIRECT]);
    }
    n1 = min(n, (fbn + 1) * BSIZE - off);
    rsect(x, buf);
    bcopy(p, buf + off - (fbn * BSIZE), n1);
    wsect(x, buf);
    n -= n1;
    off += n1;
    p += n1;
  }
  din.ಗಾತ್ರ_2 = xint(off);
  winode(inum, &din);
}

ಶೂನ್ಯ
die(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s)
{
  perror(s);
  ನಿರ್ಗಮಿಸು(1);
}
