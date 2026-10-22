// Mutual exclusion lock.
ସଂରଚନା spinlock {
  uint locked; // Is the lock held?

  // For debugging:
  ଅକ୍ଷର *name;      // Name of lock.
  ସଂରଚନା cpu *cpu; // The cpu holding the lock.
};
