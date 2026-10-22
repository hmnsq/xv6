काष्ठा file {
  क्रमागत { FD_NONE, FD_PIPE, FD_INODE, FD_DEVICE } type;
  पूर्णांक ref; // reference count
  अक्षर readable;
  अक्षर writable;
  काष्ठा pipe *pipe; // FD_PIPE
  काष्ठा inode *ip;  // FD_INODE and FD_DEVICE
  uint off;          // FD_INODE
  लघु major;       // FD_DEVICE
};

#घोषणा major(dev)  ((dev) >> 16 & 0xFFFF)
#घोषणा minor(dev)  ((dev) & 0xFFFF)
#घोषणा mkdev(m, n) ((uint)((m) << 16 | (n)))

// in-memory copy of an inode
काष्ठा inode {
  uint dev;              // Device number
  uint inum;             // Inode number
  पूर्णांक ref;               // Reference count
  काष्ठा sleeplock lock; // protects everything below here
  पूर्णांक valid;             // inode has been read from disk?

  लघु type; // copy of disk inode
  लघु major;
  लघु minor;
  लघु nlink;
  uint आकार;
  uint addrs[NDIRECT + 1];
};

// map major device number to device functions.
काष्ठा devsw {
  पूर्णांक (*पढ़ो)(पूर्णांक, uint64, पूर्णांक);
  पूर्णांक (*ग_लिखो)(पूर्णांक, uint64, पूर्णांक);
};

बाह्य काष्ठा devsw devsw[];

#घोषणा CONSOLE 1
