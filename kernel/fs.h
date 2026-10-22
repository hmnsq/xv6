// On-disk file system format.
// Both the kernel and user programs use this header file.

#घोषणा ROOTINO 1    // root i-number
#घोषणा BSIZE   1024 // block size

// Disk layout:
// [ boot block | super block | log | inode blocks |
//                                          free bit map | data blocks]
//
// mkfs computes the super block and builds an initial file system. The
// super block describes the disk layout:
काष्ठा superblock {
  uint magic;      // Must be FSMAGIC
  uint आकार;       // Size of file system image (blocks)
  uint nblocks;    // Number of data blocks
  uint ninodes;    // Number of inodes.
  uint nlog;       // Number of log blocks
  uint logstart;   // Block number of first log block
  uint inodestart; // Block number of first inode block
  uint bmapstart;  // Block number of first free map block
};

#घोषणा FSMAGIC 0x10203040

#घोषणा NDIRECT   12
#घोषणा NINDIRECT (BSIZE / माप(uint))
#घोषणा MAXFILE   (NDIRECT + NINDIRECT)
#घोषणा NLINK_MAX 32767 // nlink is a short; refuse links past its maximum

// On-disk inode structure
काष्ठा dinode {
  लघु type;              // File type
  लघु major;             // Major device number (T_DEVICE only)
  लघु minor;             // Minor device number (T_DEVICE only)
  लघु nlink;             // Number of links to inode in file system
  uint आकार;               // Size of file (bytes)
  uint addrs[NDIRECT + 1]; // Data block addresses
};

// Inodes per block.
#घोषणा IPB (BSIZE / माप(काष्ठा dinode))

// Block containing inode i
#घोषणा IBLOCK(i, sb) ((i) / IPB + sb.inodestart)

// Bitmap bits per block
#घोषणा BPB (BSIZE * 8)

// Block of free map containing bit for block b
#घोषणा BBLOCK(b, sb) ((b) / BPB + sb.bmapstart)

// Directory is a file containing a sequence of dirent structures.
#घोषणा DIRSIZ 14

// The name field may have DIRSIZ characters and not end in a NUL
// character.
काष्ठा dirent {
  ushort inum;
  अक्षर name[DIRSIZ] __attribute__((nonstring));
};
