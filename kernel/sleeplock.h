// Long-term locks for processes
ساختار sleeplock {
  uint locked;        // Is the lock held?
  ساختار spinlock lk; // spinlock protecting this sleep lock

  // For debugging:
  نویسه *name; // Name of lock.
  صحیح pid;    // Process holding lock
};
