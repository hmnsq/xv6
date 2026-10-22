#અન્તર્ભાવ <stdio.h>
#અન્તર્ભાવ <unistd.h>
#અન્તર્ભાવ <stdlib.h>
#અન્તર્ભાવ <string.h>
#અન્તર્ભાવ <fcntl.h>
#અન્તર્ભાવ <assert.h>

#define stat xv6_stat // avoid clash with host struct stat
#અન્તર્ભાવ "kernel/types.h"
#અન્તર્ભાવ "kernel/fs.h"
#અન્તર્ભાવ "kernel/stat.h"
#અન્તર્ભાવ "kernel/param.h"

#ifndef static_assert
#define static_assert(a, b)                                                    \
  કરો {                                                                         \
    પસંદગી (0)                                                                 \
    સ્થિતિ 0:                                                                    \
    સ્થિતિ (a):;                                                                 \
  } જ્યારે (0)
#endif

#define NINODES 200

// Disk layout:
// [ boot block | sb block | log | inode blocks | free bit map | data blocks ]

પૂર્ણાંક nbitmap = FSSIZE / BPB + 1;
પૂર્ણાંક ninodeblocks = NINODES / IPB + 1;
પૂર્ણાંક nlog = LOGBLOCKS + 1; // Header followed by LOGBLOCKS data blocks.
પૂર્ણાંક nmeta;   // Number of meta blocks (boot, sb, nlog, inode, bitmap)
પૂર્ણાંક nblocks; // Number of data blocks

પૂર્ણાંક fsfd;
સંરચના superblock sb;
અક્ષર zeroes[BSIZE];
uint freeinode = 1;
uint freeblock;

શૂન્ય balloc(પૂર્ણાંક);
શૂન્ય wsect(uint, શૂન્ય *);
શૂન્ય winode(uint, સંરચના dinode *);
શૂન્ય rinode(uint inum, સંરચના dinode *ip);
શૂન્ય rsect(uint sec, શૂન્ય *buf);
uint ialloc(ushort type);
શૂન્ય iappend(uint inum, શૂન્ય *p, પૂર્ણાંક n);
શૂન્ય die(અચલ અક્ષર *);

// convert to riscv byte order
ushort
xshort(ushort x)
{
  ushort y;
  uchar *a = (uchar *)&y;
  a[0] = x;
  a[1] = x >> 8;
  પાછા y;
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
  પાછા y;
}

