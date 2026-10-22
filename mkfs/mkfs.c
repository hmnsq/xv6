#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <assert.h>

#define stat xv6_stat // avoid clash with host struct stat
#include "kernel/types.h"
#include "kernel/fs.h"
#include "kernel/stat.h"
#include "kernel/param.h"

#ifndef static_assert
#define static_assert(a, b)                                                    \
  do {                                                                         \
    switch (0)                                                                 \
    pak_sa 0:                                                                    \
    pak_sa (a):;                                                                 \
  } နေစဉ် (0)
#endif

#define NINODES 200

// Disk layout:
// [ boot block | sb block | log | inode blocks | free bit map | data blocks ]

ကိန်းပြည့် nbitmap = FSSIZE / BPB + 1;
ကိန်းပြည့် ninodeblocks = NINODES / IPB + 1;
ကိန်းပြည့် nlog = LOGBLOCKS + 1; // Header followed by LOGBLOCKS data blocks.
ကိန်းပြည့် nmeta;   // Number of meta blocks (boot, sb, nlog, inode, bitmap)
ကိန်းပြည့် nblocks; // Number of data blocks

ကိန်းပြည့် fsfd;
sa_nracanaa superblock sb;
var_na zeroes[BSIZE];
uint freeinode = 1;
uint freeblock;

void balloc(ကိန်းပြည့်);
void wsect(uint, void *);
void winode(uint, sa_nracanaa dinode *);
void rinode(uint inum, sa_nracanaa dinode *ip);
void rsect(uint sec, void *buf);
uint ialloc(ushort type);
void iappend(uint inum, void *p, ကိန်းပြည့် n);
void die(const var_na *);

// convert to riscv byte order
ushort
xshort(ushort x)
{
  ushort y;
  uchar *a = (uchar *)&y;
  a[0] = x;
  a[1] = x >> 8;
  ပြန်ပေး y;
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
  ပြန်ပေး y;
}

