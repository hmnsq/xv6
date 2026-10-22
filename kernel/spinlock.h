// Mutual exclusion lock.
संरचना spinlock {
  uint locked; // Is the lock held?

  // For debugging:
  अक्षर *name;      // Name of lock.
  संरचना cpu *cpu; // The cpu holding the lock.
};
