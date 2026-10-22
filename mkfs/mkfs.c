#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <unistd.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdlib.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <fcntl.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <assert.h>

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ stat xv6_stat // avoid clash with host struct stat
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/types.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/fs.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/stat.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/param.h"

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ static_assert
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ static_assert(a, b)                                                    \
  ਕਰੋ {                                                                         \
    ਚੋਣ (0)                                                                 \
    ਹਾਲਤ 0:                                                                    \
    ਹਾਲਤ (a):;                                                                 \
  } ਜਦੋਂ (0)
#ਜੇ_ਅੰਤ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NINODES 200

// Disk layout:
// [ boot block | sb block | log | inode blocks | free bit map | data blocks ]

ਪੂਰਨ nbitmap = FSSIZE / BPB + 1;
ਪੂਰਨ ninodeblocks = NINODES / IPB + 1;
ਪੂਰਨ nlog = LOGBLOCKS + 1; // Header followed by LOGBLOCKS data blocks.
ਪੂਰਨ nmeta;   // Number of meta blocks (boot, sb, nlog, inode, bitmap)
ਪੂਰਨ nblocks; // Number of data blocks

ਪੂਰਨ fsfd;
ਬਣਤਰ superblock sb;
ਅੱਖਰ zeroes[BSIZE];
uint freeinode = 1;
uint freeblock;

ਖਾਲੀ balloc(ਪੂਰਨ);
ਖਾਲੀ wsect(uint, ਖਾਲੀ *);
ਖਾਲੀ winode(uint, ਬਣਤਰ dinode *);
ਖਾਲੀ rinode(uint inum, ਬਣਤਰ dinode *ip);
ਖਾਲੀ rsect(uint sec, ਖਾਲੀ *buf);
uint ialloc(ushort type);
ਖਾਲੀ iappend(uint inum, ਖਾਲੀ *p, ਪੂਰਨ n);
ਖਾਲੀ die(ਅਚਲ ਅੱਖਰ *);

// convert to riscv byte order
ushort
xshort(ushort x)
{
  ushort y;
  uchar *a = (uchar *)&y;
  a[0] = x;
  a[1] = x >> 8;
  ਵਾਪਸ y;
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
  ਵਾਪਸ y;
}

