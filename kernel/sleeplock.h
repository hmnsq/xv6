// Long-term locks for processes
ਬਣਤਰ sleeplock {
  uint locked;        // Is the lock held?
  ਬਣਤਰ spinlock lk; // spinlock protecting this sleep lock

  // For debugging:
  ਅੱਖਰ *name; // Name of lock.
  ਪੂਰਨ pid;    // Process holding lock
};
