संरचना buf {
  पूर्णाङ्क valid; // has data been read from disk?
  पूर्णाङ्क disk;  // does disk "own" buf?
  uint dev;
  uint blockno;
  संरचना sleeplock तालकम्;
  uint refcnt;
  संरचना buf *पूर्व; // LRU cache list
  संरचना buf *अग्र;
  uchar दत्त[BSIZE];
};
