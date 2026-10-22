#अन्तर्भाव "types.h"
#अन्तर्भाव "param.h"
#अन्तर्भाव "memlayout.h"
#अन्तर्भाव "riscv.h"
#अन्तर्भाव "spinlock.h"
#अन्तर्भाव "proc.h"
#अन्तर्भाव "defs.h"
#अन्तर्भाव "elf.h"

स्थिर पूर्णाङ्क loadseg(pde_t *, uint64, संरचना inode *, uint, uint);

// map ELF permissions to PTE permission bits.
पूर्णाङ्क
flags2perm(पूर्णाङ्क flags)
{
  पूर्णाङ्क perm = 0;
  जइ (flags & 0x1)
    perm = PTE_X;
  जइ (flags & 0x2)
    perm |= PTE_W;
  प्रत्यागम perm;
}

//
// the implementation of the exec() system call
//
पूर्णाङ्क
kexec(वर्ण *path, वर्ण **argv)
{
  वर्ण *s, *last;
  पूर्णाङ्क i, off;
  uint64 argc, sz = 0, sp, ustack[MAXARG], stackbase;
  संरचना elfhdr elf;
  संरचना inode *ip;
  संरचना proghdr ph;
  pagetable_t pagetable = 0, oldpagetable;
  संरचना proc *p = myproc();

  begin_op();

  // Open the executable file.
  जइ ((ip = namei(path)) == 0) {
    end_op();
    प्रत्यागम -1;
  }
  ilock(ip);

  // Read the ELF header.
  जइ (readi(ip, 0, (uint64)&elf, 0, परिमाण(elf)) != परिमाण(elf))
    गच्छ bad;

  // Is this really an ELF file?
  जइ (elf.magic != ELF_MAGIC)
    गच्छ bad;

  जइ ((pagetable = proc_pagetable(p)) == 0)
    गच्छ bad;

  // Load program into memory.
  प्रति (i = 0, off = elf.phoff; i < elf.phnum; i++, off += परिमाण(ph)) {
    जइ (readi(ip, 0, (uint64)&ph, off, परिमाण(ph)) != परिमाण(ph))
      गच्छ bad;
    जइ (ph.type != ELF_PROG_LOAD)
      अनुवर्त;
    जइ (ph.memsz < ph.filesz)
      गच्छ bad;
    जइ (ph.vaddr + ph.memsz < ph.vaddr)
      गच्छ bad;
    जइ (ph.vaddr % PGSIZE != 0)
      गच्छ bad;
    uint64 sz1;
    जइ ((sz1 = uvmalloc(pagetable, sz, ph.vaddr + ph.memsz,
                        flags2perm(ph.flags))) == 0)
      गच्छ bad;
    sz = sz1;
    जइ (loadseg(pagetable, ph.vaddr, ip, ph.off, ph.filesz) < 0)
      गच्छ bad;
  }
  iunlockput(ip);
  end_op();
  ip = 0;

  p = myproc();
  uint64 oldsz = p->sz;

  // Allocate some pages at the next page boundary.
  // Make the first inaccessible as a stack guard.
  // Use the rest as the user stack.
  sz = PGROUNDUP(sz);
  uint64 sz1;
  जइ ((sz1 = uvmalloc(pagetable, sz, sz + (USERSTACK + 1) * PGSIZE, PTE_W)) ==
      0)
    गच्छ bad;
  sz = sz1;
  uvmclear(pagetable, sz - (USERSTACK + 1) * PGSIZE);
  sp = sz;
  stackbase = sp - USERSTACK * PGSIZE;

  // Copy argument strings into new stack, remember their
  // addresses in ustack[].
  प्रति (argc = 0; argv[argc]; argc++) {
    sp -= strlen(argv[argc]) + 1;
    sp -= sp % 16; // riscv sp must be 16-byte aligned
    जइ (sp < stackbase)
      गच्छ bad;
    जइ (copyout(pagetable, sz, sp, argv[argc], strlen(argv[argc]) + 1) < 0)
      गच्छ bad;
    ustack[argc] = sp;
  }
  ustack[argc] = 0;

  // push a copy of ustack[], the array of argv[] pointers.
  sp -= (argc + 1) * परिमाण(uint64);
  sp -= sp % 16;
  जइ (sp < stackbase)
    गच्छ bad;
  जइ (copyout(pagetable, sz, sp, (वर्ण *)ustack, (argc + 1) * परिमाण(uint64)) <
      0)
    गच्छ bad;

  // a0 and a1 contain arguments to user main(argc, argv)
  // argc is returned via the system call return
  // value, which goes in a0.
  p->trapframe->a1 = sp;

  // Save program name for debugging.
  प्रति (last = s = path; *s; s++)
    जइ (*s == '/')
      last = s + 1;
  safestrcpy(p->name, last, परिमाण(p->name));

  // Commit to the user image.
  oldpagetable = p->pagetable;
  p->pagetable = pagetable;
  p->sz = sz;
  p->trapframe->epc = elf.entry; // initial program counter = ulib.c:start()
  p->trapframe->sp = sp;         // initial stack pointer
  proc_freepagetable(oldpagetable, oldsz);

  प्रत्यागम argc; // this ends up in a0, the first argument to main(argc, argv)

bad:
  जइ (pagetable)
    proc_freepagetable(pagetable, sz);
  जइ (ip) {
    iunlockput(ip);
    end_op();
  }
  प्रत्यागम -1;
}

// Load an ELF program segment into pagetable at virtual address va.
// va must be page-aligned
// and the pages from va to va+sz must already be mapped.
// Returns 0 on success, -1 on failure.
स्थिर पूर्णाङ्क
loadseg(pagetable_t pagetable, uint64 va, संरचना inode *ip, uint offset,
        uint sz)
{
  uint i, n;
  uint64 pa;

  प्रति (i = 0; i < sz; i += PGSIZE) {
    pa = walkaddr(pagetable, va + i);
    जइ (pa == 0)
      panic("loadseg: address should exist");
    जइ (sz - i < PGSIZE)
      n = sz - i;
    अण्णहा
      n = PGSIZE;
    जइ (readi(ip, 0, (uint64)pa, offset + i, n) != n)
      प्रत्यागम -1;
  }

  प्रत्यागम 0;
}
