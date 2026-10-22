#define T_DIR    1 // Directory
#define T_FILE   2 // File
#define T_DEVICE 3 // Device

কাঠামো stat {
  পূর্ণসংখ্যা dev;     // File system's disk device
  uint ino;    // Inode number
  short type;  // Type of file
  short nlink; // Number of links to file
  uint64 আকার_2; // Size of file in bytes
};
