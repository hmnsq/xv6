// Long-term locks for processes
sa_nracanaa sleeplock {
  uint locked;        // Is the lock held?
  sa_nracanaa spinlock lk; // spinlock protecting this sleep lock

  // For debugging:
  var_na *name; // Name of lock.
  පූර්ණ_සංඛ්‍යා pid;    // Process holding lock
};
