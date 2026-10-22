#تعریف_کرو T_DIR    1 // Directory
#تعریف_کرو T_FILE   2 // File
#تعریف_کرو T_DEVICE 3 // Device

ساخت stat {
  صحیح dev;     // File system's disk device
  uint ino;    // Inode number
  مختصر type;  // Type of file
  مختصر nlink; // Number of links to file
  uint64 سائز; // Size of file in bytes
};
