ഘടന file {
  എണ്ണൽ { FD_NONE, FD_PIPE, FD_INODE, FD_DEVICE } type;
  പൂർണ്ണസംഖ്യ ref; // reference count
  അക്ഷരം readable;
  അക്ഷരം writable;
  ഘടന pipe *pipe; // FD_PIPE
  ഘടന inode *ip;  // FD_INODE and FD_DEVICE
  uint off;          // FD_INODE
  short major;       // FD_DEVICE
};

#define major(dev)  ((dev) >> 16 & 0xFFFF)
#define minor(dev)  ((dev) & 0xFFFF)
#define mkdev(m, n) ((uint)((m) << 16 | (n)))

// in-memory copy of an inode
ഘടന inode {
  uint dev;              // Device number
  uint inum;             // Inode number
  പൂർണ്ണസംഖ്യ ref;               // Reference count
  ഘടന sleeplock lock; // protects everything below here
  പൂർണ്ണസംഖ്യ valid;             // inode has been read from disk?

  short type; // copy of disk inode
  short major;
  short minor;
  short nlink;
  uint size;
  uint addrs[NDIRECT + 1];
};

// map major device number to device functions.
ഘടന devsw {
  പൂർണ്ണസംഖ്യ (*പഠ)(പൂർണ്ണസംഖ്യ, uint64, പൂർണ്ണസംഖ്യ);
  പൂർണ്ണസംഖ്യ (*ലിഖ)(പൂർണ്ണസംഖ്യ, uint64, പൂർണ്ണസംഖ്യ);
};

ബാഹ്യ ഘടന devsw devsw[];

#define CONSOLE 1
