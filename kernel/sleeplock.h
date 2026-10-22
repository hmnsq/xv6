// Long-term locks for processes
ഘടന sleeplock {
  uint locked;        // Is the lock held?
  ഘടന spinlock lk; // spinlock protecting this sleep lock

  // For debugging:
  അക്ഷരം *name; // Name of lock.
  പൂർണ്ണസംഖ്യ pid;    // Process holding lock
};