પૂર્ણાંક
મુખ્ય(પૂર્ણાંક argc, અક્ષર *argv[])
{
  પૂર્ણાંક i, cc, fd;
  uint rootino, inum, off;
  સંરચના dirent de;
  અક્ષર buf[BSIZE];
  સંરચના dinode din;

  static_assert(કદ(પૂર્ણાંક) == 4, "Integers must be 4 bytes!");

  જો (argc < 2) {
    fprintf(stderr, "Usage: mkfs fs.img files...\n");
    exit(1);
  }

  પ્રતિજ્ઞા((BSIZE % કદ(સંરચના dinode)) == 0);
  પ્રતિજ્ઞા((BSIZE % કદ(સંરચના dirent)) == 0);

  fsfd = open(argv[1], O_RDWR | O_CREAT | O_TRUNC, 0666);
  જો (fsfd < 0)
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

  માટે (i = 0; i < FSSIZE; i++)
    wsect(i, zeroes);

  memset(buf, 0, કદ(buf));
  memmove(buf, &sb, કદ(sb));
  wsect(1, buf);

  rootino = ialloc(T_DIR);
  પ્રતિજ્ઞા(rootino == ROOTINO);

  bzero(&de, કદ(de));
  de.inum = xshort(rootino);
  strcpy(de.name, ".");
  iappend(rootino, &de, કદ(de));

  bzero(&de, કદ(de));
  de.inum = xshort(rootino);
  strcpy(de.name, "..");
  iappend(rootino, &de, કદ(de));

  માટે (i = 2; i < argc; i++) {
    // get rid of "user/"
    અક્ષર *shortname;
    જો (strncmp(argv[i], "user/", 5) == 0)
      shortname = argv[i] + 5;
    નહીંતર
      shortname = argv[i];

    પ્રતિજ્ઞા(index(shortname, '/') == 0);

    જો ((fd = open(argv[i], 0)) < 0)
      die(argv[i]);

    // Skip leading _ in name when writing to file system.
    // The binaries are named _rm, _cat, etc. to keep the
    // build operating system from trying to execute them
    // in place of system binaries like rm and cat.
    જો (shortname[0] == '_')
      shortname += 1;

    પ્રતિજ્ઞા(strlen(shortname) <= DIRSIZ);

    inum = ialloc(T_FILE);

    bzero(&de, કદ(de));
    de.inum = xshort(inum);
    strncpy(de.name, shortname, DIRSIZ);
    iappend(rootino, &de, કદ(de));

    જ્યારે ((cc = પઠ(fd, buf, કદ(buf))) > 0)
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

શૂન્ય
wsect(uint sec, શૂન્ય *buf)
{
  જો (lseek(fsfd, sec * BSIZE, 0) != sec * BSIZE)
    die("lseek");
  જો (લિખ(fsfd, buf, BSIZE) != BSIZE)
    die("write");
}

શૂન્ય
winode(uint inum, સંરચના dinode *ip)
{
  અક્ષર buf[BSIZE];
  uint bn;
  સંરચના dinode *dip;

  bn = IBLOCK(inum, sb);
  rsect(bn, buf);
  dip = ((સંરચના dinode *)buf) + (inum % IPB);
  *dip = *ip;
  wsect(bn, buf);
}

શૂન્ય
rinode(uint inum, સંરચના dinode *ip)
{
  અક્ષર buf[BSIZE];
  uint bn;
  સંરચના dinode *dip;

  bn = IBLOCK(inum, sb);
  rsect(bn, buf);
  dip = ((સંરચના dinode *)buf) + (inum % IPB);
  *ip = *dip;
}

શૂન્ય
rsect(uint sec, શૂન્ય *buf)
{
  જો (lseek(fsfd, sec * BSIZE, 0) != sec * BSIZE)
    die("lseek");
  જો (પઠ(fsfd, buf, BSIZE) != BSIZE)
    die("read");
}

uint
ialloc(ushort type)
{
  uint inum = freeinode++;
  સંરચના dinode din;

  bzero(&din, કદ(din));
  din.type = xshort(type);
  din.nlink = xshort(1);
  din.size = xint(0);
  winode(inum, &din);
  પાછા inum;
}

શૂન્ય
balloc(પૂર્ણાંક used)
{
  uchar buf[BSIZE];
  પૂર્ણાંક i;

  printf("balloc: first %d blocks have been allocated\n", used);
  પ્રતિજ્ઞા(used < BPB);
  bzero(buf, BSIZE);
  માટે (i = 0; i < used; i++) {
    buf[i / 8] = buf[i / 8] | (0x1 << (i % 8));
  }
  printf("balloc: write bitmap block at sector %d\n", sb.bmapstart);
  wsect(sb.bmapstart, buf);
}

#define min(a, b) ((a) < (b) ? (a) : (b))

શૂન્ય
iappend(uint inum, શૂન્ય *xp, પૂર્ણાંક n)
{
  અક્ષર *p = (અક્ષર *)xp;
  uint fbn, off, n1;
  સંરચના dinode din;
  અક્ષર buf[BSIZE];
  uint indirect[NINDIRECT];
  uint x;

  rinode(inum, &din);
  off = xint(din.size);
  // printf("append inum %d at off %d sz %d\n", inum, off, n);
  જ્યારે (n > 0) {
    fbn = off / BSIZE;
    પ્રતિજ્ઞા(fbn < MAXFILE);
    જો (fbn < NDIRECT) {
      જો (xint(din.addrs[fbn]) == 0) {
        din.addrs[fbn] = xint(freeblock++);
      }
      x = xint(din.addrs[fbn]);
    } નહીંતર {
      જો (xint(din.addrs[NDIRECT]) == 0) {
        din.addrs[NDIRECT] = xint(freeblock++);
      }
      rsect(xint(din.addrs[NDIRECT]), (અક્ષર *)indirect);
      જો (indirect[fbn - NDIRECT] == 0) {
        indirect[fbn - NDIRECT] = xint(freeblock++);
        wsect(xint(din.addrs[NDIRECT]), (અક્ષર *)indirect);
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

શૂન્ય
die(અચલ અક્ષર *s)
{
  perror(s);
  exit(1);
}
