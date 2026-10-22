સંરચના file {
  ગણના { FD_NONE, FD_PIPE, FD_INODE, FD_DEVICE } type;
  પૂર્ણાંક ref; // reference count
  અક્ષર readable;
  અક્ષર writable;
  સંરચના pipe *pipe; // FD_PIPE
  સંરચના inode *ip;  // FD_INODE and FD_DEVICE
  uint off;          // FD_INODE
  short major;       // FD_DEVICE
};

#define major(dev)  ((dev) >> 16 & 0xFFFF)
#define minor(dev)  ((dev) & 0xFFFF)
#define mkdev(m, n) ((uint)((m) << 16 | (n)))

// in-memory copy of an inode
સંરચના inode {
  uint dev;              // Device number
  uint inum;             // Inode number
  પૂર્ણાંક ref;               // Reference count
  સંરચના sleeplock lock; // protects everything below here
  પૂર્ણાંક valid;             // inode has been read from disk?

  short type; // copy of disk inode
  short major;
  short minor;
  short nlink;
  uint size;
  uint addrs[NDIRECT + 1];
};

// map major device number to device functions.
સંરચના devsw {
  પૂર્ણાંક (*પઠ)(પૂર્ણાંક, uint64, પૂર્ણાંક);
  પૂર્ણાંક (*લિખ)(પૂર્ણાંક, uint64, પૂર્ણાંક);
};

બાહ્ય સંરચના devsw devsw[];

#define CONSOLE 1
