// Long-term locks for processes
ساخت sleeplock {
  uint locked;        // Is the lock held?
  ساخت spinlock lk; // spinlock protecting this sleep lock

  // For debugging:
  حرف *name; // Name of lock.
  صحیح pid;    // Process holding lock
};
