sa_nracanaa buf {
  puur_naa_nka valid; // has data been read from disk?
  puur_naa_nka disk;  // does disk "own" buf?
  uint dev;
  uint blockno;
  sa_nracanaa sleeplock lock;
  uint refcnt;
  sa_nracanaa buf *prev; // LRU cache list
  sa_nracanaa buf *next;
  uchar data[BSIZE];
};
