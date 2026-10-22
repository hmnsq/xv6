// Long-term locks for processes
কাঠামো sleeplock {
  uint locked;        // Is the lock held?
  কাঠামো spinlock lk; // spinlock protecting this sleep lock

  // For debugging:
  অক্ষর *name; // Name of lock.
  পূর্ণসংখ্যা pid;    // Process holding lock
};
