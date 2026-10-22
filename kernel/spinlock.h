// Mutual exclusion lock.
ساخت spinlock {
  uint locked; // Is the lock held?

  // For debugging:
  حرف *name;      // Name of lock.
  ساخت cpu *cpu; // The cpu holding the lock.
};
