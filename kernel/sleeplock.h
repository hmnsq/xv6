// Long-term locks for processes
સંરચના sleeplock {
  uint locked;        // Is the lock held?
  સંરચના spinlock lk; // spinlock protecting this sleep lock

  // For debugging:
  અક્ષર *name; // Name of lock.
  પૂર્ણાંક pid;    // Process holding lock
};
