#అన్తర్భావ <stdio.h>
#అన్తర్భావ <unistd.h>
#అన్తర్భావ <stdlib.h>
#అన్తర్భావ <string.h>
#అన్తర్భావ <fcntl.h>
#అన్తర్భావ <assert.h>

#నిర్వచించు stat xv6_stat // avoid clash with host struct stat
#అన్తర్భావ "kernel/types.h"
#అన్తర్భావ "kernel/fs.h"
#అన్తర్భావ "kernel/stat.h"
#అన్తర్భావ "kernel/param.h"

#నిర్వచితం_కాకపోతే static_assert
#నిర్వచించు static_assert(a, b)                                                    \
  చేయి {                                                                         \
    ఎంపిక (0)                                                                 \
    స్థితి 0:                                                                    \
    స్థితి (a):;                                                                 \
  } అయితే (0)
#అయితే_ముగింపు

#నిర్వచించు NINODES 200

// Disk layout:
// [ boot block | sb block | log | inode blocks | free bit map | data blocks ]

పూర్ణాంకం nbitmap = FSSIZE / BPB + 1;
పూర్ణాంకం ninodeblocks = NINODES / IPB + 1;
పూర్ణాంకం nlog = LOGBLOCKS + 1; // Header followed by LOGBLOCKS data blocks.
పూర్ణాంకం nmeta;   // Number of meta blocks (boot, sb, nlog, inode, bitmap)
పూర్ణాంకం nblocks; // Number of data blocks

పూర్ణాంకం fsfd;
నిర్మాణం superblock sb;
అక్షరం zeroes[BSIZE];
uint freeinode = 1;
uint freeblock;

శూన్య balloc(పూర్ణాంకం);
శూన్య wsect(uint, శూన్య *);
శూన్య winode(uint, నిర్మాణం dinode *);
శూన్య rinode(uint inum, నిర్మాణం dinode *ip);
శూన్య rsect(uint sec, శూన్య *buf);
uint ialloc(ushort type);
శూన్య iappend(uint inum, శూన్య *p, పూర్ణాంకం n);
శూన్య die(స్థిరరాశి అక్షరం *);

// convert to riscv byte order
ushort
xshort(ushort x)
{
  ushort y;
  uchar *a = (uchar *)&y;
  a[0] = x;
  a[1] = x >> 8;
  తిరిగి y;
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
  తిరిగి y;
}

పూర్ణాంకం
ప్రధాన(పూర్ణాంకం argc, అక్షరం *argv[])
{
  పూర్ణాంకం i, cc, fd;
  uint rootino, inum, off;
  నిర్మాణం dirent de;
  అక్షరం buf[BSIZE];
  నిర్మాణం dinode din;

  static_assert(పరిమాణం(పూర్ణాంకం) == 4, "Integers must be 4 bytes!");

  ఒకవేళ (argc < 2) {
    fprintf(stderr, "Usage: mkfs fs.img files...\n");
    నిష్క్రమించు(1);
  }

  ప్రతిజ్ఞా((BSIZE % పరిమాణం(నిర్మాణం dinode)) == 0);
  ప్రతిజ్ఞా((BSIZE % పరిమాణం(నిర్మాణం dirent)) == 0);

  fsfd = open(argv[1], O_RDWR | O_CREAT | O_TRUNC, 0666);
  ఒకవేళ (fsfd < 0)
    die(argv[1]);

  // 1 fs block = 1 disk sector
  nmeta = 2 + nlog + ninodeblocks + nbitmap;
  nblocks = FSSIZE - nmeta;

  sb.magic = FSMAGIC;
  sb.కొలత = xint(FSSIZE);
  sb.nblocks = xint(nblocks);
  sb.ninodes = xint(NINODES);
  sb.nlog = xint(nlog);
  sb.logstart = xint(2);
  sb.inodestart = xint(2 + nlog);
  sb.bmapstart = xint(2 + nlog + ninodeblocks);

  రాయి(
    "nmeta %d (boot, super, log blocks %u, inode blocks %u, bitmap blocks %u) blocks %d total %d\n",
    nmeta, nlog, ninodeblocks, nbitmap, nblocks, FSSIZE);

  freeblock = nmeta; // the first free block that we can allocate

  కొరకు (i = 0; i < FSSIZE; i++)
    wsect(i, zeroes);

  memset(buf, 0, పరిమాణం(buf));
  memmove(buf, &sb, పరిమాణం(sb));
  wsect(1, buf);

  rootino = ialloc(T_DIR);
  ప్రతిజ్ఞా(rootino == ROOTINO);

  bzero(&de, పరిమాణం(de));
  de.inum = xshort(rootino);
  strcpy(de.name, ".");
  iappend(rootino, &de, పరిమాణం(de));

  bzero(&de, పరిమాణం(de));
  de.inum = xshort(rootino);
  strcpy(de.name, "..");
  iappend(rootino, &de, పరిమాణం(de));

  కొరకు (i = 2; i < argc; i++) {
    // get rid of "user/"
    అక్షరం *shortname;
    ఒకవేళ (అక్షరశ్రేణి_పోలిక(argv[i], "user/", 5) == 0)
      shortname = argv[i] + 5;
    లేకపోతే
      shortname = argv[i];

    ప్రతిజ్ఞా(index(shortname, '/') == 0);

    ఒకవేళ ((fd = open(argv[i], 0)) < 0)
      die(argv[i]);

    // Skip leading _ in name when writing to file system.
    // The binaries are named _rm, _cat, etc. to keep the
    // build operating system from trying to execute them
    // in place of system binaries like rm and cat.
    ఒకవేళ (shortname[0] == '_')
      shortname += 1;

    ప్రతిజ్ఞా(అక్షరశ్రేణి_పొడవు(shortname) <= DIRSIZ);

    inum = ialloc(T_FILE);

    bzero(&de, పరిమాణం(de));
    de.inum = xshort(inum);
    strncpy(de.name, shortname, DIRSIZ);
    iappend(rootino, &de, పరిమాణం(de));

    అయితే ((cc = పఠ(fd, buf, పరిమాణం(buf))) > 0)
      iappend(inum, buf, cc);

    close(fd);
  }

  // fix size of root inode dir
  rinode(rootino, &din);
  off = xint(din.కొలత);
  off = ((off / BSIZE) + 1) * BSIZE;
  din.కొలత = xint(off);
  winode(rootino, &din);

  balloc(freeblock);

  నిష్క్రమించు(0);
}

