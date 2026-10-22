সংরচনা buf {
  পূর্ণাঙ্ক valid; // has data been read from disk?
  পূর্ণাঙ্ক disk;  // does disk "own" buf?
  uint dev;
  uint blockno;
  সংরচনা sleeplock lock;
  uint refcnt;
  সংরচনা buf *prev; // LRU cache list
  সংরচনা buf *next;
  uchar data[BSIZE];
};
