ساخت file {
  شمار { FD_NONE, FD_PIPE, FD_INODE, FD_DEVICE } type;
  صحیح ref; // reference count
  حرف readable;
  حرف writable;
  ساخت pipe *pipe; // FD_PIPE
  ساخت inode *ip;  // FD_INODE and FD_DEVICE
  uint off;          // FD_INODE
  مختصر major;       // FD_DEVICE
};

#تعریف_کرو major(dev)  ((dev) >> 16 & 0xFFFF)
#تعریف_کرو minor(dev)  ((dev) & 0xFFFF)
#تعریف_کرو mkdev(m, n) ((uint)((m) << 16 | (n)))

// in-memory copy of an inode
ساخت inode {
  uint dev;              // Device number
  uint inum;             // Inode number
  صحیح ref;               // Reference count
  ساخت sleeplock تالا; // protects everything below here
  صحیح valid;             // inode has been read from disk?

  مختصر type; // copy of disk inode
  مختصر major;
  مختصر minor;
  مختصر nlink;
  uint سائز;
  uint addrs[NDIRECT + 1];
};

// map major device number to device functions.
ساخت devsw {
  صحیح (*اقرأ)(صحیح, uint64, صحیح);
  صحیح (*اكتب)(صحیح, uint64, صحیح);
};

خارجی ساخت devsw devsw[];

#تعریف_کرو CONSOLE 1
