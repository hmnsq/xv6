// On-disk file system format.
// Both the kernel and user programs use this header file.

#परिभाषय ROOTINO 1    // root i-number
#परिभाषय BSIZE   1024 // block size

// Disk layout:
// [ boot block | super block | log | inode blocks |
//                                          free bit map | data blocks]
//
// mkfs computes the super block and builds an initial file system. The
// super block describes the disk layout:
संरचना superblock {
  uint magic;      // Must be FSMAGIC
  uint आकार;       // Size of file system image (blocks)
  uint nblocks;    // Number of data blocks
  uint ninodes;    // Number of inodes.
  uint nlog;       // Number of log blocks
  uint logstart;   // Block number of first log block
  uint inodestart; // Block number of first inode block
  uint bmapstart;  // Block number of first free map block
};

#परिभाषय FSMAGIC 0x10203040

#परिभाषय NDIRECT   12
#परिभाषय NINDIRECT (BSIZE / परिमाण(uint))
#परिभाषय MAXFILE   (NDIRECT + NINDIRECT)
#परिभाषय NLINK_MAX 32767 // nlink is a short; refuse links past its maximum

// On-disk inode structure
संरचना dinode {
  ह्रस्व type;              // File type
  ह्रस्व major;             // Major device number (T_DEVICE only)
  ह्रस्व minor;             // Minor device number (T_DEVICE only)
  ह्रस्व nlink;             // Number of links to inode in file system
  uint आकार;               // Size of file (bytes)
  uint addrs[NDIRECT + 1]; // Data block addresses
};

// Inodes per block.
#परिभाषय IPB (BSIZE / परिमाण(संरचना dinode))

// Block containing inode i
#परिभाषय IBLOCK(i, sb) ((i) / IPB + sb.inodestart)

// Bitmap bits per block
#परिभाषय BPB (BSIZE * 8)

// Block of free map containing bit for block b
#परिभाषय BBLOCK(b, sb) ((b) / BPB + sb.bmapstart)

// Directory is a file containing a sequence of dirent structures.
#परिभाषय DIRSIZ 14

// The name field may have DIRSIZ characters and not end in a NUL
// character.
संरचना dirent {
  ushort inum;
  वर्ण name[DIRSIZ] __attribute__((nonstring));
};
