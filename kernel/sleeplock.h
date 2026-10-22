// Long-term locks for processes
ସଂରଚନା sleeplock {
  uint locked;        // Is the lock held?
  ସଂରଚନା spinlock lk; // spinlock protecting this sleep lock

  // For debugging:
  ଅକ୍ଷର *name; // Name of lock.
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା pid;    // Process holding lock
};
