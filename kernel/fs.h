// On-disk file system format.
// Both the kernel and user programs use this header file.

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ROOTINO 1    // root i-number
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BSIZE   1024 // block size

// Disk layout:
// [ boot block | super block | log | inode blocks |
//                                          free bit map | data blocks]
//
// mkfs computes the super block and builds an initial file system. The
// super block describes the disk layout:
ਬਣਤਰ superblock {
  uint magic;      // Must be FSMAGIC
  uint ਮਾਪ;       // Size of file system image (blocks)
  uint nblocks;    // Number of data blocks
  uint ninodes;    // Number of inodes.
  uint nlog;       // Number of log blocks
  uint logstart;   // Block number of first log block
  uint inodestart; // Block number of first inode block
  uint bmapstart;  // Block number of first free map block
};

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FSMAGIC 0x10203040

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NDIRECT   12
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NINDIRECT (BSIZE / ਆਕਾਰ(uint))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAXFILE   (NDIRECT + NINDIRECT)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NLINK_MAX 32767 // nlink is a short; refuse links past its maximum

// On-disk inode structure
ਬਣਤਰ dinode {
  ਛੋਟਾ type;              // File type
  ਛੋਟਾ major;             // Major device number (T_DEVICE only)
  ਛੋਟਾ minor;             // Minor device number (T_DEVICE only)
  ਛੋਟਾ nlink;             // Number of links to inode in file system
  uint ਮਾਪ;               // Size of file (bytes)
  uint addrs[NDIRECT + 1]; // Data block addresses
};

// Inodes per block.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IPB (BSIZE / ਆਕਾਰ(ਬਣਤਰ dinode))

// Block containing inode i
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IBLOCK(i, sb) ((i) / IPB + sb.inodestart)

// Bitmap bits per block
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BPB (BSIZE * 8)

// Block of free map containing bit for block b
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BBLOCK(b, sb) ((b) / BPB + sb.bmapstart)

// Directory is a file containing a sequence of dirent structures.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DIRSIZ 14

// The name field may have DIRSIZ characters and not end in a NUL
// character.
ਬਣਤਰ dirent {
  ushort inum;
  ਅੱਖਰ name[DIRSIZ] __attribute__((nonstring));
};
