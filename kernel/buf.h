sa_nracanaa buf {
  ចំនួនគត់ valid; // has data been read from disk?
  ចំនួនគត់ disk;  // does disk "own" buf?
  uint dev;
  uint blockno;
  sa_nracanaa sleeplock lock;
  uint refcnt;
  sa_nracanaa buf *prev; // LRU cache list
  sa_nracanaa buf *next;
  uchar data[BSIZE];
};
