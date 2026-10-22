// Format of an ELF executable file

#నిర్వచించు ELF_MAGIC 0x464C457FU // "\x7FELF" in little endian

// File header
నిర్మాణం elfhdr {
  uint magic; // must equal ELF_MAGIC
  uchar elf[12];
  ushort type;
  ushort machine;
  uint సంస్కరణ;
  uint64 నమోదు_2;
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
నిర్మాణం proghdr {
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
#నిర్వచించు ELF_PROG_ఎక్కించు 1

// Flag bits for Proghdr flags
#నిర్వచించు ELF_PROG_FLAG_EXEC  1
#నిర్వచించు ELF_PROG_FLAG_WRITE 2
#నిర్వచించు ELF_PROG_FLAG_READ  4
