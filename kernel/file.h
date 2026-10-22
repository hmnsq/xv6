কাঠামো file {
  গণনা { FD_NONE, FD_PIPE, FD_INODE, FD_DEVICE } type;
  পূর্ণসংখ্যা ref; // reference count
  অক্ষর readable;
  অক্ষর writable;
  কাঠামো pipe *pipe; // FD_PIPE
  কাঠামো inode *ip;  // FD_INODE and FD_DEVICE
  uint off;          // FD_INODE
  short major;       // FD_DEVICE
};

#define major(dev)  ((dev) >> 16 & 0xFFFF)
#define minor(dev)  ((dev) & 0xFFFF)
#define mkdev(m, n) ((uint)((m) << 16 | (n)))

// in-memory copy of an inode
কাঠামো inode {
  uint dev;              // Device number
  uint inum;             // Inode number
  পূর্ণসংখ্যা ref;               // Reference count
  কাঠামো sleeplock lock; // protects everything below here
  পূর্ণসংখ্যা valid;             // inode has been read from disk?

  short type; // copy of disk inode
  short major;
  short minor;
  short nlink;
  uint আকার_2;
  uint addrs[NDIRECT + 1];
};

// map major device number to device functions.
কাঠামো devsw {
  পূর্ণসংখ্যা (*পঠ)(পূর্ণসংখ্যা, uint64, পূর্ণসংখ্যা);
  পূর্ণসংখ্যা (*লিখ)(পূর্ণসংখ্যা, uint64, পূর্ণসংখ্যা);
};

বাহ্যিক কাঠামো devsw devsw[];

#define CONSOLE 1
