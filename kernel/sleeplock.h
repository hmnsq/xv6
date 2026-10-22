// Long-term locks for processes
నిర్మాణం sleeplock {
  uint locked;        // Is the lock held?
  నిర్మాణం spinlock lk; // spinlock protecting this sleep lock

  // For debugging:
  అక్షరం *name; // Name of lock.
  పూర్ణాంకం pid;    // Process holding lock
};
