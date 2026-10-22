काष्ठा buf {
  पूर्णांक valid; // has data been read from disk?
  पूर्णांक disk;  // does disk "own" buf?
  uint dev;
  uint blockno;
  काष्ठा sleeplock lock;
  uint refcnt;
  काष्ठा buf *पूर्व; // LRU cache list
  काष्ठा buf *अग्र;
  uchar तथ्य[BSIZE];
};
