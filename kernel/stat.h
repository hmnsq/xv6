#تعریف_کړه T_DIR    1 // Directory
#تعریف_کړه T_FILE   2 // File
#تعریف_کړه T_DEVICE 3 // Device

بنية stat {
  صحيح dev;     // File system's disk device
  uint ino;    // Inode number
  لنډ type;  // Type of file
  لنډ nlink; // Number of links to file
  uint64 اندازه; // Size of file in bytes
};
