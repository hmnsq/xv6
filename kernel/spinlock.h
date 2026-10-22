// Mutual exclusion lock.
ರಚನೆ spinlock {
  uint locked; // Is the lock held?

  // For debugging:
  ಅಕ್ಷರ *name;      // Name of lock.
  ರಚನೆ cpu *cpu; // The cpu holding the lock.
};
