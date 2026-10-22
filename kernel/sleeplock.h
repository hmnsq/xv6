// Long-term locks for processes
সংরচনা sleeplock {
  uint locked;        // Is the lock held?
  সংরচনা spinlock lk; // spinlock protecting this sleep lock

  // For debugging:
  বর্ণ *name; // Name of lock.
  পূর্ণাঙ্ক pid;    // Process holding lock
};
