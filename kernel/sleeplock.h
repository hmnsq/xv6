// Long-term locks for processes
ರಚನೆ sleeplock {
  uint locked;        // Is the lock held?
  ರಚನೆ spinlock lk; // spinlock protecting this sleep lock

  // For debugging:
  ಅಕ್ಷರ *name; // Name of lock.
  ಪೂರ್ಣಾಂಕ pid;    // Process holding lock
};
