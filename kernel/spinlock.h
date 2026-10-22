// Mutual exclusion lock.
నిర్మాణం spinlock {
  uint locked; // Is the lock held?

  // For debugging:
  అక్షరం *name;      // Name of lock.
  నిర్మాణం cpu *cpu; // The cpu holding the lock.
};
