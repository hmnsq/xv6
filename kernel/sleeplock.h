// Long-term locks for processes
கட்டமைப்பு sleeplock {
  uint locked;        // Is the lock held?
  கட்டமைப்பு spinlock lk; // spinlock protecting this sleep lock

  // For debugging:
  எழுத்து *name; // Name of lock.
  முழு pid;    // Process holding lock
};
