بنية file {
  تعداد { FD_NONE, FD_PIPE, FD_INODE, FD_DEVICE } type;
  صحيح ref; // reference count
  حرف readable;
  حرف writable;
  بنية pipe *pipe; // FD_PIPE
  بنية inode *ip;  // FD_INODE and FD_DEVICE
  uint off;          // FD_INODE
  چھوٹا major;       // FD_DEVICE
};

#تعریف_کرو major(dev)  ((dev) >> 16 & 0xFFFF)
#تعریف_کرو minor(dev)  ((dev) & 0xFFFF)
#تعریف_کرو mkdev(m, n) ((uint)((m) << 16 | (n)))

// in-memory copy of an inode
بنية inode {
  uint dev;              // Device number
  uint inum;             // Inode number
  صحيح ref;               // Reference count
  بنية sleeplock جندرا; // protects everything below here
  صحيح valid;             // inode has been read from disk?

  چھوٹا type; // copy of disk inode
  چھوٹا major;
  چھوٹا minor;
  چھوٹا nlink;
  uint ناپ;
  uint addrs[NDIRECT + 1];
};

// map major device number to device functions.
بنية devsw {
  صحيح (*اقرأ)(صحيح, uint64, صحيح);
  صحيح (*اكتب)(صحيح, uint64, صحيح);
};

خارجي بنية devsw devsw[];

#تعریف_کرو CONSOLE 1
