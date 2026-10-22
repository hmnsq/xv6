// Long-term locks for processes
संरचना sleeplock {
  uint locked;        // Is the lock held?
  संरचना spinlock lk; // spinlock protecting this sleep lock

  // For debugging:
  वर्ण *name; // Name of lock.
  पूर्णाङ्क pid;    // Process holding lock
};
