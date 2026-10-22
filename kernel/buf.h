સંરચના buf {
  પૂર્ણાંક valid; // has data been read from disk?
  પૂર્ણાંક disk;  // does disk "own" buf?
  uint dev;
  uint blockno;
  સંરચના sleeplock lock;
  uint refcnt;
  સંરચના buf *prev; // LRU cache list
  સંરચના buf *next;
  uchar data[BSIZE];
};
