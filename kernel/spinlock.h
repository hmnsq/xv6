// Mutual exclusion lock.
কাঠামো spinlock {
  uint locked; // Is the lock held?

  // For debugging:
  অক্ষর *name;      // Name of lock.
  কাঠামো cpu *cpu; // The cpu holding the lock.
};
