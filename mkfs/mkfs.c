#अन्तर्भाव <stdio.h>
#अन्तर्भाव <unistd.h>
#अन्तर्भाव <stdlib.h>
#अन्तर्भाव <string.h>
#अन्तर्भाव <fcntl.h>
#अन्तर्भाव <assert.h>

#define stat xv6_stat // avoid clash with host struct stat
#अन्तर्भाव "kernel/types.h"
#अन्तर्भाव "kernel/fs.h"
#अन्तर्भाव "kernel/stat.h"
#अन्तर्भाव "kernel/param.h"

#ifndef static_assert
#define static_assert(a, b)                                                    \
  करा {                                                                         \
    निवड (0)                                                                 \
    स्थिती 0:                                                                    \
    स्थिती (a):;                                                                 \
  } पर्यंत (0)
#endif

#define NINODES 200

// Disk layout:
// [ boot block | sb block | log | inode blocks | free bit map | data blocks ]

पूर्णांक nbitmap = FSSIZE / BPB + 1;
पूर्णांक ninodeblocks = NINODES / IPB + 1;
पूर्णांक nlog = LOGBLOCKS + 1; // Header followed by LOGBLOCKS data blocks.
पूर्णांक nmeta;   // Number of meta blocks (boot, sb, nlog, inode, bitmap)
पूर्णांक nblocks; // Number of data blocks

पूर्णांक fsfd;
संरचना superblock sb;
अक्षर zeroes[BSIZE];
uint freeinode = 1;
uint freeblock;

शून्य balloc(पूर्णांक);
शून्य wsect(uint, शून्य *);
शून्य winode(uint, संरचना dinode *);
शून्य rinode(uint inum, संरचना dinode *ip);
शून्य rsect(uint sec, शून्य *buf);
uint ialloc(ushort type);
शून्य iappend(uint inum, शून्य *p, पूर्णांक n);
शून्य die(स्थिरांक अक्षर *);

// convert to riscv byte order
ushort
xshort(ushort x)
{
  ushort y;
  uchar *a = (uchar *)&y;
  a[0] = x;
  a[1] = x >> 8;
  परत y;
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
  परत y;
}

पूर्णांक
main(पूर्णांक argc, अक्षर *argv[])
{
  पूर्णांक i, cc, fd;
  uint rootino, inum, off;
  संरचना dirent de;
  अक्षर buf[BSIZE];
  संरचना dinode din;

  static_assert(आकार(पूर्णांक) == 4, "Integers must be 4 bytes!");

  जर (argc < 2) {
    ख_लिखो(मानक_त्रुटि, "Usage: mkfs fs.img files...\n");
    निकास(1);
  }

  प्रतिज्ञा((BSIZE % आकार(संरचना dinode)) == 0);
  प्रतिज्ञा((BSIZE % आकार(संरचना dirent)) == 0);

  fsfd = खोलो(argv[1], O_RDWR | O_CREAT | O_TRUNC, 0666);
  जर (fsfd < 0)
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

  म_लिखो(
    "nmeta %d (boot, super, log blocks %u, inode blocks %u, bitmap blocks %u) blocks %d total %d\n",
    nmeta, nlog, ninodeblocks, nbitmap, nblocks, FSSIZE);

  freeblock = nmeta; // the first free block that we can allocate

  साठी (i = 0; i < FSSIZE; i++)
    wsect(i, zeroes);

  स_रखो(buf, 0, आकार(buf));
  स_हटाओ(buf, &sb, आकार(sb));
  wsect(1, buf);

  rootino = ialloc(T_DIR);
  प्रतिज्ञा(rootino == ROOTINO);

  bzero(&de, आकार(de));
  de.inum = xshort(rootino);
  म_नकल(de.name, ".");
  iappend(rootino, &de, आकार(de));

  bzero(&de, आकार(de));
  de.inum = xshort(rootino);
  म_नकल(de.name, "..");
  iappend(rootino, &de, आकार(de));

  साठी (i = 2; i < argc; i++) {
    // get rid of "user/"
    अक्षर *shortname;
    जर (म_भेदन(argv[i], "user/", 5) == 0)
      shortname = argv[i] + 5;
    अन्यथा
      shortname = argv[i];

    प्रतिज्ञा(index(shortname, '/') == 0);

    जर ((fd = खोलो(argv[i], 0)) < 0)
      die(argv[i]);

    // Skip leading _ in name when writing to file system.
    // The binaries are named _rm, _cat, etc. to keep the
    // build operating system from trying to execute them
    // in place of system binaries like rm and cat.
    जर (shortname[0] == '_')
      shortname += 1;

    प्रतिज्ञा(म_माप(shortname) <= DIRSIZ);

    inum = ialloc(T_FILE);

    bzero(&de, आकार(de));
    de.inum = xshort(inum);
    म_नकलन(de.name, shortname, DIRSIZ);
    iappend(rootino, &de, आकार(de));

    पर्यंत ((cc = पठ(fd, buf, आकार(buf))) > 0)
      iappend(inum, buf, cc);

    बंद(fd);
  }

  // fix size of root inode dir
  rinode(rootino, &din);
  off = xint(din.size);
  off = ((off / BSIZE) + 1) * BSIZE;
  din.size = xint(off);
  winode(rootino, &din);

  balloc(freeblock);

  निकास(0);
}

