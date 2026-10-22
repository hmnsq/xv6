#समावेश <stdio.h>
#समावेश <unistd.h>
#समावेश <stdlib.h>
#समावेश <string.h>
#समावेश <fcntl.h>
#समावेश <assert.h>

#घोषणा stat xv6_stat // avoid clash with host struct stat
#समावेश "kernel/types.h"
#समावेश "kernel/fs.h"
#समावेश "kernel/stat.h"
#समावेश "kernel/param.h"

#अगर_अघोषित static_assert
#घोषणा static_assert(a, b)                                                    \
  करो {                                                                         \
    चयन (0)                                                                 \
    हाल 0:                                                                    \
    हाल (a):;                                                                 \
  } जबतक (0)
#पूर्ण_अगर

#घोषणा NINODES 200

// Disk layout:
// [ boot block | sb block | log | inode blocks | free bit map | data blocks ]

पूर्णांक nbitmap = FSSIZE / BPB + 1;
पूर्णांक ninodeblocks = NINODES / IPB + 1;
पूर्णांक nlog = LOGBLOCKS + 1; // Header followed by LOGBLOCKS data blocks.
पूर्णांक nmeta;   // Number of meta blocks (boot, sb, nlog, inode, bitmap)
पूर्णांक nblocks; // Number of data blocks

पूर्णांक fsfd;
काष्ठा superblock sb;
अक्षर zeroes[BSIZE];
uint freeinode = 1;
uint freeblock;

व्योम balloc(पूर्णांक);
व्योम wsect(uint, व्योम *);
व्योम winode(uint, काष्ठा dinode *);
व्योम rinode(uint inum, काष्ठा dinode *ip);
व्योम rsect(uint sec, व्योम *buf);
uint ialloc(ushort type);
व्योम iappend(uint inum, व्योम *p, पूर्णांक n);
व्योम die(स्थिर अक्षर *);

// convert to riscv byte order
ushort
xshort(ushort x)
{
  ushort y;
  uchar *a = (uchar *)&y;
  a[0] = x;
  a[1] = x >> 8;
  वापस y;
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
  वापस y;
}

पूर्णांक
मुख्य(पूर्णांक argc, अक्षर *argv[])
{
  पूर्णांक i, cc, fd;
  uint rootino, inum, off;
  काष्ठा dirent de;
  अक्षर buf[BSIZE];
  काष्ठा dinode din;

  static_assert(माप(पूर्णांक) == 4, "Integers must be 4 bytes!");

  अगर (argc < 2) {
    ख_लिखो(मानक_त्रुटि, "Usage: mkfs fs.img files...\n");
    निकास(1);
  }

  निश्चित((BSIZE % माप(काष्ठा dinode)) == 0);
  निश्चित((BSIZE % माप(काष्ठा dirent)) == 0);

  fsfd = खोलो(argv[1], O_RDWR | O_CREAT | O_TRUNC, 0666);
  अगर (fsfd < 0)
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

  म_लिखो(
    "nmeta %d (boot, super, log blocks %u, inode blocks %u, bitmap blocks %u) blocks %d total %d\n",
    nmeta, nlog, ninodeblocks, nbitmap, nblocks, FSSIZE);

  freeblock = nmeta; // the first free block that we can allocate

  क्रम (i = 0; i < FSSIZE; i++)
    wsect(i, zeroes);

  स_रखो(buf, 0, माप(buf));
  स_हटाओ(buf, &sb, माप(sb));
  wsect(1, buf);

  rootino = ialloc(T_DIR);
  निश्चित(rootino == ROOTINO);

  bzero(&de, माप(de));
  de.inum = xshort(rootino);
  म_नकल(de.name, ".");
  iappend(rootino, &de, माप(de));

  bzero(&de, माप(de));
  de.inum = xshort(rootino);
  म_नकल(de.name, "..");
  iappend(rootino, &de, माप(de));

  क्रम (i = 2; i < argc; i++) {
    // get rid of "user/"
    अक्षर *shortname;
    अगर (म_भेदन(argv[i], "user/", 5) == 0)
      shortname = argv[i] + 5;
    अन्यथा
      shortname = argv[i];

    निश्चित(index(shortname, '/') == 0);

    अगर ((fd = खोलो(argv[i], 0)) < 0)
      die(argv[i]);

    // Skip leading _ in name when writing to file system.
    // The binaries are named _rm, _cat, etc. to keep the
    // build operating system from trying to execute them
    // in place of system binaries like rm and cat.
    अगर (shortname[0] == '_')
      shortname += 1;

    निश्चित(म_माप(shortname) <= DIRSIZ);

    inum = ialloc(T_FILE);

    bzero(&de, माप(de));
    de.inum = xshort(inum);
    म_नकलन(de.name, shortname, DIRSIZ);
    iappend(rootino, &de, माप(de));

    जबतक ((cc = पढ़ो(fd, buf, माप(buf))) > 0)
      iappend(inum, buf, cc);

    बंद(fd);
  }

  // fix size of root inode dir
  rinode(rootino, &din);
  off = xint(din.आकार);
  off = ((off / BSIZE) + 1) * BSIZE;
  din.आकार = xint(off);
  winode(rootino, &din);

  balloc(freeblock);

  निकास(0);
}

