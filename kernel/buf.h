ഘടന buf {
  പൂർണ്ണസംഖ്യ valid; // has data been read from disk?
  പൂർണ്ണസംഖ്യ disk;  // does disk "own" buf?
  uint dev;
  uint blockno;
  ഘടന sleeplock lock;
  uint refcnt;
  ഘടന buf *prev; // LRU cache list
  ഘടന buf *next;
  uchar data[BSIZE];
};
