#ضمن <stdio.h>
#ضمن <unistd.h>
#ضمن <stdlib.h>
#ضمن <string.h>
#ضمن <fcntl.h>
#ضمن <assert.h>

#وصف_ڪريو stat xv6_stat // avoid clash with host struct stat
#ضمن "kernel/types.h"
#ضمن "kernel/fs.h"
#ضمن "kernel/stat.h"
#ضمن "kernel/param.h"

#جيڪڏهن_وصف_نه_ٿيل static_assert
#وصف_ڪريو static_assert(a, b)                                                    \
  افعل {                                                                         \
    بدل (0)                                                                 \
    حالة 0:                                                                    \
    حالة (a):;                                                                 \
  } طالما (0)
#جيڪڏهن_پورو

#وصف_ڪريو NINODES 200

// Disk layout:
// [ boot block | sb block | log | inode blocks | free bit map | data blocks ]

صحيح nbitmap = FSSIZE / BPB + 1;
صحيح ninodeblocks = NINODES / IPB + 1;
صحيح nlog = LOGBLOCKS + 1; // Header followed by LOGBLOCKS data blocks.
صحيح nmeta;   // Number of meta blocks (boot, sb, nlog, inode, bitmap)
صحيح nblocks; // Number of data blocks

صحيح fsfd;
بنية superblock sb;
حرف zeroes[BSIZE];
uint freeinode = 1;
uint freeblock;

فراغ balloc(صحيح);
فراغ wsect(uint, فراغ *);
فراغ winode(uint, بنية dinode *);
فراغ rinode(uint inum, بنية dinode *ip);
فراغ rsect(uint sec, فراغ *buf);
uint ialloc(ushort type);
فراغ iappend(uint inum, فراغ *p, صحيح n);
فراغ die(ثابت حرف *);

// convert to riscv byte order
ushort
xshort(ushort x)
{
  ushort y;
  uchar *a = (uchar *)&y;
  a[0] = x;
  a[1] = x >> 8;
  أرجع y;
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
  أرجع y;
}

صحيح
مکيه(صحيح argc, حرف *argv[])
{
  صحيح i, cc, fd;
  uint rootino, inum, off;
  بنية dirent de;
  حرف buf[BSIZE];
  بنية dinode din;

  static_assert(حجم(صحيح) == 4, "Integers must be 4 bytes!");

  إذا (argc < 2) {
    fprintf(stderr, "Usage: mkfs fs.img files...\n");
    ٻاهر_نڪرو(1);
  }

  أكد((BSIZE % حجم(بنية dinode)) == 0);
  أكد((BSIZE % حجم(بنية dirent)) == 0);

  fsfd = open(argv[1], O_RDWR | O_CREAT | O_TRUNC, 0666);
  إذا (fsfd < 0)
    die(argv[1]);

  // 1 fs block = 1 disk sector
  nmeta = 2 + nlog + ninodeblocks + nbitmap;
  nblocks = FSSIZE - nmeta;

  sb.magic = FSMAGIC;
  sb.سائيز = xint(FSSIZE);
  sb.nblocks = xint(nblocks);
  sb.ninodes = xint(NINODES);
  sb.nlog = xint(nlog);
  sb.logstart = xint(2);
  sb.inodestart = xint(2 + nlog);
  sb.bmapstart = xint(2 + nlog + ninodeblocks);

  لکو(
    "nmeta %d (boot, super, log blocks %u, inode blocks %u, bitmap blocks %u) blocks %d total %d\n",
    nmeta, nlog, ninodeblocks, nbitmap, nblocks, FSSIZE);

  freeblock = nmeta; // the first free block that we can allocate

  لكل (i = 0; i < FSSIZE; i++)
    wsect(i, zeroes);

  memset(buf, 0, حجم(buf));
  memmove(buf, &sb, حجم(sb));
  wsect(1, buf);

  rootino = ialloc(T_DIR);
  أكد(rootino == ROOTINO);

  bzero(&de, حجم(de));
  de.inum = xshort(rootino);
  strcpy(de.name, ".");
  iappend(rootino, &de, حجم(de));

  bzero(&de, حجم(de));
  de.inum = xshort(rootino);
  strcpy(de.name, "..");
  iappend(rootino, &de, حجم(de));

  لكل (i = 2; i < argc; i++) {
    // get rid of "user/"
    حرف *shortname;
    إذا (لڙي_ڀيٽ(argv[i], "user/", 5) == 0)
      shortname = argv[i] + 5;
    وإلا
      shortname = argv[i];

    أكد(index(shortname, '/') == 0);

    إذا ((fd = open(argv[i], 0)) < 0)
      die(argv[i]);

    // Skip leading _ in name when writing to file system.
    // The binaries are named _rm, _cat, etc. to keep the
    // build operating system from trying to execute them
    // in place of system binaries like rm and cat.
    إذا (shortname[0] == '_')
      shortname += 1;

    أكد(لڙي_ڊيگهه(shortname) <= DIRSIZ);

    inum = ialloc(T_FILE);

    bzero(&de, حجم(de));
    de.inum = xshort(inum);
    strncpy(de.name, shortname, DIRSIZ);
    iappend(rootino, &de, حجم(de));

    طالما ((cc = اقرأ(fd, buf, حجم(buf))) > 0)
      iappend(inum, buf, cc);

    close(fd);
  }

  // fix size of root inode dir
  rinode(rootino, &din);
  off = xint(din.سائيز);
  off = ((off / BSIZE) + 1) * BSIZE;
  din.سائيز = xint(off);
  winode(rootino, &din);

  balloc(freeblock);

  ٻاهر_نڪرو(0);
}

