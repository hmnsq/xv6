ସଂରଚନା file {
  ଗଣନା { FD_NONE, FD_PIPE, FD_INODE, FD_DEVICE } type;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା ref; // reference count
  ଅକ୍ଷର readable;
  ଅକ୍ଷର writable;
  ସଂରଚନା pipe *pipe; // FD_PIPE
  ସଂରଚନା inode *ip;  // FD_INODE and FD_DEVICE
  uint off;          // FD_INODE
  short major;       // FD_DEVICE
};

#define major(dev)  ((dev) >> 16 & 0xFFFF)
#define minor(dev)  ((dev) & 0xFFFF)
#define mkdev(m, n) ((uint)((m) << 16 | (n)))

// in-memory copy of an inode
ସଂରଚନା inode {
  uint dev;              // Device number
  uint inum;             // Inode number
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା ref;               // Reference count
  ସଂରଚନା sleeplock lock; // protects everything below here
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା valid;             // inode has been read from disk?

  short type; // copy of disk inode
  short major;
  short minor;
  short nlink;
  uint size;
  uint addrs[NDIRECT + 1];
};

// map major device number to device functions.
ସଂରଚନା devsw {
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*ପଠ)(ପୂର୍ଣ୍ଣସଂଖ୍ୟା, uint64, ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*ଲିଖ)(ପୂର୍ଣ୍ଣସଂଖ୍ୟା, uint64, ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
};

ବାହ୍ୟ ସଂରଚନା devsw devsw[];

#define CONSOLE 1
