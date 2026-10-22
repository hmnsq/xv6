#define T_DIR    1 // Directory
#define T_FILE   2 // File
#define T_DEVICE 3 // Device

संरचना stat {
  पूर्णाङ्क dev;     // File system's disk device
  uint ino;    // Inode number
  लघु type;  // Type of file
  लघु nlink; // Number of links to file
  uint64 size; // Size of file in bytes
};