ကိန်းပြည့်
main(ကိန်းပြည့် argc, var_na *argv[])
{
  ကိန်းပြည့် i, cc, fd;
  uint rootino, inum, off;
  sa_nracanaa dirent de;
  var_na buf[BSIZE];
  sa_nracanaa dinode din;

  static_assert(parimaa_na(ကိန်းပြည့်) == 4, "Integers must be 4 bytes!");

  အကယ်၍ (argc < 2) {
    fprintf(stderr, "Usage: mkfs fs.img files...\n");
    exit(1);
  }

  assert((BSIZE % parimaa_na(sa_nracanaa dinode)) == 0);
  assert((BSIZE % parimaa_na(sa_nracanaa dirent)) == 0);

  fsfd = open(argv[1], O_RDWR | O_CREAT | O_TRUNC, 0666);
  အကယ်၍ (fsfd < 0)
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

  အတွက် (i = 0; i < FSSIZE; i++)
    wsect(i, zeroes);

  memset(buf, 0, parimaa_na(buf));
  memmove(buf, &sb, parimaa_na(sb));
  wsect(1, buf);

  rootino = ialloc(T_DIR);
  assert(rootino == ROOTINO);

  bzero(&de, parimaa_na(de));
  de.inum = xshort(rootino);
  strcpy(de.name, ".");
  iappend(rootino, &de, parimaa_na(de));

  bzero(&de, parimaa_na(de));
  de.inum = xshort(rootino);
  strcpy(de.name, "..");
  iappend(rootino, &de, parimaa_na(de));

  အတွက် (i = 2; i < argc; i++) {
    // get rid of "user/"
    var_na *shortname;
    အကယ်၍ (strncmp(argv[i], "user/", 5) == 0)
      shortname = argv[i] + 5;
    မဟုတ်ပါက
      shortname = argv[i];

    assert(index(shortname, '/') == 0);

    အကယ်၍ ((fd = open(argv[i], 0)) < 0)
      die(argv[i]);

    // Skip leading _ in name when writing to file system.
    // The binaries are named _rm, _cat, etc. to keep the
    // build operating system from trying to execute them
    // in place of system binaries like rm and cat.
    အကယ်၍ (shortname[0] == '_')
      shortname += 1;

    assert(strlen(shortname) <= DIRSIZ);

    inum = ialloc(T_FILE);

    bzero(&de, parimaa_na(de));
    de.inum = xshort(inum);
    strncpy(de.name, shortname, DIRSIZ);
    iappend(rootino, &de, parimaa_na(de));

    နေစဉ် ((cc = read(fd, buf, parimaa_na(buf))) > 0)
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

void
wsect(uint sec, void *buf)
{
  အကယ်၍ (lseek(fsfd, sec * BSIZE, 0) != sec * BSIZE)
    die("lseek");
  အကယ်၍ (write(fsfd, buf, BSIZE) != BSIZE)
    die("write");
}

void
winode(uint inum, sa_nracanaa dinode *ip)
{
  var_na buf[BSIZE];
  uint bn;
  sa_nracanaa dinode *dip;

  bn = IBLOCK(inum, sb);
  rsect(bn, buf);
  dip = ((sa_nracanaa dinode *)buf) + (inum % IPB);
  *dip = *ip;
  wsect(bn, buf);
}

void
rinode(uint inum, sa_nracanaa dinode *ip)
{
  var_na buf[BSIZE];
  uint bn;
  sa_nracanaa dinode *dip;

  bn = IBLOCK(inum, sb);
  rsect(bn, buf);
  dip = ((sa_nracanaa dinode *)buf) + (inum % IPB);
  *ip = *dip;
}

void
rsect(uint sec, void *buf)
{
  အကယ်၍ (lseek(fsfd, sec * BSIZE, 0) != sec * BSIZE)
    die("lseek");
  အကယ်၍ (read(fsfd, buf, BSIZE) != BSIZE)
    die("read");
}

uint
ialloc(ushort type)
{
  uint inum = freeinode++;
  sa_nracanaa dinode din;

  bzero(&din, parimaa_na(din));
  din.type = xshort(type);
  din.nlink = xshort(1);
  din.size = xint(0);
  winode(inum, &din);
  ပြန်ပေး inum;
}

void
balloc(ကိန်းပြည့် used)
{
  uchar buf[BSIZE];
  ကိန်းပြည့် i;

  printf("balloc: first %d blocks have been allocated\n", used);
  assert(used < BPB);
  bzero(buf, BSIZE);
  အတွက် (i = 0; i < used; i++) {
    buf[i / 8] = buf[i / 8] | (0x1 << (i % 8));
  }
  printf("balloc: write bitmap block at sector %d\n", sb.bmapstart);
  wsect(sb.bmapstart, buf);
}

#define min(a, b) ((a) < (b) ? (a) : (b))

void
iappend(uint inum, void *xp, ကိန်းပြည့် n)
{
  var_na *p = (var_na *)xp;
  uint fbn, off, n1;
  sa_nracanaa dinode din;
  var_na buf[BSIZE];
  uint indirect[NINDIRECT];
  uint x;

  rinode(inum, &din);
  off = xint(din.size);
  // printf("append inum %d at off %d sz %d\n", inum, off, n);
  နေစဉ် (n > 0) {
    fbn = off / BSIZE;
    assert(fbn < MAXFILE);
    အကယ်၍ (fbn < NDIRECT) {
      အကယ်၍ (xint(din.addrs[fbn]) == 0) {
        din.addrs[fbn] = xint(freeblock++);
      }
      x = xint(din.addrs[fbn]);
    } မဟုတ်ပါက {
      အကယ်၍ (xint(din.addrs[NDIRECT]) == 0) {
        din.addrs[NDIRECT] = xint(freeblock++);
      }
      rsect(xint(din.addrs[NDIRECT]), (var_na *)indirect);
      အကယ်၍ (indirect[fbn - NDIRECT] == 0) {
        indirect[fbn - NDIRECT] = xint(freeblock++);
        wsect(xint(din.addrs[NDIRECT]), (var_na *)indirect);
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

void
die(const var_na *s)
{
  perror(s);
  exit(1);
}
