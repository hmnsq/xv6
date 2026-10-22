// Long-term locks for processes
काष्ठा sleeplock {
  uint locked;        // Is the lock held?
  काष्ठा spinlock lk; // spinlock protecting this sleep lock

  // For debugging:
  अक्षर *name; // Name of lock.
  पूर्णांक pid;    // Process holding lock
};
