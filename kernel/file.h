struct file {
  enum { FD_NONE, FD_PIPE, FD_INODE, FD_DEVICE } type;
  số_nguyên ref; // reference count
  char readable;
  char writable;
  struct pipe *pipe; // FD_PIPE
  struct inode *ip;  // FD_INODE and FD_DEVICE
  uint off;          // FD_INODE
  short major;       // FD_DEVICE
};

#define major(dev)  ((dev) >> 16 & 0xFFFF)
#define minor(dev)  ((dev) & 0xFFFF)
#define mkdev(m, n) ((uint)((m) << 16 | (n)))

// in-memory copy of an inode
struct inode {
  uint dev;              // Device number
  uint inum;             // Inode number
  số_nguyên ref;               // Reference count
  struct sleeplock lock; // protects everything below here
  số_nguyên valid;             // inode has been read from disk?

  short type; // copy of disk inode
  short major;
  short minor;
  short nlink;
  uint size;
  uint addrs[NDIRECT + 1];
};

// map major device number to device functions.
struct devsw {
  số_nguyên (*read)(số_nguyên, uint64, số_nguyên);
  số_nguyên (*write)(số_nguyên, uint64, số_nguyên);
};

extern struct devsw devsw[];

#define CONSOLE 1
