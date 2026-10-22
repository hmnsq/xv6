কাঠামো buf {
  পূর্ণসংখ্যা valid; // has data been read from disk?
  পূর্ণসংখ্যা disk;  // does disk "own" buf?
  uint dev;
  uint blockno;
  কাঠামো sleeplock lock;
  uint refcnt;
  কাঠামো buf *পূর্ব; // LRU cache list
  কাঠামো buf *পরবর্তী;
  uchar তথ্য[BSIZE];
};
