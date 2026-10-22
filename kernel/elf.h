// Format of an ELF executable file

#وصف_ڪريو ELF_MAGIC 0x464C457FU // "\x7FELF" in little endian

// File header
بنية elfhdr {
  uint magic; // must equal ELF_MAGIC
  uchar elf[12];
  ushort type;
  ushort machine;
  uint نسخو;
  uint64 داخلا;
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
بنية proghdr {
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
#وصف_ڪريو ELF_PROG_ڀريو 1

// Flag bits for Proghdr flags
#وصف_ڪريو ELF_PROG_FLAG_EXEC  1
#وصف_ڪريو ELF_PROG_FLAG_WRITE 2
#وصف_ڪريو ELF_PROG_FLAG_READ  4
