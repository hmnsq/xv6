#அந்தர்பாவ <stdio.h>
#அந்தர்பாவ <unistd.h>
#அந்தர்பாவ <stdlib.h>
#அந்தர்பாவ <string.h>
#அந்தர்பாவ <fcntl.h>
#அந்தர்பாவ <assert.h>

#define stat xv6_stat // avoid clash with host struct stat
#அந்தர்பாவ "kernel/types.h"
#அந்தர்பாவ "kernel/fs.h"
#அந்தர்பாவ "kernel/stat.h"
#அந்தர்பாவ "kernel/param.h"

#ifndef static_assert
#define static_assert(a, b)                                                    \
  செய் {                                                                         \
    தேர்வு (0)                                                                 \
    நிலை 0:                                                                    \
    நிலை (a):;                                                                 \
  } போது (0)
#endif

#define NINODES 200

// Disk layout:
// [ boot block | sb block | log | inode blocks | free bit map | data blocks ]

முழு nbitmap = FSSIZE / BPB + 1;
முழு ninodeblocks = NINODES / IPB + 1;
முழு nlog = LOGBLOCKS + 1; // Header followed by LOGBLOCKS data blocks.
முழு nmeta;   // Number of meta blocks (boot, sb, nlog, inode, bitmap)
முழு nblocks; // Number of data blocks

முழு fsfd;
கட்டமைப்பு superblock sb;
எழுத்து zeroes[BSIZE];
uint freeinode = 1;
uint freeblock;

வெறுமை balloc(முழு);
வெறுமை wsect(uint, வெறுமை *);
வெறுமை winode(uint, கட்டமைப்பு dinode *);
வெறுமை rinode(uint inum, கட்டமைப்பு dinode *ip);
வெறுமை rsect(uint sec, வெறுமை *buf);
uint ialloc(ushort type);
வெறுமை iappend(uint inum, வெறுமை *p, முழு n);
வெறுமை die(மாறிலி எழுத்து *);

// convert to riscv byte order
ushort
xshort(ushort x)
{
  ushort y;
  uchar *a = (uchar *)&y;
  a[0] = x;
  a[1] = x >> 8;
  திரும்பு y;
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
  திரும்பு y;
}

