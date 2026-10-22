// On-disk file system format.
// Both the kernel and user programs use this header file.

#నిర్వచించు ROOTINO 1    // root i-number
#నిర్వచించు BSIZE   1024 // block size

// Disk layout:
// [ boot block | super block | log | inode blocks |
//                                          free bit map | data blocks]
//
// mkfs computes the super block and builds an initial file system. The
// super block describes the disk layout:
నిర్మాణం superblock {
  uint magic;      // Must be FSMAGIC
  uint కొలత;       // Size of file system image (blocks)
  uint nblocks;    // Number of data blocks
  uint ninodes;    // Number of inodes.
  uint nlog;       // Number of log blocks
  uint logstart;   // Block number of first log block
  uint inodestart; // Block number of first inode block
  uint bmapstart;  // Block number of first free map block
};

#నిర్వచించు FSMAGIC 0x10203040

#నిర్వచించు NDIRECT   12
#నిర్వచించు NINDIRECT (BSIZE / పరిమాణం(uint))
#నిర్వచించు MAXFILE   (NDIRECT + NINDIRECT)
#నిర్వచించు NLINK_MAX 32767 // nlink is a short; refuse links past its maximum

// On-disk inode structure
నిర్మాణం dinode {
  పొట్టి type;              // File type
  పొట్టి major;             // Major device number (T_DEVICE only)
  పొట్టి minor;             // Minor device number (T_DEVICE only)
  పొట్టి nlink;             // Number of links to inode in file system
  uint కొలత;               // Size of file (bytes)
  uint addrs[NDIRECT + 1]; // Data block addresses
};

// Inodes per block.
#నిర్వచించు IPB (BSIZE / పరిమాణం(నిర్మాణం dinode))

// Block containing inode i
#నిర్వచించు IBLOCK(i, sb) ((i) / IPB + sb.inodestart)

// Bitmap bits per block
#నిర్వచించు BPB (BSIZE * 8)

// Block of free map containing bit for block b
#నిర్వచించు BBLOCK(b, sb) ((b) / BPB + sb.bmapstart)

// Directory is a file containing a sequence of dirent structures.
#నిర్వచించు DIRSIZ 14

// The name field may have DIRSIZ characters and not end in a NUL
// character.
నిర్మాణం dirent {
  ushort inum;
  అక్షరం name[DIRSIZ] __attribute__((nonstring));
};