शून्य
wsect(uint sec, शून्य *buf)
{
  जर (lseek(fsfd, sec * BSIZE, 0) != sec * BSIZE)
    die("lseek");
  जर (लिख(fsfd, buf, BSIZE) != BSIZE)
    die("write");
}

शून्य
winode(uint inum, संरचना dinode *ip)
{
  अक्षर buf[BSIZE];
  uint bn;
  संरचना dinode *dip;

  bn = IBLOCK(inum, sb);
  rsect(bn, buf);
  dip = ((संरचना dinode *)buf) + (inum % IPB);
  *dip = *ip;
  wsect(bn, buf);
}

शून्य
rinode(uint inum, संरचना dinode *ip)
{
  अक्षर buf[BSIZE];
  uint bn;
  संरचना dinode *dip;

  bn = IBLOCK(inum, sb);
  rsect(bn, buf);
  dip = ((संरचना dinode *)buf) + (inum % IPB);
  *ip = *dip;
}

शून्य
rsect(uint sec, शून्य *buf)
{
  जर (lseek(fsfd, sec * BSIZE, 0) != sec * BSIZE)
    die("lseek");
  जर (पठ(fsfd, buf, BSIZE) != BSIZE)
    die("read");
}

uint
ialloc(ushort type)
{
  uint inum = freeinode++;
  संरचना dinode din;

  bzero(&din, आकार(din));
  din.type = xshort(type);
  din.nlink = xshort(1);
  din.size = xint(0);
  winode(inum, &din);
  परत inum;
}

शून्य
balloc(पूर्णांक used)
{
  uchar buf[BSIZE];
  पूर्णांक i;

  म_लिखो("balloc: first %d blocks have been allocated\n", used);
  प्रतिज्ञा(used < BPB);
  bzero(buf, BSIZE);
  साठी (i = 0; i < used; i++) {
    buf[i / 8] = buf[i / 8] | (0x1 << (i % 8));
  }
  म_लिखो("balloc: write bitmap block at sector %d\n", sb.bmapstart);
  wsect(sb.bmapstart, buf);
}

#define min(a, b) ((a) < (b) ? (a) : (b))

शून्य
iappend(uint inum, शून्य *xp, पूर्णांक n)
{
  अक्षर *p = (अक्षर *)xp;
  uint fbn, off, n1;
  संरचना dinode din;
  अक्षर buf[BSIZE];
  uint indirect[NINDIRECT];
  uint x;

  rinode(inum, &din);
  off = xint(din.size);
  // printf("append inum %d at off %d sz %d\n", inum, off, n);
  पर्यंत (n > 0) {
    fbn = off / BSIZE;
    प्रतिज्ञा(fbn < MAXFILE);
    जर (fbn < NDIRECT) {
      जर (xint(din.addrs[fbn]) == 0) {
        din.addrs[fbn] = xint(freeblock++);
      }
      x = xint(din.addrs[fbn]);
    } अन्यथा {
      जर (xint(din.addrs[NDIRECT]) == 0) {
        din.addrs[NDIRECT] = xint(freeblock++);
      }
      rsect(xint(din.addrs[NDIRECT]), (अक्षर *)indirect);
      जर (indirect[fbn - NDIRECT] == 0) {
        indirect[fbn - NDIRECT] = xint(freeblock++);
        wsect(xint(din.addrs[NDIRECT]), (अक्षर *)indirect);
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

शून्य
die(स्थिरांक अक्षर *s)
{
  लिखो_त्रुटि(s);
  निकास(1);
}
