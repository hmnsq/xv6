#घोषणा T_DIR    1 // Directory
#घोषणा T_FILE   2 // File
#घोषणा T_DEVICE 3 // Device

काष्ठा stat {
  पूर्णांक dev;     // File system's disk device
  uint ino;    // Inode number
  लघु type;  // Type of file
  लघु nlink; // Number of links to file
  uint64 आकार; // Size of file in bytes
};
