#నిర్వచించు T_DIR    1 // Directory
#నిర్వచించు T_FILE   2 // File
#నిర్వచించు T_DEVICE 3 // Device

నిర్మాణం stat {
  పూర్ణాంకం dev;     // File system's disk device
  uint ino;    // Inode number
  పొట్టి type;  // Type of file
  పొట్టి nlink; // Number of links to file
  uint64 కొలత; // Size of file in bytes
};
