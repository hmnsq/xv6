// Format of an ELF executable file

#ವ್ಯಾಖ್ಯಾನಿಸು ELF_MAGIC 0x464C457FU // "\x7FELF" in little endian

// File header
ರಚನೆ elfhdr {
  uint magic; // must equal ELF_MAGIC
  uchar elf[12];
  ushort type;
  ushort machine;
  uint ಆವೃತ್ತಿ;
  uint64 ನಮೂದು;
  uint64 phoff;
  uint64 shoff;
  uint flags;
  ushort ehsize;
  ushort phentsize;
  ushort phnum;
  ushort shentsize;
  ushort shnum;
  ushort shstrndx;
};

// Program section header
ರಚನೆ proghdr {
  uint32 type;
  uint32 flags;
  uint64 off;
  uint64 vaddr;
  uint64 paddr;
  uint64 filesz;
  uint64 memsz;
  uint64 align;
};

// Values for Proghdr type
#ವ್ಯಾಖ್ಯಾನಿಸು ELF_PROG_ತುಂಬು 1

// Flag bits for Proghdr flags
#ವ್ಯಾಖ್ಯಾನಿಸು ELF_PROG_FLAG_EXEC  1
#ವ್ಯಾಖ್ಯಾನಿಸು ELF_PROG_FLAG_WRITE 2
#ವ್ಯಾಖ್ಯಾನಿಸು ELF_PROG_FLAG_READ  4
