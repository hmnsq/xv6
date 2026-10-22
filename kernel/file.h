संरचना file {
  गणना { FD_NONE, FD_PIPE, FD_INODE, FD_DEVICE } type;
  पूर्णांक ref; // reference count
  अक्षर readable;
  अक्षर writable;
  संरचना pipe *pipe; // FD_PIPE
  संरचना inode *ip;  // FD_INODE and FD_DEVICE
  uint off;          // FD_INODE
  लघु major;       // FD_DEVICE
};

#define major(dev)  ((dev) >> 16 & 0xFFFF)
#define minor(dev)  ((dev) & 0xFFFF)
#define mkdev(m, n) ((uint)((m) << 16 | (n)))

// in-memory copy of an inode
संरचना inode {
  uint dev;              // Device number
  uint inum;             // Inode number
  पूर्णांक ref;               // Reference count
  संरचना sleeplock lock; // protects everything below here
  पूर्णांक valid;             // inode has been read from disk?

  लघु type; // copy of disk inode
  लघु major;
  लघु minor;
  लघु nlink;
  uint size;
  uint addrs[NDIRECT + 1];
};

// map major device number to device functions.
संरचना devsw {
  पूर्णांक (*पठ)(पूर्णांक, uint64, पूर्णांक);
  पूर्णांक (*लिख)(पूर्णांक, uint64, पूर्णांक);
};

बाह्य संरचना devsw devsw[];

#define CONSOLE 1
