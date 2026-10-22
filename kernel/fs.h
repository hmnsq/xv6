// On-disk file system format.
// Both the kernel and user programs use this header file.

#تعریف_کرو ROOTINO 1    // root i-number
#تعریف_کرو BSIZE   1024 // block size

// Disk layout:
// [ boot block | super block | log | inode blocks |
//                                          free bit map | data blocks]
//
// mkfs computes the super block and builds an initial file system. The
// super block describes the disk layout:
ساخت superblock {
  uint magic;      // Must be FSMAGIC
  uint سائز;       // Size of file system image (blocks)
  uint nblocks;    // Number of data blocks
  uint ninodes;    // Number of inodes.
  uint nlog;       // Number of log blocks
  uint logstart;   // Block number of first log block
  uint inodestart; // Block number of first inode block
  uint bmapstart;  // Block number of first free map block
};

#تعریف_کرو FSMAGIC 0x10203040

#تعریف_کرو NDIRECT   12
#تعریف_کرو NINDIRECT (BSIZE / حجم(uint))
#تعریف_کرو MAXFILE   (NDIRECT + NINDIRECT)
#تعریف_کرو NLINK_MAX 32767 // nlink is a short; refuse links past its maximum

// On-disk inode structure
ساخت dinode {
  مختصر type;              // File type
  مختصر major;             // Major device number (T_DEVICE only)
  مختصر minor;             // Minor device number (T_DEVICE only)
  مختصر nlink;             // Number of links to inode in file system
  uint سائز;               // Size of file (bytes)
  uint addrs[NDIRECT + 1]; // Data block addresses
};

// Inodes per block.
#تعریف_کرو IPB (BSIZE / حجم(ساخت dinode))

// Block containing inode i
#تعریف_کرو IBLOCK(i, sb) ((i) / IPB + sb.inodestart)

// Bitmap bits per block
#تعریف_کرو BPB (BSIZE * 8)

// Block of free map containing bit for block b
#تعریف_کرو BBLOCK(b, sb) ((b) / BPB + sb.bmapstart)

// Directory is a file containing a sequence of dirent structures.
#تعریف_کرو DIRSIZ 14

// The name field may have DIRSIZ characters and not end in a NUL
// character.
ساخت dirent {
  ushort inum;
  حرف name[DIRSIZ] __attribute__((nonstring));
};
