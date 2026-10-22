#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ T_DIR    1 // Directory
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ T_FILE   2 // File
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ T_DEVICE 3 // Device

ਬਣਤਰ stat {
  ਪੂਰਨ dev;     // File system's disk device
  uint ino;    // Inode number
  ਛੋਟਾ type;  // Type of file
  ਛੋਟਾ nlink; // Number of links to file
  uint64 ਮਾਪ; // Size of file in bytes
};
