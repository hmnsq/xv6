#अन्तर्भाव <stdio.h>
#अन्तर्भाव <unistd.h>
#अन्तर्भाव <stdlib.h>
#अन्तर्भाव <string.h>
#अन्तर्भाव <fcntl.h>
#अन्तर्भाव <assert.h>

#परिभाषय stat xv6_stat // avoid clash with host struct stat
#अन्तर्भाव "kernel/types.h"
#अन्तर्भाव "kernel/fs.h"
#अन्तर्भाव "kernel/stat.h"
#अन्तर्भाव "kernel/param.h"

#यदि_अपरिभाषितम् static_assert
#परिभाषय static_assert(a, b)                                                    \
  कुरु {                                                                         \
    विकल्प (0)                                                                 \
    पक्ष 0:                                                                    \
    पक्ष (a):;                                                                 \
  } यावत् (0)
#यदि_अन्तः

#परिभाषय NINODES 200

// Disk layout:
// [ boot block | sb block | log | inode blocks | free bit map | data blocks ]

पूर्णाङ्क nbitmap = FSSIZE / BPB + 1;
पूर्णाङ्क ninodeblocks = NINODES / IPB + 1;
पूर्णाङ्क nlog = LOGBLOCKS + 1; // Header followed by LOGBLOCKS data blocks.
पूर्णाङ्क nmeta;   // Number of meta blocks (boot, sb, nlog, inode, bitmap)
पूर्णाङ्क nblocks; // Number of data blocks

पूर्णाङ्क fsfd;
संरचना superblock sb;
वर्ण zeroes[BSIZE];
uint freeinode = 1;
uint freeblock;

शून्य balloc(पूर्णाङ्क);
शून्य wsect(uint, शून्य *);
शून्य winode(uint, संरचना dinode *);
शून्य rinode(uint inum, संरचना dinode *ip);
शून्य rsect(uint sec, शून्य *buf);
uint ialloc(ushort type);
शून्य iappend(uint inum, शून्य *p, पूर्णाङ्क n);
शून्य die(ध्रुव वर्ण *);

// convert to riscv byte order
ushort
xshort(ushort x)
{
  ushort y;
  uchar *a = (uchar *)&y;
  a[0] = x;
  a[1] = x >> 8;
  प्रत्यागम y;
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
  प्रत्यागम y;
}

पूर्णाङ्क
मुख्यम्(पूर्णाङ्क argc, वर्ण *argv[])
{
  पूर्णाङ्क i, cc, fd;
  uint rootino, inum, off;
  संरचना dirent de;
  वर्ण buf[BSIZE];
  संरचना dinode din;

  static_assert(परिमाण(पूर्णाङ्क) == 4, "Integers must be 4 bytes!");

  यदि (argc < 2) {
    fprintf(stderr, "Usage: mkfs fs.img files...\n");
    निर्गच्छ(1);
  }

  प्रतिज्ञा((BSIZE % परिमाण(संरचना dinode)) == 0);
  प्रतिज्ञा((BSIZE % परिमाण(संरचना dirent)) == 0);

  fsfd = उद्घाटय(argv[1], O_RDWR | O_CREAT | O_TRUNC, 0666);
  यदि (fsfd < 0)
    die(argv[1]);

  // 1 fs block = 1 disk sector
  nmeta = 2 + nlog + ninodeblocks + nbitmap;
  nblocks = FSSIZE - nmeta;

  sb.magic = FSMAGIC;
  sb.आकार = xint(FSSIZE);
  sb.nblocks = xint(nblocks);
  sb.ninodes = xint(NINODES);
  sb.nlog = xint(nlog);
  sb.logstart = xint(2);
  sb.inodestart = xint(2 + nlog);
  sb.bmapstart = xint(2 + nlog + ninodeblocks);

  प्रारूप_लिख(
    "nmeta %d (boot, super, log blocks %u, inode blocks %u, bitmap blocks %u) blocks %d total %d\n",
    nmeta, nlog, ninodeblocks, nbitmap, nblocks, FSSIZE);

  freeblock = nmeta; // the first free block that we can allocate

  प्रति (i = 0; i < FSSIZE; i++)
    wsect(i, zeroes);

  स्मृतिं_पूरय(buf, 0, परिमाण(buf));
  memmove(buf, &sb, परिमाण(sb));
  wsect(1, buf);

  rootino = ialloc(T_DIR);
  प्रतिज्ञा(rootino == ROOTINO);

  bzero(&de, परिमाण(de));
  de.inum = xshort(rootino);
  वाक्यं_प्रतिलिख(de.name, ".");
  iappend(rootino, &de, परिमाण(de));

  bzero(&de, परिमाण(de));
  de.inum = xshort(rootino);
  वाक्यं_प्रतिलिख(de.name, "..");
  iappend(rootino, &de, परिमाण(de));

  प्रति (i = 2; i < argc; i++) {
    // get rid of "user/"
    वर्ण *shortname;
    यदि (वाक्ये_तुलय(argv[i], "user/", 5) == 0)
      shortname = argv[i] + 5;
    अन्यथा
      shortname = argv[i];

    प्रतिज्ञा(index(shortname, '/') == 0);

    यदि ((fd = उद्घाटय(argv[i], 0)) < 0)
      die(argv[i]);

    // Skip leading _ in name when writing to file system.
    // The binaries are named _rm, _cat, etc. to keep the
    // build operating system from trying to execute them
    // in place of system binaries like rm and cat.
    यदि (shortname[0] == '_')
      shortname += 1;

    प्रतिज्ञा(वाक्यदैर्घ्यम्(shortname) <= DIRSIZ);

    inum = ialloc(T_FILE);

    bzero(&de, परिमाण(de));
    de.inum = xshort(inum);
    strncpy(de.name, shortname, DIRSIZ);
    iappend(rootino, &de, परिमाण(de));

    यावत् ((cc = पठ(fd, buf, परिमाण(buf))) > 0)
      iappend(inum, buf, cc);

    close(fd);
  }

  // fix size of root inode dir
  rinode(rootino, &din);
  off = xint(din.आकार);
  off = ((off / BSIZE) + 1) * BSIZE;
  din.आकार = xint(off);
  winode(rootino, &din);

  balloc(freeblock);

  निर्गच्छ(0);
}

