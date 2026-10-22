// Mutual exclusion lock.
بنية spinlock {
  uint locked; // Is the lock held?

  // For debugging:
  حرف *name;      // Name of lock.
  بنية cpu *cpu; // The cpu holding the lock.
};
