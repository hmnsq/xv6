#परिभाषय T_DIR    1 // Directory
#परिभाषय T_FILE   2 // File
#परिभाषय T_DEVICE 3 // Device

संरचना stat {
  पूर्णाङ्क dev;     // File system's disk device
  uint ino;    // Inode number
  ह्रस्व type;  // Type of file
  ह्रस्व nlink; // Number of links to file
  uint64 आकार; // Size of file in bytes
};
