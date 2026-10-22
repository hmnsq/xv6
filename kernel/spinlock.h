// Mutual exclusion lock.
ਬਣਤਰ spinlock {
  uint locked; // Is the lock held?

  // For debugging:
  ਅੱਖਰ *name;      // Name of lock.
  ਬਣਤਰ cpu *cpu; // The cpu holding the lock.
};
