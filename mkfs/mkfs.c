#شامل <stdio.h>
#شامل <unistd.h>
#شامل <stdlib.h>
#شامل <string.h>
#شامل <fcntl.h>
#شامل <assert.h>

#تعریف_کرو stat xv6_stat // avoid clash with host struct stat
#شامل "kernel/types.h"
#شامل "kernel/fs.h"
#شامل "kernel/stat.h"
#شامل "kernel/param.h"

#اگر_غیر_تعریف_شدہ static_assert
#تعریف_کرو static_assert(a, b)                                                    \
  کرو {                                                                         \
    انتخاب (0)                                                                 \
    حالت 0:                                                                    \
    حالت (a):;                                                                 \
  } جب (0)
#اگر_ختم

#تعریف_کرو NINODES 200

// Disk layout:
// [ boot block | sb block | log | inode blocks | free bit map | data blocks ]

صحیح nbitmap = FSSIZE / BPB + 1;
صحیح ninodeblocks = NINODES / IPB + 1;
صحیح nlog = LOGBLOCKS + 1; // Header followed by LOGBLOCKS data blocks.
صحیح nmeta;   // Number of meta blocks (boot, sb, nlog, inode, bitmap)
صحیح nblocks; // Number of data blocks

صحیح fsfd;
ساخت superblock sb;
حرف zeroes[BSIZE];
uint freeinode = 1;
uint freeblock;

خالی balloc(صحیح);
خالی wsect(uint, خالی *);
خالی winode(uint, ساخت dinode *);
خالی rinode(uint inum, ساخت dinode *ip);
خالی rsect(uint sec, خالی *buf);
uint ialloc(ushort type);
خالی iappend(uint inum, خالی *p, صحیح n);
خالی die(ثابت حرف *);

// convert to riscv byte order
ushort
xshort(ushort x)
{
  ushort y;
  uchar *a = (uchar *)&y;
  a[0] = x;
  a[1] = x >> 8;
  واپس y;
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
  واپس y;
}

صحیح
مین(صحیح argc, حرف *argv[])
{
  صحیح i, cc, fd;
  uint rootino, inum, off;
  ساخت dirent de;
  حرف buf[BSIZE];
  ساخت dinode din;

  static_assert(حجم(صحیح) == 4, "Integers must be 4 bytes!");

  اگر (argc < 2) {
    fprintf(stderr, "Usage: mkfs fs.img files...\n");
    باہر_نکلو(1);
  }

  أكد((BSIZE % حجم(ساخت dinode)) == 0);
  أكد((BSIZE % حجم(ساخت dirent)) == 0);

  fsfd = open(argv[1], O_RDWR | O_CREAT | O_TRUNC, 0666);
  اگر (fsfd < 0)
    die(argv[1]);

  // 1 fs block = 1 disk sector
  nmeta = 2 + nlog + ninodeblocks + nbitmap;
  nblocks = FSSIZE - nmeta;

  sb.magic = FSMAGIC;
  sb.سائز = xint(FSSIZE);
  sb.nblocks = xint(nblocks);
  sb.ninodes = xint(NINODES);
  sb.nlog = xint(nlog);
  sb.logstart = xint(2);
  sb.inodestart = xint(2 + nlog);
  sb.bmapstart = xint(2 + nlog + ninodeblocks);

  لکھو(
    "nmeta %d (boot, super, log blocks %u, inode blocks %u, bitmap blocks %u) blocks %d total %d\n",
    nmeta, nlog, ninodeblocks, nbitmap, nblocks, FSSIZE);

  freeblock = nmeta; // the first free block that we can allocate

  برائے (i = 0; i < FSSIZE; i++)
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

  برائے (i = 2; i < argc; i++) {
    // get rid of "user/"
    حرف *shortname;
    اگر (لڑی_موازنہ(argv[i], "user/", 5) == 0)
      shortname = argv[i] + 5;
    ورنہ
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

    أكد(لڑی_لمبائی(shortname) <= DIRSIZ);

    inum = ialloc(T_FILE);

    bzero(&de, حجم(de));
    de.inum = xshort(inum);
    strncpy(de.name, shortname, DIRSIZ);
    iappend(rootino, &de, حجم(de));

    جب ((cc = اقرأ(fd, buf, حجم(buf))) > 0)
      iappend(inum, buf, cc);

    close(fd);
  }

  // fix size of root inode dir
  rinode(rootino, &din);
  off = xint(din.سائز);
  off = ((off / BSIZE) + 1) * BSIZE;
  din.سائز = xint(off);
  winode(rootino, &din);

  balloc(freeblock);

  باہر_نکلو(0);
}

