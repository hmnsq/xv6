// Mutual exclusion lock.
sa_nracanaa spinlock {
  uint locked; // Is the lock held?

  // For debugging:
  var_na *name;      // Name of lock.
  sa_nracanaa cpu *cpu; // The cpu holding the lock.
};
