கட்டமைப்பு file {
  எண்ணிக்கை { FD_NONE, FD_PIPE, FD_INODE, FD_DEVICE } type;
  முழு ref; // reference count
  எழுத்து readable;
  எழுத்து writable;
  கட்டமைப்பு pipe *pipe; // FD_PIPE
  கட்டமைப்பு inode *ip;  // FD_INODE and FD_DEVICE
  uint off;          // FD_INODE
  short major;       // FD_DEVICE
};

#define major(dev)  ((dev) >> 16 & 0xFFFF)
#define minor(dev)  ((dev) & 0xFFFF)
#define mkdev(m, n) ((uint)((m) << 16 | (n)))

// in-memory copy of an inode
கட்டமைப்பு inode {
  uint dev;              // Device number
  uint inum;             // Inode number
  முழு ref;               // Reference count
  கட்டமைப்பு sleeplock lock; // protects everything below here
  முழு valid;             // inode has been read from disk?

  short type; // copy of disk inode
  short major;
  short minor;
  short nlink;
  uint size;
  uint addrs[NDIRECT + 1];
};

// map major device number to device functions.
கட்டமைப்பு devsw {
  முழு (*பட)(முழு, uint64, முழு);
  முழு (*லிக)(முழு, uint64, முழு);
};

வெளி கட்டமைப்பு devsw devsw[];

#define CONSOLE 1