فراغ
wsect(uint sec, فراغ *buf)
{
  إذا (lseek(fsfd, sec * BSIZE, 0) != sec * BSIZE)
    die("lseek");
  إذا (اكتب(fsfd, buf, BSIZE) != BSIZE)
    die("write");
}

فراغ
winode(uint inum, بنية dinode *ip)
{
  حرف buf[BSIZE];
  uint bn;
  بنية dinode *dip;

  bn = IBLOCK(inum, sb);
  rsect(bn, buf);
  dip = ((بنية dinode *)buf) + (inum % IPB);
  *dip = *ip;
  wsect(bn, buf);
}

فراغ
rinode(uint inum, بنية dinode *ip)
{
  حرف buf[BSIZE];
  uint bn;
  بنية dinode *dip;

  bn = IBLOCK(inum, sb);
  rsect(bn, buf);
  dip = ((بنية dinode *)buf) + (inum % IPB);
  *ip = *dip;
}

فراغ
rsect(uint sec, فراغ *buf)
{
  إذا (lseek(fsfd, sec * BSIZE, 0) != sec * BSIZE)
    die("lseek");
  إذا (اقرأ(fsfd, buf, BSIZE) != BSIZE)
    die("read");
}

uint
ialloc(ushort type)
{
  uint inum = freeinode++;
  بنية dinode din;

  bzero(&din, حجم(din));
  din.type = xshort(type);
  din.nlink = xshort(1);
  din.سائيز = xint(0);
  winode(inum, &din);
  أرجع inum;
}

فراغ
balloc(صحيح استعمال_ٿيل_2)
{
  uchar buf[BSIZE];
  صحيح i;

  لکو("balloc: first %d blocks have been allocated\n", استعمال_ٿيل_2);
  أكد(استعمال_ٿيل_2 < BPB);
  bzero(buf, BSIZE);
  لكل (i = 0; i < استعمال_ٿيل_2; i++) {
    buf[i / 8] = buf[i / 8] | (0x1 << (i % 8));
  }
  لکو("balloc: write bitmap block at sector %d\n", sb.bmapstart);
  wsect(sb.bmapstart, buf);
}

#وصف_ڪريو min(a, b) ((a) < (b) ? (a) : (b))

فراغ
iappend(uint inum, فراغ *xp, صحيح n)
{
  حرف *p = (حرف *)xp;
  uint fbn, off, n1;
  بنية dinode din;
  حرف buf[BSIZE];
  uint اڻسڌو[NINDIRECT];
  uint x;

  rinode(inum, &din);
  off = xint(din.سائيز);
  // printf("append inum %d at off %d sz %d\n", inum, off, n);
  طالما (n > 0) {
    fbn = off / BSIZE;
    أكد(fbn < MAXFILE);
    إذا (fbn < NDIRECT) {
      إذا (xint(din.addrs[fbn]) == 0) {
        din.addrs[fbn] = xint(freeblock++);
      }
      x = xint(din.addrs[fbn]);
    } وإلا {
      إذا (xint(din.addrs[NDIRECT]) == 0) {
        din.addrs[NDIRECT] = xint(freeblock++);
      }
      rsect(xint(din.addrs[NDIRECT]), (حرف *)اڻسڌو);
      إذا (اڻسڌو[fbn - NDIRECT] == 0) {
        اڻسڌو[fbn - NDIRECT] = xint(freeblock++);
        wsect(xint(din.addrs[NDIRECT]), (حرف *)اڻسڌو);
      }
      x = xint(اڻسڌو[fbn - NDIRECT]);
    }
    n1 = min(n, (fbn + 1) * BSIZE - off);
    rsect(x, buf);
    bcopy(p, buf + off - (fbn * BSIZE), n1);
    wsect(x, buf);
    n -= n1;
    off += n1;
    p += n1;
  }
  din.سائيز = xint(off);
  winode(inum, &din);
}

فراغ
die(ثابت حرف *s)
{
  perror(s);
  ٻاهر_نڪرو(1);
}
