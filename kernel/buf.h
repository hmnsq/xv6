ರಚನೆ buf {
  ಪೂರ್ಣಾಂಕ valid; // has data been read from disk?
  ಪೂರ್ಣಾಂಕ disk;  // does disk "own" buf?
  uint dev;
  uint blockno;
  ರಚನೆ sleeplock ಬೀಗ;
  uint refcnt;
  ರಚನೆ buf *prev; // LRU cache list
  ರಚನೆ buf *ಮುಂದಿನ;
  uchar ದತ್ತಾಂಶ[BSIZE];
};