व्योम
wsect(uint sec, व्योम *buf)
{
  अगर (lseek(fsfd, sec * BSIZE, 0) != sec * BSIZE)
    die("lseek");
  अगर (ग_लिखो(fsfd, buf, BSIZE) != BSIZE)
    die("write");
}

व्योम
winode(uint inum, काष्ठा dinode *ip)
{
  अक्षर buf[BSIZE];
  uint bn;
  काष्ठा dinode *dip;

  bn = IBLOCK(inum, sb);
  rsect(bn, buf);
  dip = ((काष्ठा dinode *)buf) + (inum % IPB);
  *dip = *ip;
  wsect(bn, buf);
}

व्योम
rinode(uint inum, काष्ठा dinode *ip)
{
  अक्षर buf[BSIZE];
  uint bn;
  काष्ठा dinode *dip;

  bn = IBLOCK(inum, sb);
  rsect(bn, buf);
  dip = ((काष्ठा dinode *)buf) + (inum % IPB);
  *ip = *dip;
}

व्योम
rsect(uint sec, व्योम *buf)
{
  अगर (lseek(fsfd, sec * BSIZE, 0) != sec * BSIZE)
    die("lseek");
  अगर (पढ़ो(fsfd, buf, BSIZE) != BSIZE)
    die("read");
}

uint
ialloc(ushort type)
{
  uint inum = freeinode++;
  काष्ठा dinode din;

  bzero(&din, माप(din));
  din.type = xshort(type);
  din.nlink = xshort(1);
  din.आकार = xint(0);
  winode(inum, &din);
  वापस inum;
}

व्योम
balloc(पूर्णांक used)
{
  uchar buf[BSIZE];
  पूर्णांक i;

  म_लिखो("balloc: first %d blocks have been allocated\n", used);
  निश्चित(used < BPB);
  bzero(buf, BSIZE);
  क्रम (i = 0; i < used; i++) {
    buf[i / 8] = buf[i / 8] | (0x1 << (i % 8));
  }
  म_लिखो("balloc: write bitmap block at sector %d\n", sb.bmapstart);
  wsect(sb.bmapstart, buf);
}

#घोषणा min(a, b) ((a) < (b) ? (a) : (b))

व्योम
iappend(uint inum, व्योम *xp, पूर्णांक n)
{
  अक्षर *p = (अक्षर *)xp;
  uint fbn, off, n1;
  काष्ठा dinode din;
  अक्षर buf[BSIZE];
  uint indirect[NINDIRECT];
  uint x;

  rinode(inum, &din);
  off = xint(din.आकार);
  // printf("append inum %d at off %d sz %d\n", inum, off, n);
  जबतक (n > 0) {
    fbn = off / BSIZE;
    निश्चित(fbn < MAXFILE);
    अगर (fbn < NDIRECT) {
      अगर (xint(din.addrs[fbn]) == 0) {
        din.addrs[fbn] = xint(freeblock++);
      }
      x = xint(din.addrs[fbn]);
    } अन्यथा {
      अगर (xint(din.addrs[NDIRECT]) == 0) {
        din.addrs[NDIRECT] = xint(freeblock++);
      }
      rsect(xint(din.addrs[NDIRECT]), (अक्षर *)indirect);
      अगर (indirect[fbn - NDIRECT] == 0) {
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
  din.आकार = xint(off);
  winode(inum, &din);
}

व्योम
die(स्थिर अक्षर *s)
{
  लिखो_त्रुटि(s);
  निकास(1);
}
