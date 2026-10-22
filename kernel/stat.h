#تعریف_کن T_DIR    1 // Directory
#تعریف_کن T_FILE   2 // File
#تعریف_کن T_DEVICE 3 // Device

بنية stat {
  صحيح dev;     // File system's disk device
  uint ino;    // Inode number
  کوتاه type;  // Type of file
  کوتاه nlink; // Number of links to file
  uint64 اندازه; // Size of file in bytes
};
