నిర్మాణం file {
  గణన { FD_NONE, FD_PIPE, FD_INODE, FD_DEVICE } type;
  పూర్ణాంకం ref; // reference count
  అక్షరం readable;
  అక్షరం writable;
  నిర్మాణం pipe *pipe; // FD_PIPE
  నిర్మాణం inode *ip;  // FD_INODE and FD_DEVICE
  uint off;          // FD_INODE
  పొట్టి major;       // FD_DEVICE
};

#నిర్వచించు major(dev)  ((dev) >> 16 & 0xFFFF)
#నిర్వచించు minor(dev)  ((dev) & 0xFFFF)
#నిర్వచించు mkdev(m, n) ((uint)((m) << 16 | (n)))

// in-memory copy of an inode
నిర్మాణం inode {
  uint dev;              // Device number
  uint inum;             // Inode number
  పూర్ణాంకం ref;               // Reference count
  నిర్మాణం sleeplock తాళం; // protects everything below here
  పూర్ణాంకం valid;             // inode has been read from disk?

  పొట్టి type; // copy of disk inode
  పొట్టి major;
  పొట్టి minor;
  పొట్టి nlink;
  uint కొలత;
  uint addrs[NDIRECT + 1];
};

// map major device number to device functions.
నిర్మాణం devsw {
  పూర్ణాంకం (*పఠ)(పూర్ణాంకం, uint64, పూర్ణాంకం);
  పూర్ణాంకం (*లిఖ)(పూర్ణాంకం, uint64, పూర్ణాంకం);
};

బాహ్య నిర్మాణం devsw devsw[];

#నిర్వచించు CONSOLE 1
