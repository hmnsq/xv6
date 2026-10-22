#ವ್ಯಾಖ್ಯಾನಿಸು T_DIR    1 // Directory
#ವ್ಯಾಖ್ಯಾನಿಸು T_FILE   2 // File
#ವ್ಯಾಖ್ಯಾನಿಸು T_DEVICE 3 // Device

ರಚನೆ stat {
  ಪೂರ್ಣಾಂಕ dev;     // File system's disk device
  uint ino;    // Inode number
  ಗಿಡ್ಡ type;  // Type of file
  ಗಿಡ್ಡ nlink; // Number of links to file
  uint64 ಗಾತ್ರ_2; // Size of file in bytes
};
