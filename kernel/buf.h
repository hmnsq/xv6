ਬਣਤਰ buf {
  ਪੂਰਨ valid; // has data been read from disk?
  ਪੂਰਨ disk;  // does disk "own" buf?
  uint dev;
  uint blockno;
  ਬਣਤਰ sleeplock ਤਾਲਾ;
  uint refcnt;
  ਬਣਤਰ buf *prev; // LRU cache list
  ਬਣਤਰ buf *ਅਗਲਾ;
  uchar ਅੰਕੜੇ[BSIZE];
};
