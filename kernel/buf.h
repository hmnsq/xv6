ساختار buf {
  صحیح valid; // has data been read from disk?
  صحیح disk;  // does disk "own" buf?
  uint dev;
  uint blockno;
  ساختار sleeplock lock;
  uint refcnt;
  ساختار buf *prev; // LRU cache list
  ساختار buf *next;
  uchar data[BSIZE];
};