خالی
wsect(uint sec, خالی *buf)
{
  اگر (lseek(fsfd, sec * BSIZE, 0) != sec * BSIZE)
    die("lseek");
  اگر (اكتب(fsfd, buf, BSIZE) != BSIZE)
    die("write");
}

خالی
winode(uint inum, ساخت dinode *ip)
{
  حرف buf[BSIZE];
  uint bn;
  ساخت dinode *dip;

  bn = IBLOCK(inum, sb);
  rsect(bn, buf);
  dip = ((ساخت dinode *)buf) + (inum % IPB);
  *dip = *ip;
  wsect(bn, buf);
}

خالی
rinode(uint inum, ساخت dinode *ip)
{
  حرف buf[BSIZE];
  uint bn;
  ساخت dinode *dip;

  bn = IBLOCK(inum, sb);
  rsect(bn, buf);
  dip = ((ساخت dinode *)buf) + (inum % IPB);
  *ip = *dip;
}

خالی
rsect(uint sec, خالی *buf)
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
  ساخت dinode din;

  bzero(&din, حجم(din));
  din.type = xshort(type);
  din.nlink = xshort(1);
  din.سائز = xint(0);
  winode(inum, &din);
  واپس inum;
}

خالی
balloc(صحیح مستعمل_2)
{
  uchar buf[BSIZE];
  صحیح i;

  لکھو("balloc: first %d blocks have been allocated\n", مستعمل_2);
  أكد(مستعمل_2 < BPB);
  bzero(buf, BSIZE);
  برائے (i = 0; i < مستعمل_2; i++) {
    buf[i / 8] = buf[i / 8] | (0x1 << (i % 8));
  }
  لکھو("balloc: write bitmap block at sector %d\n", sb.bmapstart);
  wsect(sb.bmapstart, buf);
}

#تعریف_کرو min(a, b) ((a) < (b) ? (a) : (b))

خالی
iappend(uint inum, خالی *xp, صحیح n)
{
  حرف *p = (حرف *)xp;
  uint fbn, off, n1;
  ساخت dinode din;
  حرف buf[BSIZE];
  uint بالواسطہ[NINDIRECT];
  uint x;

  rinode(inum, &din);
  off = xint(din.سائز);
  // printf("append inum %d at off %d sz %d\n", inum, off, n);
  جب (n > 0) {
    fbn = off / BSIZE;
    أكد(fbn < MAXFILE);
    اگر (fbn < NDIRECT) {
      اگر (xint(din.addrs[fbn]) == 0) {
        din.addrs[fbn] = xint(freeblock++);
      }
      x = xint(din.addrs[fbn]);
    } ورنہ {
      اگر (xint(din.addrs[NDIRECT]) == 0) {
        din.addrs[NDIRECT] = xint(freeblock++);
      }
      rsect(xint(din.addrs[NDIRECT]), (حرف *)بالواسطہ);
      اگر (بالواسطہ[fbn - NDIRECT] == 0) {
        بالواسطہ[fbn - NDIRECT] = xint(freeblock++);
        wsect(xint(din.addrs[NDIRECT]), (حرف *)بالواسطہ);
      }
      x = xint(بالواسطہ[fbn - NDIRECT]);
    }
    n1 = min(n, (fbn + 1) * BSIZE - off);
    rsect(x, buf);
    bcopy(p, buf + off - (fbn * BSIZE), n1);
    wsect(x, buf);
    n -= n1;
    off += n1;
    p += n1;
  }
  din.سائز = xint(off);
  winode(inum, &din);
}

خالی
die(ثابت حرف *s)
{
  perror(s);
  باہر_نکلو(1);
}
