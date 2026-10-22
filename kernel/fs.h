// On-disk file system format.
// Both the kernel and user programs use this header file.

#تعریف_کن ROOTINO 1    // root i-number
#تعریف_کن BSIZE   1024 // block size

// Disk layout:
// [ boot block | super block | log | inode blocks |
//                                          free bit map | data blocks]
//
// mkfs computes the super block and builds an initial file system. The
// super block describes the disk layout:
بنية superblock {
  uint magic;      // Must be FSMAGIC
  uint اندازه;       // Size of file system image (blocks)
  uint nblocks;    // Number of data blocks
  uint ninodes;    // Number of inodes.
  uint nlog;       // Number of log blocks
  uint logstart;   // Block number of first log block
  uint inodestart; // Block number of first inode block
  uint bmapstart;  // Block number of first free map block
};

#تعریف_کن FSMAGIC 0x10203040

#تعریف_کن NDIRECT   12
#تعریف_کن NINDIRECT (BSIZE / حجم(uint))
#تعریف_کن MAXFILE   (NDIRECT + NINDIRECT)
#تعریف_کن NLINK_MAX 32767 // nlink is a short; refuse links past its maximum

// On-disk inode structure
بنية dinode {
  کوتاه type;              // File type
  کوتاه major;             // Major device number (T_DEVICE only)
  کوتاه minor;             // Minor device number (T_DEVICE only)
  کوتاه nlink;             // Number of links to inode in file system
  uint اندازه;               // Size of file (bytes)
  uint addrs[NDIRECT + 1]; // Data block addresses
};

// Inodes per block.
#تعریف_کن IPB (BSIZE / حجم(بنية dinode))

// Block containing inode i
#تعریف_کن IBLOCK(i, sb) ((i) / IPB + sb.inodestart)

// Bitmap bits per block
#تعریف_کن BPB (BSIZE * 8)

// Block of free map containing bit for block b
#تعریف_کن BBLOCK(b, sb) ((b) / BPB + sb.bmapstart)

// Directory is a file containing a sequence of dirent structures.
#تعریف_کن DIRSIZ 14

// The name field may have DIRSIZ characters and not end in a NUL
// character.
بنية dirent {
  ushort inum;
  حرف name[DIRSIZ] __attribute__((nonstring));
};
