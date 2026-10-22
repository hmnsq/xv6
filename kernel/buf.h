بنية buf {
  صحيح valid; // has data been read from disk?
  صحيح disk;  // does disk "own" buf?
  uint dev;
  uint blockno;
  بنية sleeplock قفل;
  uint refcnt;
  بنية buf *prev; // LRU cache list
  بنية buf *بعدی;
  uchar داده[BSIZE];
};
