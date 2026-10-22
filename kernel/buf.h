sa_nracanaa buf {
  පූර්ණ_සංඛ්‍යා valid; // has data been read from disk?
  පූර්ණ_සංඛ්‍යා disk;  // does disk "own" buf?
  uint dev;
  uint blockno;
  sa_nracanaa sleeplock lock;
  uint refcnt;
  sa_nracanaa buf *prev; // LRU cache list
  sa_nracanaa buf *next;
  uchar data[BSIZE];
};
