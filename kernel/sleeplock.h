// Long-term locks for processes
संरचना sleeplock {
  uint locked;        // Is the lock held?
  संरचना spinlock lk; // spinlock protecting this sleep lock

  // For debugging:
  अक्षर *name; // Name of lock.
  पूर्णांक pid;    // Process holding lock
};
