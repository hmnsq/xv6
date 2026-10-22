// Mutual exclusion lock.
काष्ठा spinlock {
  uint locked; // Is the lock held?

  // For debugging:
  अक्षर *name;      // Name of lock.
  काष्ठा cpu *cpu; // The cpu holding the lock.
};
