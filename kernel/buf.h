கட்டமைப்பு buf {
  முழு valid; // has data been read from disk?
  முழு disk;  // does disk "own" buf?
  uint dev;
  uint blockno;
  கட்டமைப்பு sleeplock lock;
  uint refcnt;
  கட்டமைப்பு buf *prev; // LRU cache list
  கட்டமைப்பு buf *next;
  uchar data[BSIZE];
};