ਪੂਰਨ
ਮੁੱਖ(ਪੂਰਨ argc, ਅੱਖਰ *argv[])
{
  ਪੂਰਨ i, cc, fd;
  uint rootino, inum, off;
  ਬਣਤਰ dirent de;
  ਅੱਖਰ buf[BSIZE];
  ਬਣਤਰ dinode din;

  static_assert(ਆਕਾਰ(ਪੂਰਨ) == 4, "Integers must be 4 bytes!");

  ਜੇ (argc < 2) {
    fprintf(stderr, "Usage: mkfs fs.img files...\n");
    ਬਾਹਰ_ਨਿਕਲੋ(1);
  }

  ਪ੍ਰਤਿਜ੍ਞਾ((BSIZE % ਆਕਾਰ(ਬਣਤਰ dinode)) == 0);
  ਪ੍ਰਤਿਜ੍ਞਾ((BSIZE % ਆਕਾਰ(ਬਣਤਰ dirent)) == 0);

  fsfd = open(argv[1], O_RDWR | O_CREAT | O_TRUNC, 0666);
  ਜੇ (fsfd < 0)
    die(argv[1]);

  // 1 fs block = 1 disk sector
  nmeta = 2 + nlog + ninodeblocks + nbitmap;
  nblocks = FSSIZE - nmeta;

  sb.magic = FSMAGIC;
  sb.ਮਾਪ = xint(FSSIZE);
  sb.nblocks = xint(nblocks);
  sb.ninodes = xint(NINODES);
  sb.nlog = xint(nlog);
  sb.logstart = xint(2);
  sb.inodestart = xint(2 + nlog);
  sb.bmapstart = xint(2 + nlog + ninodeblocks);

  ਲਿਖੋ(
    "nmeta %d (boot, super, log blocks %u, inode blocks %u, bitmap blocks %u) blocks %d total %d\n",
    nmeta, nlog, ninodeblocks, nbitmap, nblocks, FSSIZE);

  freeblock = nmeta; // the first free block that we can allocate

  ਲਈ (i = 0; i < FSSIZE; i++)
    wsect(i, zeroes);

  memset(buf, 0, ਆਕਾਰ(buf));
  memmove(buf, &sb, ਆਕਾਰ(sb));
  wsect(1, buf);

  rootino = ialloc(T_DIR);
  ਪ੍ਰਤਿਜ੍ਞਾ(rootino == ROOTINO);

  bzero(&de, ਆਕਾਰ(de));
  de.inum = xshort(rootino);
  strcpy(de.name, ".");
  iappend(rootino, &de, ਆਕਾਰ(de));

  bzero(&de, ਆਕਾਰ(de));
  de.inum = xshort(rootino);
  strcpy(de.name, "..");
  iappend(rootino, &de, ਆਕਾਰ(de));

  ਲਈ (i = 2; i < argc; i++) {
    // get rid of "user/"
    ਅੱਖਰ *shortname;
    ਜੇ (ਲੜੀ_ਤੁਲਨਾ(argv[i], "user/", 5) == 0)
      shortname = argv[i] + 5;
    ਨਹੀਂਤਾਂ
      shortname = argv[i];

    ਪ੍ਰਤਿਜ੍ਞਾ(index(shortname, '/') == 0);

    ਜੇ ((fd = open(argv[i], 0)) < 0)
      die(argv[i]);

    // Skip leading _ in name when writing to file system.
    // The binaries are named _rm, _cat, etc. to keep the
    // build operating system from trying to execute them
    // in place of system binaries like rm and cat.
    ਜੇ (shortname[0] == '_')
      shortname += 1;

    ਪ੍ਰਤਿਜ੍ਞਾ(ਲੜੀ_ਲੰਬਾਈ(shortname) <= DIRSIZ);

    inum = ialloc(T_FILE);

    bzero(&de, ਆਕਾਰ(de));
    de.inum = xshort(inum);
    strncpy(de.name, shortname, DIRSIZ);
    iappend(rootino, &de, ਆਕਾਰ(de));

    ਜਦੋਂ ((cc = ਪਠ(fd, buf, ਆਕਾਰ(buf))) > 0)
      iappend(inum, buf, cc);

    close(fd);
  }

  // fix size of root inode dir
  rinode(rootino, &din);
  off = xint(din.ਮਾਪ);
  off = ((off / BSIZE) + 1) * BSIZE;
  din.ਮਾਪ = xint(off);
  winode(rootino, &din);

  balloc(freeblock);

  ਬਾਹਰ_ਨਿਕਲੋ(0);
}

ਖਾਲੀ
wsect(uint sec, ਖਾਲੀ *buf)
{
  ਜੇ (lseek(fsfd, sec * BSIZE, 0) != sec * BSIZE)
    die("lseek");
  ਜੇ (ਲਿਖ(fsfd, buf, BSIZE) != BSIZE)
    die("write");
}

ਖਾਲੀ
winode(uint inum, ਬਣਤਰ dinode *ip)
{
  ਅੱਖਰ buf[BSIZE];
  uint bn;
  ਬਣਤਰ dinode *dip;

  bn = IBLOCK(inum, sb);
  rsect(bn, buf);
  dip = ((ਬਣਤਰ dinode *)buf) + (inum % IPB);
  *dip = *ip;
  wsect(bn, buf);
}

ਖਾਲੀ
rinode(uint inum, ਬਣਤਰ dinode *ip)
{
  ਅੱਖਰ buf[BSIZE];
  uint bn;
  ਬਣਤਰ dinode *dip;

  bn = IBLOCK(inum, sb);
  rsect(bn, buf);
  dip = ((ਬਣਤਰ dinode *)buf) + (inum % IPB);
  *ip = *dip;
}

