#അന്തര്ഭാവ <stdio.h>
#അന്തര്ഭാവ <unistd.h>
#അന്തര്ഭാവ <stdlib.h>
#അന്തര്ഭാവ <string.h>
#അന്തര്ഭാവ <fcntl.h>
#അന്തര്ഭാവ <assert.h>

#define stat xv6_stat // avoid clash with host struct stat
#അന്തര്ഭാവ "kernel/types.h"
#അന്തര്ഭാവ "kernel/fs.h"
#അന്തര്ഭാവ "kernel/stat.h"
#അന്തര്ഭാവ "kernel/param.h"

#ifndef static_assert
#define static_assert(a, b)                                                    \
  ചെയ്യുക {                                                                         \
    തിരഞ്ഞെടുപ്പ് (0)                                                                 \
    അവസ്ഥ 0:                                                                    \
    അവസ്ഥ (a):;                                                                 \
  } ആകുമ്പോൾ (0)
#endif

#define NINODES 200

// Disk layout:
// [ boot block | sb block | log | inode blocks | free bit map | data blocks ]

പൂർണ്ണസംഖ്യ nbitmap = FSSIZE / BPB + 1;
പൂർണ്ണസംഖ്യ ninodeblocks = NINODES / IPB + 1;
പൂർണ്ണസംഖ്യ nlog = LOGBLOCKS + 1; // Header followed by LOGBLOCKS data blocks.
പൂർണ്ണസംഖ്യ nmeta;   // Number of meta blocks (boot, sb, nlog, inode, bitmap)
പൂർണ്ണസംഖ്യ nblocks; // Number of data blocks

പൂർണ്ണസംഖ്യ fsfd;
ഘടന superblock sb;
അക്ഷരം zeroes[BSIZE];
uint freeinode = 1;
uint freeblock;

ശൂന്യം balloc(പൂർണ്ണസംഖ്യ);
ശൂന്യം wsect(uint, ശൂന്യം *);
ശൂന്യം winode(uint, ഘടന dinode *);
ശൂന്യം rinode(uint inum, ഘടന dinode *ip);
ശൂന്യം rsect(uint sec, ശൂന്യം *buf);
uint ialloc(ushort type);
ശൂന്യം iappend(uint inum, ശൂന്യം *p, പൂർണ്ണസംഖ്യ n);
ശൂന്യം die(സ്ഥിരം അക്ഷരം *);

// convert to riscv byte order
ushort
xshort(ushort x)
{
  ushort y;
  uchar *a = (uchar *)&y;
  a[0] = x;
  a[1] = x >> 8;
  തിരികെ y;
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
  തിരികെ y;
}

