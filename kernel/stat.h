#تعریف_کرو T_DIR    1 // Directory
#تعریف_کرو T_FILE   2 // File
#تعریف_کرو T_DEVICE 3 // Device

بنية stat {
  صحيح dev;     // File system's disk device
  uint ino;    // Inode number
  چھوٹا type;  // Type of file
  چھوٹا nlink; // Number of links to file
  uint64 ناپ; // Size of file in bytes
};