ਖਾਲੀ
rsect(uint sec, ਖਾਲੀ *buf)
{
  ਜੇ (lseek(fsfd, sec * BSIZE, 0) != sec * BSIZE)
    die("lseek");
  ਜੇ (ਪਠ(fsfd, buf, BSIZE) != BSIZE)
    die("read");
}

uint
ialloc(ushort type)
{
  uint inum = freeinode++;
  ਬਣਤਰ dinode din;

  bzero(&din, ਆਕਾਰ(din));
  din.type = xshort(type);
  din.nlink = xshort(1);
  din.ਮਾਪ = xint(0);
  winode(inum, &din);
  ਵਾਪਸ inum;
}

ਖਾਲੀ
balloc(ਪੂਰਨ ਵਰਤਿਆ_2)
{
  uchar buf[BSIZE];
  ਪੂਰਨ i;

  ਲਿਖੋ("balloc: first %d blocks have been allocated\n", ਵਰਤਿਆ_2);
  ਪ੍ਰਤਿਜ੍ਞਾ(ਵਰਤਿਆ_2 < BPB);
  bzero(buf, BSIZE);
  ਲਈ (i = 0; i < ਵਰਤਿਆ_2; i++) {
    buf[i / 8] = buf[i / 8] | (0x1 << (i % 8));
  }
  ਲਿਖੋ("balloc: write bitmap block at sector %d\n", sb.bmapstart);
  wsect(sb.bmapstart, buf);
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ min(a, b) ((a) < (b) ? (a) : (b))

ਖਾਲੀ
iappend(uint inum, ਖਾਲੀ *xp, ਪੂਰਨ n)
{
  ਅੱਖਰ *p = (ਅੱਖਰ *)xp;
  uint fbn, off, n1;
  ਬਣਤਰ dinode din;
  ਅੱਖਰ buf[BSIZE];
  uint ਅਸਿੱਧਾ[NINDIRECT];
  uint x;

  rinode(inum, &din);
  off = xint(din.ਮਾਪ);
  // printf("append inum %d at off %d sz %d\n", inum, off, n);
  ਜਦੋਂ (n > 0) {
    fbn = off / BSIZE;
    ਪ੍ਰਤਿਜ੍ਞਾ(fbn < MAXFILE);
    ਜੇ (fbn < NDIRECT) {
      ਜੇ (xint(din.addrs[fbn]) == 0) {
        din.addrs[fbn] = xint(freeblock++);
      }
      x = xint(din.addrs[fbn]);
    } ਨਹੀਂਤਾਂ {
      ਜੇ (xint(din.addrs[NDIRECT]) == 0) {
        din.addrs[NDIRECT] = xint(freeblock++);
      }
      rsect(xint(din.addrs[NDIRECT]), (ਅੱਖਰ *)ਅਸਿੱਧਾ);
      ਜੇ (ਅਸਿੱਧਾ[fbn - NDIRECT] == 0) {
        ਅਸਿੱਧਾ[fbn - NDIRECT] = xint(freeblock++);
        wsect(xint(din.addrs[NDIRECT]), (ਅੱਖਰ *)ਅਸਿੱਧਾ);
      }
      x = xint(ਅਸਿੱਧਾ[fbn - NDIRECT]);
    }
    n1 = min(n, (fbn + 1) * BSIZE - off);
    rsect(x, buf);
    bcopy(p, buf + off - (fbn * BSIZE), n1);
    wsect(x, buf);
    n -= n1;
    off += n1;
    p += n1;
  }
  din.ਮਾਪ = xint(off);
  winode(inum, &din);
}

ਖਾਲੀ
die(ਅਚਲ ਅੱਖਰ *s)
{
  perror(s);
  ਬਾਹਰ_ਨਿਕਲੋ(1);
}
