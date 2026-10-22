संरचना file {
  गणना { FD_NONE, FD_PIPE, FD_INODE, FD_DEVICE } type;
  पूर्णाङ्क ref; // reference count
  वर्ण readable;
  वर्ण writable;
  संरचना pipe *pipe; // FD_PIPE
  संरचना inode *ip;  // FD_INODE and FD_DEVICE
  uint off;          // FD_INODE
  short major;       // FD_DEVICE
};

#define major(dev)  ((dev) >> 16 & 0xFFFF)
#define minor(dev)  ((dev) & 0xFFFF)
#define mkdev(m, n) ((uint)((m) << 16 | (n)))

// in-memory copy of an inode
संरचना inode {
  uint dev;              // Device number
  uint inum;             // Inode number
  पूर्णाङ्क ref;               // Reference count
  संरचना sleeplock lock; // protects everything below here
  पूर्णाङ्क valid;             // inode has been read from disk?

  short type; // copy of disk inode
  short major;
  short minor;
  short nlink;
  uint size;
  uint addrs[NDIRECT + 1];
};

// map major device number to device functions.
संरचना devsw {
  पूर्णाङ्क (*पठ)(पूर्णाङ्क, uint64, पूर्णाङ्क);
  पूर्णाङ्क (*लिख)(पूर्णाङ्क, uint64, पूर्णाङ्क);
};

बाह्य संरचना devsw devsw[];

#define CONSOLE 1