शून्य
wsect(uint sec, शून्य *buf)
{
  यदि (lseek(fsfd, sec * BSIZE, 0) != sec * BSIZE)
    die("lseek");
  यदि (लिख(fsfd, buf, BSIZE) != BSIZE)
    die("write");
}

शून्य
winode(uint inum, संरचना dinode *ip)
{
  वर्ण buf[BSIZE];
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
  वर्ण buf[BSIZE];
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
  यदि (lseek(fsfd, sec * BSIZE, 0) != sec * BSIZE)
    die("lseek");
  यदि (पठ(fsfd, buf, BSIZE) != BSIZE)
    die("read");
}

uint
ialloc(ushort type)
{
  uint inum = freeinode++;
  संरचना dinode din;

  bzero(&din, परिमाण(din));
  din.type = xshort(type);
  din.nlink = xshort(1);
  din.आकार = xint(0);
  winode(inum, &din);
  प्रत्यागम inum;
}

शून्य
balloc(पूर्णाङ्क प्रयुक्तम्_2)
{
  uchar buf[BSIZE];
  पूर्णाङ्क i;

  प्रारूप_लिख("balloc: first %d blocks have been allocated\n", प्रयुक्तम्_2);
  प्रतिज्ञा(प्रयुक्तम्_2 < BPB);
  bzero(buf, BSIZE);
  प्रति (i = 0; i < प्रयुक्तम्_2; i++) {
    buf[i / 8] = buf[i / 8] | (0x1 << (i % 8));
  }
  प्रारूप_लिख("balloc: write bitmap block at sector %d\n", sb.bmapstart);
  wsect(sb.bmapstart, buf);
}

#परिभाषय min(a, b) ((a) < (b) ? (a) : (b))

शून्य
iappend(uint inum, शून्य *xp, पूर्णाङ्क n)
{
  वर्ण *p = (वर्ण *)xp;
  uint fbn, off, n1;
  संरचना dinode din;
  वर्ण buf[BSIZE];
  uint परोक्षम्[NINDIRECT];
  uint x;

  rinode(inum, &din);
  off = xint(din.आकार);
  // printf("append inum %d at off %d sz %d\n", inum, off, n);
  यावत् (n > 0) {
    fbn = off / BSIZE;
    प्रतिज्ञा(fbn < MAXFILE);
    यदि (fbn < NDIRECT) {
      यदि (xint(din.addrs[fbn]) == 0) {
        din.addrs[fbn] = xint(freeblock++);
      }
      x = xint(din.addrs[fbn]);
    } अन्यथा {
      यदि (xint(din.addrs[NDIRECT]) == 0) {
        din.addrs[NDIRECT] = xint(freeblock++);
      }
      rsect(xint(din.addrs[NDIRECT]), (वर्ण *)परोक्षम्);
      यदि (परोक्षम्[fbn - NDIRECT] == 0) {
        परोक्षम्[fbn - NDIRECT] = xint(freeblock++);
        wsect(xint(din.addrs[NDIRECT]), (वर्ण *)परोक्षम्);
      }
      x = xint(परोक्षम्[fbn - NDIRECT]);
    }
    n1 = min(n, (fbn + 1) * BSIZE - off);
    rsect(x, buf);
    bcopy(p, buf + off - (fbn * BSIZE), n1);
    wsect(x, buf);
    n -= n1;
    off += n1;
    p += n1;
  }
  din.आकार = xint(off);
  winode(inum, &din);
}

शून्य
die(ध्रुव वर्ण *s)
{
  perror(s);
  निर्गच्छ(1);
}
