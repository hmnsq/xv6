// Mutual exclusion lock.
संरचना spinlock {
  uint locked; // Is the lock held?

  // For debugging:
  वर्ण *name;      // Name of lock.
  संरचना cpu *cpu; // The cpu holding the lock.
};
