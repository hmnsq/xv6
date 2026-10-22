ساخت buf {
  صحیح valid; // has data been read from disk?
  صحیح disk;  // does disk "own" buf?
  uint dev;
  uint blockno;
  ساخت sleeplock تالا;
  uint refcnt;
  ساخت buf *پچھلا; // LRU cache list
  ساخت buf *اگلا;
  uchar ڈیٹا[BSIZE];
};
