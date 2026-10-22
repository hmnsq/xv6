#अन्तर्भाव "types.h"
#अन्तर्भाव "param.h"
#अन्तर्भाव "memlayout.h"
#अन्तर्भाव "riscv.h"
#अन्तर्भाव "spinlock.h"
#अन्तर्भाव "proc.h"
#अन्तर्भाव "defs.h"
#अन्तर्भाव "elf.h"

स्थिर पूर्णांक loadseg(pde_t *, uint64, संरचना inode *, uint, uint);

// map ELF permissions to PTE permission bits.
पूर्णांक
flags2perm(पूर्णांक flags)
{
  पूर्णांक perm = 0;
  जर (flags & 0x1)
    perm = PTE_X;
  जर (flags & 0x2)
    perm |= PTE_W;
  परत perm;
}

//
// the implementation of the exec() system call
//
पूर्णांक
kexec(अक्षर *path, अक्षर **argv)
{
  अक्षर *s, *last;
  पूर्णांक i, off;
  uint64 argc, sz = 0, sp, ustack[MAXARG], stackbase;
  संरचना elfhdr elf;
  संरचना inode *ip;
  संरचना proghdr ph;
  pagetable_t pagetable = 0, oldpagetable;
  संरचना proc *p = myproc();

  begin_op();

  // Open the executable file.
  जर ((ip = namei(path)) == 0) {
    end_op();
    परत -1;
  }
  ilock(ip);

  // Read the ELF header.
  जर (readi(ip, 0, (uint64)&elf, 0, आकार(elf)) != आकार(elf))
    जा bad;

  // Is this really an ELF file?
  जर (elf.magic != ELF_MAGIC)
    जा bad;

  जर ((pagetable = proc_pagetable(p)) == 0)
    जा bad;

  // Load program into memory.
  साठी (i = 0, off = elf.phoff; i < elf.phnum; i++, off += आकार(ph)) {
    जर (readi(ip, 0, (uint64)&ph, off, आकार(ph)) != आकार(ph))
      जा bad;
    जर (ph.type != ELF_PROG_LOAD)
      सुरूठेवा;
    जर (ph.memsz < ph.filesz)
      जा bad;
    जर (ph.vaddr + ph.memsz < ph.vaddr)
      जा bad;
    जर (ph.vaddr % PGSIZE != 0)
      जा bad;
    uint64 sz1;
    जर ((sz1 = uvmalloc(pagetable, sz, ph.vaddr + ph.memsz,
                        flags2perm(ph.flags))) == 0)
      जा bad;
    sz = sz1;
    जर (loadseg(pagetable, ph.vaddr, ip, ph.off, ph.filesz) < 0)
      जा bad;
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
  जर ((sz1 = uvmalloc(pagetable, sz, sz + (USERSTACK + 1) * PGSIZE, PTE_W)) ==
      0)
    जा bad;
  sz = sz1;
  uvmclear(pagetable, sz - (USERSTACK + 1) * PGSIZE);
  sp = sz;
  stackbase = sp - USERSTACK * PGSIZE;

  // Copy argument strings into new stack, remember their
  // addresses in ustack[].
  साठी (argc = 0; argv[argc]; argc++) {
    sp -= म_माप(argv[argc]) + 1;
    sp -= sp % 16; // riscv sp must be 16-byte aligned
    जर (sp < stackbase)
      जा bad;
    जर (copyout(pagetable, sz, sp, argv[argc], म_माप(argv[argc]) + 1) < 0)
      जा bad;
    ustack[argc] = sp;
  }
  ustack[argc] = 0;

  // push a copy of ustack[], the array of argv[] pointers.
  sp -= (argc + 1) * आकार(uint64);
  sp -= sp % 16;
  जर (sp < stackbase)
    जा bad;
  जर (copyout(pagetable, sz, sp, (अक्षर *)ustack, (argc + 1) * आकार(uint64)) <
      0)
    जा bad;

  // a0 and a1 contain arguments to user main(argc, argv)
  // argc is returned via the system call return
  // value, which goes in a0.
  p->trapframe->a1 = sp;

  // Save program name for debugging.
  साठी (last = s = path; *s; s++)
    जर (*s == '/')
      last = s + 1;
  safestrcpy(p->name, last, आकार(p->name));

  // Commit to the user image.
  oldpagetable = p->pagetable;
  p->pagetable = pagetable;
  p->sz = sz;
  p->trapframe->epc = elf.entry; // initial program counter = ulib.c:start()
  p->trapframe->sp = sp;         // initial stack pointer
  proc_freepagetable(oldpagetable, oldsz);

  परत argc; // this ends up in a0, the first argument to main(argc, argv)

bad:
  जर (pagetable)
    proc_freepagetable(pagetable, sz);
  जर (ip) {
    iunlockput(ip);
    end_op();
  }
  परत -1;
}

// Load an ELF program segment into pagetable at virtual address va.
// va must be page-aligned
// and the pages from va to va+sz must already be mapped.
// Returns 0 on success, -1 on failure.
स्थिर पूर्णांक
loadseg(pagetable_t pagetable, uint64 va, संरचना inode *ip, uint offset,
        uint sz)
{
  uint i, n;
  uint64 pa;

  साठी (i = 0; i < sz; i += PGSIZE) {
    pa = walkaddr(pagetable, va + i);
    जर (pa == 0)
      panic("loadseg: address should exist");
    जर (sz - i < PGSIZE)
      n = sz - i;
    अन्यथा
      n = PGSIZE;
    जर (readi(ip, 0, (uint64)pa, offset + i, n) != n)
      परत -1;
  }

  परत 0;
}