പൂർണ്ണസംഖ്യ
പ്രധാന(പൂർണ്ണസംഖ്യ argc, അക്ഷരം *argv[])
{
  പൂർണ്ണസംഖ്യ i, cc, fd;
  uint rootino, inum, off;
  ഘടന dirent de;
  അക്ഷരം buf[BSIZE];
  ഘടന dinode din;

  static_assert(വലുപ്പം(പൂർണ്ണസംഖ്യ) == 4, "Integers must be 4 bytes!");

  എങ്കിൽ (argc < 2) {
    fprintf(stderr, "Usage: mkfs fs.img files...\n");
    exit(1);
  }

  പ്രതിജ്ഞാ((BSIZE % വലുപ്പം(ഘടന dinode)) == 0);
  പ്രതിജ്ഞാ((BSIZE % വലുപ്പം(ഘടന dirent)) == 0);

  fsfd = open(argv[1], O_RDWR | O_CREAT | O_TRUNC, 0666);
  എങ്കിൽ (fsfd < 0)
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

  വേണ്ടി (i = 0; i < FSSIZE; i++)
    wsect(i, zeroes);

  memset(buf, 0, വലുപ്പം(buf));
  memmove(buf, &sb, വലുപ്പം(sb));
  wsect(1, buf);

  rootino = ialloc(T_DIR);
  പ്രതിജ്ഞാ(rootino == ROOTINO);

  bzero(&de, വലുപ്പം(de));
  de.inum = xshort(rootino);
  strcpy(de.name, ".");
  iappend(rootino, &de, വലുപ്പം(de));

  bzero(&de, വലുപ്പം(de));
  de.inum = xshort(rootino);
  strcpy(de.name, "..");
  iappend(rootino, &de, വലുപ്പം(de));

  വേണ്ടി (i = 2; i < argc; i++) {
    // get rid of "user/"
    അക്ഷരം *shortname;
    എങ്കിൽ (strncmp(argv[i], "user/", 5) == 0)
      shortname = argv[i] + 5;
    അല്ലെങ്കിൽ
      shortname = argv[i];

    പ്രതിജ്ഞാ(index(shortname, '/') == 0);

    എങ്കിൽ ((fd = open(argv[i], 0)) < 0)
      die(argv[i]);

    // Skip leading _ in name when writing to file system.
    // The binaries are named _rm, _cat, etc. to keep the
    // build operating system from trying to execute them
    // in place of system binaries like rm and cat.
    എങ്കിൽ (shortname[0] == '_')
      shortname += 1;

    പ്രതിജ്ഞാ(strlen(shortname) <= DIRSIZ);

    inum = ialloc(T_FILE);

    bzero(&de, വലുപ്പം(de));
    de.inum = xshort(inum);
    strncpy(de.name, shortname, DIRSIZ);
    iappend(rootino, &de, വലുപ്പം(de));

    ആകുമ്പോൾ ((cc = പഠ(fd, buf, വലുപ്പം(buf))) > 0)
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

ശൂന്യം
wsect(uint sec, ശൂന്യം *buf)
{
  എങ്കിൽ (lseek(fsfd, sec * BSIZE, 0) != sec * BSIZE)
    die("lseek");
  എങ്കിൽ (ലിഖ(fsfd, buf, BSIZE) != BSIZE)
    die("write");
}

ശൂന്യം
winode(uint inum, ഘടന dinode *ip)
{
  അക്ഷരം buf[BSIZE];
  uint bn;
  ഘടന dinode *dip;

  bn = IBLOCK(inum, sb);
  rsect(bn, buf);
  dip = ((ഘടന dinode *)buf) + (inum % IPB);
  *dip = *ip;
  wsect(bn, buf);
}

ശൂന്യം
rinode(uint inum, ഘടന dinode *ip)
{
  അക്ഷരം buf[BSIZE];
  uint bn;
  ഘടന dinode *dip;

  bn = IBLOCK(inum, sb);
  rsect(bn, buf);
  dip = ((ഘടന dinode *)buf) + (inum % IPB);
  *ip = *dip;
}

ശൂന്യം
rsect(uint sec, ശൂന്യം *buf)
{
  എങ്കിൽ (lseek(fsfd, sec * BSIZE, 0) != sec * BSIZE)
    die("lseek");
  എങ്കിൽ (പഠ(fsfd, buf, BSIZE) != BSIZE)
    die("read");
}

uint
ialloc(ushort type)
{
  uint inum = freeinode++;
  ഘടന dinode din;

  bzero(&din, വലുപ്പം(din));
  din.type = xshort(type);
  din.nlink = xshort(1);
  din.size = xint(0);
  winode(inum, &din);
  തിരികെ inum;
}

ശൂന്യം
balloc(പൂർണ്ണസംഖ്യ used)
{
  uchar buf[BSIZE];
  പൂർണ്ണസംഖ്യ i;

  printf("balloc: first %d blocks have been allocated\n", used);
  പ്രതിജ്ഞാ(used < BPB);
  bzero(buf, BSIZE);
  വേണ്ടി (i = 0; i < used; i++) {
    buf[i / 8] = buf[i / 8] | (0x1 << (i % 8));
  }
  printf("balloc: write bitmap block at sector %d\n", sb.bmapstart);
  wsect(sb.bmapstart, buf);
}

#define min(a, b) ((a) < (b) ? (a) : (b))

ശൂന്യം
iappend(uint inum, ശൂന്യം *xp, പൂർണ്ണസംഖ്യ n)
{
  അക്ഷരം *p = (അക്ഷരം *)xp;
  uint fbn, off, n1;
  ഘടന dinode din;
  അക്ഷരം buf[BSIZE];
  uint indirect[NINDIRECT];
  uint x;

  rinode(inum, &din);
  off = xint(din.size);
  // printf("append inum %d at off %d sz %d\n", inum, off, n);
  ആകുമ്പോൾ (n > 0) {
    fbn = off / BSIZE;
    പ്രതിജ്ഞാ(fbn < MAXFILE);
    എങ്കിൽ (fbn < NDIRECT) {
      എങ്കിൽ (xint(din.addrs[fbn]) == 0) {
        din.addrs[fbn] = xint(freeblock++);
      }
      x = xint(din.addrs[fbn]);
    } അല്ലെങ്കിൽ {
      എങ്കിൽ (xint(din.addrs[NDIRECT]) == 0) {
        din.addrs[NDIRECT] = xint(freeblock++);
      }
      rsect(xint(din.addrs[NDIRECT]), (അക്ഷരം *)indirect);
      എങ്കിൽ (indirect[fbn - NDIRECT] == 0) {
        indirect[fbn - NDIRECT] = xint(freeblock++);
        wsect(xint(din.addrs[NDIRECT]), (അക്ഷരം *)indirect);
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

ശൂന്യം
die(സ്ഥിരം അക്ഷരം *s)
{
  perror(s);
  exit(1);
}
