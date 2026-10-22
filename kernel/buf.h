بنية buf {
  صحيح valid; // has data been read from disk?
  صحيح disk;  // does disk "own" buf?
  uint dev;
  uint blockno;
  بنية sleeplock قلف;
  uint refcnt;
  بنية buf *prev; // LRU cache list
  بنية buf *بل;
  uchar معلومات[BSIZE];
};
