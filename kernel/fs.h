// On-disk file system format.
// Both the kernel and user programs use this header file.

#ವ್ಯಾಖ್ಯಾನಿಸು ROOTINO 1    // root i-number
#ವ್ಯಾಖ್ಯಾನಿಸು BSIZE   1024 // block size

// Disk layout:
// [ boot block | super block | log | inode blocks |
//                                          free bit map | data blocks]
//
// mkfs computes the super block and builds an initial file system. The
// super block describes the disk layout:
ರಚನೆ superblock {
  uint magic;      // Must be FSMAGIC
  uint ಗಾತ್ರ_2;       // Size of file system image (blocks)
  uint nblocks;    // Number of data blocks
  uint ninodes;    // Number of inodes.
  uint nlog;       // Number of log blocks
  uint logstart;   // Block number of first log block
  uint inodestart; // Block number of first inode block
  uint bmapstart;  // Block number of first free map block
};

#ವ್ಯಾಖ್ಯಾನಿಸು FSMAGIC 0x10203040

#ವ್ಯಾಖ್ಯಾನಿಸು NDIRECT   12
#ವ್ಯಾಖ್ಯಾನಿಸು NINDIRECT (BSIZE / ಗಾತ್ರ(uint))
#ವ್ಯಾಖ್ಯಾನಿಸು MAXFILE   (NDIRECT + NINDIRECT)
#ವ್ಯಾಖ್ಯಾನಿಸು NLINK_MAX 32767 // nlink is a short; refuse links past its maximum

// On-disk inode structure
ರಚನೆ dinode {
  ಗಿಡ್ಡ type;              // File type
  ಗಿಡ್ಡ major;             // Major device number (T_DEVICE only)
  ಗಿಡ್ಡ minor;             // Minor device number (T_DEVICE only)
  ಗಿಡ್ಡ nlink;             // Number of links to inode in file system
  uint ಗಾತ್ರ_2;               // Size of file (bytes)
  uint addrs[NDIRECT + 1]; // Data block addresses
};

// Inodes per block.
#ವ್ಯಾಖ್ಯಾನಿಸು IPB (BSIZE / ಗಾತ್ರ(ರಚನೆ dinode))

// Block containing inode i
#ವ್ಯಾಖ್ಯಾನಿಸು IBLOCK(i, sb) ((i) / IPB + sb.inodestart)

// Bitmap bits per block
#ವ್ಯಾಖ್ಯಾನಿಸು BPB (BSIZE * 8)

// Block of free map containing bit for block b
#ವ್ಯಾಖ್ಯಾನಿಸು BBLOCK(b, sb) ((b) / BPB + sb.bmapstart)

// Directory is a file containing a sequence of dirent structures.
#ವ್ಯಾಖ್ಯಾನಿಸು DIRSIZ 14

// The name field may have DIRSIZ characters and not end in a NUL
// character.
ರಚನೆ dirent {
  ushort inum;
  ಅಕ್ಷರ name[DIRSIZ] __attribute__((nonstring));
};
