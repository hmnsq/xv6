// Long-term locks for processes
ಸಂರಚನಾ sleeplock {
  uint locked;        // Is the lock held?
  ಸಂರಚನಾ spinlock lk; // spinlock protecting this sleep lock

  // For debugging:
  ವರ್ಣ *name; // Name of lock.
  ಪೂರ್ಣಾಙ್ಕ pid;    // Process holding lock
};
