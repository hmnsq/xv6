sa_nracanaa buf {
  จำนวนเต็ม valid; // has data been read from disk?
  จำนวนเต็ม disk;  // does disk "own" buf?
  uint dev;
  uint blockno;
  sa_nracanaa sleeplock lock;
  uint refcnt;
  sa_nracanaa buf *prev; // LRU cache list
  sa_nracanaa buf *next;
  uchar data[BSIZE];
};
