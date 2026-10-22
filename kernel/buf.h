నిర్మాణం buf {
  పూర్ణాంకం valid; // has data been read from disk?
  పూర్ణాంకం disk;  // does disk "own" buf?
  uint dev;
  uint blockno;
  నిర్మాణం sleeplock తాళం;
  uint refcnt;
  నిర్మాణం buf *prev; // LRU cache list
  నిర్మాణం buf *తదుపరి;
  uchar దత్తాంశం[BSIZE];
};
