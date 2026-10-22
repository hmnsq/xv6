// Format of an ELF executable file

#परिभाषय ELF_MAGIC 0x464C457FU // "\x7FELF" in little endian

// File header
संरचना elfhdr {
  uint magic; // must equal ELF_MAGIC
  uchar elf[12];
  ushort type;
  ushort machine;
  uint संस्करणम्;
  uint64 प्रविष्टिः;
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
संरचना proghdr {
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
#परिभाषय ELF_PROG_आरोपय 1

// Flag bits for Proghdr flags
#परिभाषय ELF_PROG_FLAG_EXEC  1
#परिभाषय ELF_PROG_FLAG_WRITE 2
#परिभाषय ELF_PROG_FLAG_READ  4
