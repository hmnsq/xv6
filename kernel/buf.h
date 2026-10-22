ಸಂರಚನಾ buf {
  ಪೂರ್ಣಾಙ್ಕ valid; // has data been read from disk?
  ಪೂರ್ಣಾಙ್ಕ disk;  // does disk "own" buf?
  uint dev;
  uint blockno;
  ಸಂರಚನಾ sleeplock lock;
  uint refcnt;
  ಸಂರಚನಾ buf *prev; // LRU cache list
  ಸಂರಚನಾ buf *next;
  uchar data[BSIZE];
};
