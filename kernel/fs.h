// On-disk file system format.
// Both the kernel and user programs use this header file.

#وصف_ڪريو ROOTINO 1    // root i-number
#وصف_ڪريو BSIZE   1024 // block size

// Disk layout:
// [ boot block | super block | log | inode blocks |
//                                          free bit map | data blocks]
//
// mkfs computes the super block and builds an initial file system. The
// super block describes the disk layout:
بنية superblock {
  uint magic;      // Must be FSMAGIC
  uint سائيز;       // Size of file system image (blocks)
  uint nblocks;    // Number of data blocks
  uint ninodes;    // Number of inodes.
  uint nlog;       // Number of log blocks
  uint logstart;   // Block number of first log block
  uint inodestart; // Block number of first inode block
  uint bmapstart;  // Block number of first free map block
};

#وصف_ڪريو FSMAGIC 0x10203040

#وصف_ڪريو NDIRECT   12
#وصف_ڪريو NINDIRECT (BSIZE / حجم(uint))
#وصف_ڪريو MAXFILE   (NDIRECT + NINDIRECT)
#وصف_ڪريو NLINK_MAX 32767 // nlink is a short; refuse links past its maximum

// On-disk inode structure
بنية dinode {
  ننڍو type;              // File type
  ننڍو major;             // Major device number (T_DEVICE only)
  ننڍو minor;             // Minor device number (T_DEVICE only)
  ننڍو nlink;             // Number of links to inode in file system
  uint سائيز;               // Size of file (bytes)
  uint addrs[NDIRECT + 1]; // Data block addresses
};

// Inodes per block.
#وصف_ڪريو IPB (BSIZE / حجم(بنية dinode))

// Block containing inode i
#وصف_ڪريو IBLOCK(i, sb) ((i) / IPB + sb.inodestart)

// Bitmap bits per block
#وصف_ڪريو BPB (BSIZE * 8)

// Block of free map containing bit for block b
#وصف_ڪريو BBLOCK(b, sb) ((b) / BPB + sb.bmapstart)

// Directory is a file containing a sequence of dirent structures.
#وصف_ڪريو DIRSIZ 14

// The name field may have DIRSIZ characters and not end in a NUL
// character.
بنية dirent {
  ushort inum;
  حرف name[DIRSIZ] __attribute__((nonstring));
};
