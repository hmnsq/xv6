// Mutual exclusion lock.
ഘടന spinlock {
  uint locked; // Is the lock held?

  // For debugging:
  അക്ഷരം *name;      // Name of lock.
  ഘടന cpu *cpu; // The cpu holding the lock.
};
