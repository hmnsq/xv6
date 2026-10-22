ସଂରଚନା buf {
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା valid; // has data been read from disk?
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା disk;  // does disk "own" buf?
  uint dev;
  uint blockno;
  ସଂରଚନା sleeplock lock;
  uint refcnt;
  ସଂରଚନା buf *prev; // LRU cache list
  ସଂରଚନା buf *next;
  uchar data[BSIZE];
};
