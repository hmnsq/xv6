// Format of an ELF executable file

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ELF_MAGIC 0x464C457FU // "\x7FELF" in little endian

// File header
ਬਣਤਰ elfhdr {
  uint magic; // must equal ELF_MAGIC
  uchar elf[12];
  ushort type;
  ushort machine;
  uint ਸੰਸਕਰਣ;
  uint64 ਇੰਦਰਾਜ;
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
ਬਣਤਰ proghdr {
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
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ELF_PROG_ਲੱਦੋ 1

// Flag bits for Proghdr flags
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ELF_PROG_FLAG_EXEC  1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ELF_PROG_FLAG_WRITE 2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ELF_PROG_FLAG_READ  4
