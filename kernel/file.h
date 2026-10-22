ರಚನೆ file {
  ಎಣಿಕೆ { FD_NONE, FD_PIPE, FD_INODE, FD_DEVICE } type;
  ಪೂರ್ಣಾಂಕ ref; // reference count
  ಅಕ್ಷರ readable;
  ಅಕ್ಷರ writable;
  ರಚನೆ pipe *pipe; // FD_PIPE
  ರಚನೆ inode *ip;  // FD_INODE and FD_DEVICE
  uint off;          // FD_INODE
  ಗಿಡ್ಡ major;       // FD_DEVICE
};

#ವ್ಯಾಖ್ಯಾನಿಸು major(dev)  ((dev) >> 16 & 0xFFFF)
#ವ್ಯಾಖ್ಯಾನಿಸು minor(dev)  ((dev) & 0xFFFF)
#ವ್ಯಾಖ್ಯಾನಿಸು mkdev(m, n) ((uint)((m) << 16 | (n)))

// in-memory copy of an inode
ರಚನೆ inode {
  uint dev;              // Device number
  uint inum;             // Inode number
  ಪೂರ್ಣಾಂಕ ref;               // Reference count
  ರಚನೆ sleeplock ಬೀಗ; // protects everything below here
  ಪೂರ್ಣಾಂಕ valid;             // inode has been read from disk?

  ಗಿಡ್ಡ type; // copy of disk inode
  ಗಿಡ್ಡ major;
  ಗಿಡ್ಡ minor;
  ಗಿಡ್ಡ nlink;
  uint ಗಾತ್ರ_2;
  uint addrs[NDIRECT + 1];
};

// map major device number to device functions.
ರಚನೆ devsw {
  ಪೂರ್ಣಾಂಕ (*ಪಠ)(ಪೂರ್ಣಾಂಕ, uint64, ಪೂರ್ಣಾಂಕ);
  ಪೂರ್ಣಾಂಕ (*ಲಿಖ)(ಪೂರ್ಣಾಂಕ, uint64, ಪೂರ್ಣಾಂಕ);
};

ಬಾಹ್ಯ ರಚನೆ devsw devsw[];

#ವ್ಯಾಖ್ಯಾನಿಸು CONSOLE 1
