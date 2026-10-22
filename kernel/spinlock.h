// Mutual exclusion lock.
સંરચના spinlock {
  uint locked; // Is the lock held?

  // For debugging:
  અક્ષર *name;      // Name of lock.
  સંરચના cpu *cpu; // The cpu holding the lock.
};
