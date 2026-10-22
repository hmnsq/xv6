ਬਣਤਰ file {
  ਗਿਣਤੀ { FD_NONE, FD_PIPE, FD_INODE, FD_DEVICE } type;
  ਪੂਰਨ ref; // reference count
  ਅੱਖਰ readable;
  ਅੱਖਰ writable;
  ਬਣਤਰ pipe *pipe; // FD_PIPE
  ਬਣਤਰ inode *ip;  // FD_INODE and FD_DEVICE
  uint off;          // FD_INODE
  ਛੋਟਾ major;       // FD_DEVICE
};

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ major(dev)  ((dev) >> 16 & 0xFFFF)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ minor(dev)  ((dev) & 0xFFFF)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ mkdev(m, n) ((uint)((m) << 16 | (n)))

// in-memory copy of an inode
ਬਣਤਰ inode {
  uint dev;              // Device number
  uint inum;             // Inode number
  ਪੂਰਨ ref;               // Reference count
  ਬਣਤਰ sleeplock ਤਾਲਾ; // protects everything below here
  ਪੂਰਨ valid;             // inode has been read from disk?

  ਛੋਟਾ type; // copy of disk inode
  ਛੋਟਾ major;
  ਛੋਟਾ minor;
  ਛੋਟਾ nlink;
  uint ਮਾਪ;
  uint addrs[NDIRECT + 1];
};

// map major device number to device functions.
ਬਣਤਰ devsw {
  ਪੂਰਨ (*ਪਠ)(ਪੂਰਨ, uint64, ਪੂਰਨ);
  ਪੂਰਨ (*ਲਿਖ)(ਪੂਰਨ, uint64, ਪੂਰਨ);
};

ਬਾਹਰੀ ਬਣਤਰ devsw devsw[];

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CONSOLE 1
