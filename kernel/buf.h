संरचना buf {
  पूर्णाङ्क valid; // has data been read from disk?
  पूर्णाङ्क disk;  // does disk "own" buf?
  uint dev;
  uint blockno;
  संरचना sleeplock lock;
  uint refcnt;
  संरचना buf *prev; // LRU cache list
  संरचना buf *next;
  uchar data[BSIZE];
};