முழு
முதன்மை(முழு argc, எழுத்து *argv[])
{
  முழு i, cc, fd;
  uint rootino, inum, off;
  கட்டமைப்பு dirent de;
  எழுத்து buf[BSIZE];
  கட்டமைப்பு dinode din;

  static_assert(அளவு(முழு) == 4, "Integers must be 4 bytes!");

  எனில் (argc < 2) {
    fprintf(stderr, "Usage: mkfs fs.img files...\n");
    exit(1);
  }

  ப்ரதிஜ்ஞா((BSIZE % அளவு(கட்டமைப்பு dinode)) == 0);
  ப்ரதிஜ்ஞா((BSIZE % அளவு(கட்டமைப்பு dirent)) == 0);

  fsfd = open(argv[1], O_RDWR | O_CREAT | O_TRUNC, 0666);
  எனில் (fsfd < 0)
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

  க்கு (i = 0; i < FSSIZE; i++)
    wsect(i, zeroes);

  memset(buf, 0, அளவு(buf));
  memmove(buf, &sb, அளவு(sb));
  wsect(1, buf);

  rootino = ialloc(T_DIR);
  ப்ரதிஜ்ஞா(rootino == ROOTINO);

  bzero(&de, அளவு(de));
  de.inum = xshort(rootino);
  strcpy(de.name, ".");
  iappend(rootino, &de, அளவு(de));

  bzero(&de, அளவு(de));
  de.inum = xshort(rootino);
  strcpy(de.name, "..");
  iappend(rootino, &de, அளவு(de));

  க்கு (i = 2; i < argc; i++) {
    // get rid of "user/"
    எழுத்து *shortname;
    எனில் (strncmp(argv[i], "user/", 5) == 0)
      shortname = argv[i] + 5;
    இல்லையெனில்
      shortname = argv[i];

    ப்ரதிஜ்ஞா(index(shortname, '/') == 0);

    எனில் ((fd = open(argv[i], 0)) < 0)
      die(argv[i]);

    // Skip leading _ in name when writing to file system.
    // The binaries are named _rm, _cat, etc. to keep the
    // build operating system from trying to execute them
    // in place of system binaries like rm and cat.
    எனில் (shortname[0] == '_')
      shortname += 1;

    ப்ரதிஜ்ஞா(strlen(shortname) <= DIRSIZ);

    inum = ialloc(T_FILE);

    bzero(&de, அளவு(de));
    de.inum = xshort(inum);
    strncpy(de.name, shortname, DIRSIZ);
    iappend(rootino, &de, அளவு(de));

    போது ((cc = பட(fd, buf, அளவு(buf))) > 0)
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

வெறுமை
wsect(uint sec, வெறுமை *buf)
{
  எனில் (lseek(fsfd, sec * BSIZE, 0) != sec * BSIZE)
    die("lseek");
  எனில் (லிக(fsfd, buf, BSIZE) != BSIZE)
    die("write");
}

வெறுமை
winode(uint inum, கட்டமைப்பு dinode *ip)
{
  எழுத்து buf[BSIZE];
  uint bn;
  கட்டமைப்பு dinode *dip;

  bn = IBLOCK(inum, sb);
  rsect(bn, buf);
  dip = ((கட்டமைப்பு dinode *)buf) + (inum % IPB);
  *dip = *ip;
  wsect(bn, buf);
}

வெறுமை
rinode(uint inum, கட்டமைப்பு dinode *ip)
{
  எழுத்து buf[BSIZE];
  uint bn;
  கட்டமைப்பு dinode *dip;

  bn = IBLOCK(inum, sb);
  rsect(bn, buf);
  dip = ((கட்டமைப்பு dinode *)buf) + (inum % IPB);
  *ip = *dip;
}

வெறுமை
rsect(uint sec, வெறுமை *buf)
{
  எனில் (lseek(fsfd, sec * BSIZE, 0) != sec * BSIZE)
    die("lseek");
  எனில் (பட(fsfd, buf, BSIZE) != BSIZE)
    die("read");
}

uint
ialloc(ushort type)
{
  uint inum = freeinode++;
  கட்டமைப்பு dinode din;

  bzero(&din, அளவு(din));
  din.type = xshort(type);
  din.nlink = xshort(1);
  din.size = xint(0);
  winode(inum, &din);
  திரும்பு inum;
}

வெறுமை
balloc(முழு used)
{
  uchar buf[BSIZE];
  முழு i;

  printf("balloc: first %d blocks have been allocated\n", used);
  ப்ரதிஜ்ஞா(used < BPB);
  bzero(buf, BSIZE);
  க்கு (i = 0; i < used; i++) {
    buf[i / 8] = buf[i / 8] | (0x1 << (i % 8));
  }
  printf("balloc: write bitmap block at sector %d\n", sb.bmapstart);
  wsect(sb.bmapstart, buf);
}

#define min(a, b) ((a) < (b) ? (a) : (b))

வெறுமை
iappend(uint inum, வெறுமை *xp, முழு n)
{
  எழுத்து *p = (எழுத்து *)xp;
  uint fbn, off, n1;
  கட்டமைப்பு dinode din;
  எழுத்து buf[BSIZE];
  uint indirect[NINDIRECT];
  uint x;

  rinode(inum, &din);
  off = xint(din.size);
  // printf("append inum %d at off %d sz %d\n", inum, off, n);
  போது (n > 0) {
    fbn = off / BSIZE;
    ப்ரதிஜ்ஞா(fbn < MAXFILE);
    எனில் (fbn < NDIRECT) {
      எனில் (xint(din.addrs[fbn]) == 0) {
        din.addrs[fbn] = xint(freeblock++);
      }
      x = xint(din.addrs[fbn]);
    } இல்லையெனில் {
      எனில் (xint(din.addrs[NDIRECT]) == 0) {
        din.addrs[NDIRECT] = xint(freeblock++);
      }
      rsect(xint(din.addrs[NDIRECT]), (எழுத்து *)indirect);
      எனில் (indirect[fbn - NDIRECT] == 0) {
        indirect[fbn - NDIRECT] = xint(freeblock++);
        wsect(xint(din.addrs[NDIRECT]), (எழுத்து *)indirect);
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

வெறுமை
die(மாறிலி எழுத்து *s)
{
  perror(s);
  exit(1);
}
