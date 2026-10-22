بنية file {
  تعداد { FD_NONE, FD_PIPE, FD_INODE, FD_DEVICE } type;
  صحيح ref; // reference count
  حرف readable;
  حرف writable;
  بنية pipe *pipe; // FD_PIPE
  بنية inode *ip;  // FD_INODE and FD_DEVICE
  uint off;          // FD_INODE
  short major;       // FD_DEVICE
};

#define major(dev)  ((dev) >> 16 & 0xFFFF)
#define minor(dev)  ((dev) & 0xFFFF)
#define mkdev(m, n) ((uint)((m) << 16 | (n)))

// in-memory copy of an inode
بنية inode {
  uint dev;              // Device number
  uint inum;             // Inode number
  صحيح ref;               // Reference count
  بنية sleeplock lock; // protects everything below here
  صحيح valid;             // inode has been read from disk?

  short type; // copy of disk inode
  short major;
  short minor;
  short nlink;
  uint size;
  uint addrs[NDIRECT + 1];
};

// map major device number to device functions.
بنية devsw {
  صحيح (*اقرأ)(صحيح, uint64, صحيح);
  صحيح (*اكتب)(صحيح, uint64, صحيح);
};

خارجي بنية devsw devsw[];

#define CONSOLE 1
