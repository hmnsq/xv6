সংরচনা file {
  গণনা { FD_NONE, FD_PIPE, FD_INODE, FD_DEVICE } type;
  পূর্ণাঙ্ক ref; // reference count
  বর্ণ readable;
  বর্ণ writable;
  সংরচনা pipe *pipe; // FD_PIPE
  সংরচনা inode *ip;  // FD_INODE and FD_DEVICE
  uint off;          // FD_INODE
  short major;       // FD_DEVICE
};

#define major(dev)  ((dev) >> 16 & 0xFFFF)
#define minor(dev)  ((dev) & 0xFFFF)
#define mkdev(m, n) ((uint)((m) << 16 | (n)))

// in-memory copy of an inode
সংরচনা inode {
  uint dev;              // Device number
  uint inum;             // Inode number
  পূর্ণাঙ্ক ref;               // Reference count
  সংরচনা sleeplock lock; // protects everything below here
  পূর্ণাঙ্ক valid;             // inode has been read from disk?

  short type; // copy of disk inode
  short major;
  short minor;
  short nlink;
  uint size;
  uint addrs[NDIRECT + 1];
};

// map major device number to device functions.
সংরচনা devsw {
  পূর্ণাঙ্ক (*পঠ)(পূর্ণাঙ্ক, uint64, পূর্ণাঙ্ক);
  পূর্ণাঙ্ক (*লিখ)(পূর্ণাঙ্ক, uint64, পূর্ণাঙ্ক);
};

বাহ্য সংরচনা devsw devsw[];

#define CONSOLE 1
