संरचना buf {
  पूर्णांक valid; // has data been read from disk?
  पूर्णांक disk;  // does disk "own" buf?
  uint dev;
  uint blockno;
  संरचना sleeplock lock;
  uint refcnt;
  संरचना buf *prev; // LRU cache list
  संरचना buf *next;
  uchar data[BSIZE];
};
