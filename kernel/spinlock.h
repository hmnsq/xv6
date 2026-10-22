// Mutual exclusion lock.
ساختار spinlock {
  uint locked; // Is the lock held?

  // For debugging:
  نویسه *name;      // Name of lock.
  ساختار cpu *cpu; // The cpu holding the lock.
};
