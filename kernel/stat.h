#وصف_ڪريو T_DIR    1 // Directory
#وصف_ڪريو T_FILE   2 // File
#وصف_ڪريو T_DEVICE 3 // Device

بنية stat {
  صحيح dev;     // File system's disk device
  uint ino;    // Inode number
  ننڍو type;  // Type of file
  ننڍو nlink; // Number of links to file
  uint64 سائيز; // Size of file in bytes
};