శూన్య
wsect(uint sec, శూన్య *buf)
{
  ఒకవేళ (lseek(fsfd, sec * BSIZE, 0) != sec * BSIZE)
    die("lseek");
  ఒకవేళ (లిఖ(fsfd, buf, BSIZE) != BSIZE)
    die("write");
}

శూన్య
winode(uint inum, నిర్మాణం dinode *ip)
{
  అక్షరం buf[BSIZE];
  uint bn;
  నిర్మాణం dinode *dip;

  bn = IBLOCK(inum, sb);
  rsect(bn, buf);
  dip = ((నిర్మాణం dinode *)buf) + (inum % IPB);
  *dip = *ip;
  wsect(bn, buf);
}

శూన్య
rinode(uint inum, నిర్మాణం dinode *ip)
{
  అక్షరం buf[BSIZE];
  uint bn;
  నిర్మాణం dinode *dip;

  bn = IBLOCK(inum, sb);
  rsect(bn, buf);
  dip = ((నిర్మాణం dinode *)buf) + (inum % IPB);
  *ip = *dip;
}

శూన్య
rsect(uint sec, శూన్య *buf)
{
  ఒకవేళ (lseek(fsfd, sec * BSIZE, 0) != sec * BSIZE)
    die("lseek");
  ఒకవేళ (పఠ(fsfd, buf, BSIZE) != BSIZE)
    die("read");
}

uint
ialloc(ushort type)
{
  uint inum = freeinode++;
  నిర్మాణం dinode din;

  bzero(&din, పరిమాణం(din));
  din.type = xshort(type);
  din.nlink = xshort(1);
  din.కొలత = xint(0);
  winode(inum, &din);
  తిరిగి inum;
}

శూన్య
balloc(పూర్ణాంకం వాడిన_2)
{
  uchar buf[BSIZE];
  పూర్ణాంకం i;

  రాయి("balloc: first %d blocks have been allocated\n", వాడిన_2);
  ప్రతిజ్ఞా(వాడిన_2 < BPB);
  bzero(buf, BSIZE);
  కొరకు (i = 0; i < వాడిన_2; i++) {
    buf[i / 8] = buf[i / 8] | (0x1 << (i % 8));
  }
  రాయి("balloc: write bitmap block at sector %d\n", sb.bmapstart);
  wsect(sb.bmapstart, buf);
}

#నిర్వచించు min(a, b) ((a) < (b) ? (a) : (b))

శూన్య
iappend(uint inum, శూన్య *xp, పూర్ణాంకం n)
{
  అక్షరం *p = (అక్షరం *)xp;
  uint fbn, off, n1;
  నిర్మాణం dinode din;
  అక్షరం buf[BSIZE];
  uint పరోక్ష[NINDIRECT];
  uint x;

  rinode(inum, &din);
  off = xint(din.కొలత);
  // printf("append inum %d at off %d sz %d\n", inum, off, n);
  అయితే (n > 0) {
    fbn = off / BSIZE;
    ప్రతిజ్ఞా(fbn < MAXFILE);
    ఒకవేళ (fbn < NDIRECT) {
      ఒకవేళ (xint(din.addrs[fbn]) == 0) {
        din.addrs[fbn] = xint(freeblock++);
      }
      x = xint(din.addrs[fbn]);
    } లేకపోతే {
      ఒకవేళ (xint(din.addrs[NDIRECT]) == 0) {
        din.addrs[NDIRECT] = xint(freeblock++);
      }
      rsect(xint(din.addrs[NDIRECT]), (అక్షరం *)పరోక్ష);
      ఒకవేళ (పరోక్ష[fbn - NDIRECT] == 0) {
        పరోక్ష[fbn - NDIRECT] = xint(freeblock++);
        wsect(xint(din.addrs[NDIRECT]), (అక్షరం *)పరోక్ష);
      }
      x = xint(పరోక్ష[fbn - NDIRECT]);
    }
    n1 = min(n, (fbn + 1) * BSIZE - off);
    rsect(x, buf);
    bcopy(p, buf + off - (fbn * BSIZE), n1);
    wsect(x, buf);
    n -= n1;
    off += n1;
    p += n1;
  }
  din.కొలత = xint(off);
  winode(inum, &din);
}

శూన్య
die(స్థిరరాశి అక్షరం *s)
{
  perror(s);
  నిష్క్రమించు(1);
}
