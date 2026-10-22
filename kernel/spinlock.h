// Mutual exclusion lock.
ಸಂರಚನಾ spinlock {
  uint locked; // Is the lock held?

  // For debugging:
  ವರ್ಣ *name;      // Name of lock.
  ಸಂರಚನಾ cpu *cpu; // The cpu holding the lock.
};
