// Long-term locks for processes
بنية sleeplock {
  uint locked;        // Is the lock held?
  بنية spinlock lk; // spinlock protecting this sleep lock

  // For debugging:
  حرف *name; // Name of lock.
  صحيح pid;    // Process holding lock
};
