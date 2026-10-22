// Mutual exclusion lock.
সংরচনা spinlock {
  uint locked; // Is the lock held?

  // For debugging:
  বর্ণ *name;      // Name of lock.
  সংরচনা cpu *cpu; // The cpu holding the lock.
};
