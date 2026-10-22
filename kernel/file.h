ساختار file {
  شمارش { FD_NONE, FD_PIPE, FD_INODE, FD_DEVICE } type;
  صحیح ref; // reference count
  نویسه readable;
  نویسه writable;
  ساختار pipe *pipe; // FD_PIPE
  ساختار inode *ip;  // FD_INODE and FD_DEVICE
  uint off;          // FD_INODE
  short major;       // FD_DEVICE
};

#define major(dev)  ((dev) >> 16 & 0xFFFF)
#define minor(dev)  ((dev) & 0xFFFF)
#define mkdev(m, n) ((uint)((m) << 16 | (n)))

// in-memory copy of an inode
ساختار inode {
  uint dev;              // Device number
  uint inum;             // Inode number
  صحیح ref;               // Reference count
  ساختار sleeplock lock; // protects everything below here
  صحیح valid;             // inode has been read from disk?

  short type; // copy of disk inode
  short major;
  short minor;
  short nlink;
  uint size;
  uint addrs[NDIRECT + 1];
};

// map major device number to device functions.
ساختار devsw {
  صحیح (*اقرأ)(صحیح, uint64, صحیح);
  صحیح (*اكتب)(صحیح, uint64, صحیح);
};

بیرونی ساختار devsw devsw[];

#define CONSOLE 1
