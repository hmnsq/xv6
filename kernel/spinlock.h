// Mutual exclusion lock.
கட்டமைப்பு spinlock {
  uint locked; // Is the lock held?

  // For debugging:
  எழுத்து *name;      // Name of lock.
  கட்டமைப்பு cpu *cpu; // The cpu holding the lock.
};
