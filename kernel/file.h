sa_nracanaa file {
  ga_nanaa { FD_NONE, FD_PIPE, FD_INODE, FD_DEVICE } type;
  ຈຳນວນເຕັມ ref; // reference count
  var_na readable;
  var_na writable;
  sa_nracanaa pipe *pipe; // FD_PIPE
  sa_nracanaa inode *ip;  // FD_INODE and FD_DEVICE
  uint off;          // FD_INODE
  short major;       // FD_DEVICE
};

#define major(dev)  ((dev) >> 16 & 0xFFFF)
#define minor(dev)  ((dev) & 0xFFFF)
#define mkdev(m, n) ((uint)((m) << 16 | (n)))

// in-memory copy of an inode
sa_nracanaa inode {
  uint dev;              // Device number
  uint inum;             // Inode number
  ຈຳນວນເຕັມ ref;               // Reference count
  sa_nracanaa sleeplock lock; // protects everything below here
  ຈຳນວນເຕັມ valid;             // inode has been read from disk?

  short type; // copy of disk inode
  short major;
  short minor;
  short nlink;
  uint size;
  uint addrs[NDIRECT + 1];
};

// map major device number to device functions.
sa_nracanaa devsw {
  ຈຳນວນເຕັມ (*read)(ຈຳນວນເຕັມ, uint64, ຈຳນວນເຕັມ);
  ຈຳນວນເຕັມ (*write)(ຈຳນວນເຕັມ, uint64, ຈຳນວນເຕັມ);
};

extern sa_nracanaa devsw devsw[];

#define CONSOLE 1
