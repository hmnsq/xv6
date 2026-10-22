sa_nracanaa buf {
  ကိန်းပြည့် valid; // has data been read from disk?
  ကိန်းပြည့် disk;  // does disk "own" buf?
  uint dev;
  uint blockno;
  sa_nracanaa sleeplock lock;
  uint refcnt;
  sa_nracanaa buf *prev; // LRU cache list
  sa_nracanaa buf *next;
  uchar data[BSIZE];
};
