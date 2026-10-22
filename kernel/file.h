ಸಂರಚನಾ file {
  ಗಣನಾ { FD_NONE, FD_PIPE, FD_INODE, FD_DEVICE } type;
  ಪೂರ್ಣಾಙ್ಕ ref; // reference count
  ವರ್ಣ readable;
  ವರ್ಣ writable;
  ಸಂರಚನಾ pipe *pipe; // FD_PIPE
  ಸಂರಚನಾ inode *ip;  // FD_INODE and FD_DEVICE
  uint off;          // FD_INODE
  short major;       // FD_DEVICE
};

#define major(dev)  ((dev) >> 16 & 0xFFFF)
#define minor(dev)  ((dev) & 0xFFFF)
#define mkdev(m, n) ((uint)((m) << 16 | (n)))

// in-memory copy of an inode
ಸಂರಚನಾ inode {
  uint dev;              // Device number
  uint inum;             // Inode number
  ಪೂರ್ಣಾಙ್ಕ ref;               // Reference count
  ಸಂರಚನಾ sleeplock lock; // protects everything below here
  ಪೂರ್ಣಾಙ್ಕ valid;             // inode has been read from disk?

  short type; // copy of disk inode
  short major;
  short minor;
  short nlink;
  uint size;
  uint addrs[NDIRECT + 1];
};

// map major device number to device functions.
ಸಂರಚನಾ devsw {
  ಪೂರ್ಣಾಙ್ಕ (*ಪಠ)(ಪೂರ್ಣಾಙ್ಕ, uint64, ಪೂರ್ಣಾಙ್ಕ);
  ಪೂರ್ಣಾಙ್ಕ (*ಲಿಖ)(ಪೂರ್ಣಾಙ್ಕ, uint64, ಪೂರ್ಣಾಙ್ಕ);
};

ಬಾಹ್ಯ ಸಂರಚನಾ devsw devsw[];

#define CONSOLE 1
