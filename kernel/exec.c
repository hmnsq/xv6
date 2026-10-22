#അന്തര്ഭാവ "types.h"
#അന്തര്ഭാവ "param.h"
#അന്തര്ഭാവ "memlayout.h"
#അന്തര്ഭാവ "riscv.h"
#അന്തര്ഭാവ "spinlock.h"
#അന്തര്ഭാവ "proc.h"
#അന്തര്ഭാവ "defs.h"
#അന്തര്ഭാവ "elf.h"

സ്ഥിതം പൂർണ്ണസംഖ്യ loadseg(pde_t *, uint64, ഘടന inode *, uint, uint);

// map ELF permissions to PTE permission bits.
പൂർണ്ണസംഖ്യ
flags2perm(പൂർണ്ണസംഖ്യ flags)
{
  പൂർണ്ണസംഖ്യ perm = 0;
  എങ്കിൽ (flags & 0x1)
    perm = PTE_X;
  എങ്കിൽ (flags & 0x2)
    perm |= PTE_W;
  തിരികെ perm;
}

//
// the implementation of the exec() system call
//
പൂർണ്ണസംഖ്യ
kexec(അക്ഷരം *path, അക്ഷരം **argv)
{
  അക്ഷരം *s, *last;
  പൂർണ്ണസംഖ്യ i, off;
  uint64 argc, sz = 0, sp, ustack[MAXARG], stackbase;
  ഘടന elfhdr elf;
  ഘടന inode *ip;
  ഘടന proghdr ph;
  pagetable_t pagetable = 0, oldpagetable;
  ഘടന proc *p = myproc();

  begin_op();

  // Open the executable file.
  എങ്കിൽ ((ip = namei(path)) == 0) {
    end_op();
    തിരികെ -1;
  }
  ilock(ip);

  // Read the ELF header.
  എങ്കിൽ (readi(ip, 0, (uint64)&elf, 0, വലുപ്പം(elf)) != വലുപ്പം(elf))
    പോകുക bad;

  // Is this really an ELF file?
  എങ്കിൽ (elf.magic != ELF_MAGIC)
    പോകുക bad;

  എങ്കിൽ ((pagetable = proc_pagetable(p)) == 0)
    പോകുക bad;

  // Load program into memory.
  വേണ്ടി (i = 0, off = elf.phoff; i < elf.phnum; i++, off += വലുപ്പം(ph)) {
    എങ്കിൽ (readi(ip, 0, (uint64)&ph, off, വലുപ്പം(ph)) != വലുപ്പം(ph))
      പോകുക bad;
    എങ്കിൽ (ph.type != ELF_PROG_LOAD)
      തുടരുക;
    എങ്കിൽ (ph.memsz < ph.filesz)
      പോകുക bad;
    എങ്കിൽ (ph.vaddr + ph.memsz < ph.vaddr)
      പോകുക bad;
    എങ്കിൽ (ph.vaddr % PGSIZE != 0)
      പോകുക bad;
    uint64 sz1;
    എങ്കിൽ ((sz1 = uvmalloc(pagetable, sz, ph.vaddr + ph.memsz,
                        flags2perm(ph.flags))) == 0)
      പോകുക bad;
    sz = sz1;
    എങ്കിൽ (loadseg(pagetable, ph.vaddr, ip, ph.off, ph.filesz) < 0)
      പോകുക bad;
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
  എങ്കിൽ ((sz1 = uvmalloc(pagetable, sz, sz + (USERSTACK + 1) * PGSIZE, PTE_W)) ==
      0)
    പോകുക bad;
  sz = sz1;
  uvmclear(pagetable, sz - (USERSTACK + 1) * PGSIZE);
  sp = sz;
  stackbase = sp - USERSTACK * PGSIZE;

  // Copy argument strings into new stack, remember their
  // addresses in ustack[].
  വേണ്ടി (argc = 0; argv[argc]; argc++) {
    sp -= strlen(argv[argc]) + 1;
    sp -= sp % 16; // riscv sp must be 16-byte aligned
    എങ്കിൽ (sp < stackbase)
      പോകുക bad;
    എങ്കിൽ (copyout(pagetable, sz, sp, argv[argc], strlen(argv[argc]) + 1) < 0)
      പോകുക bad;
    ustack[argc] = sp;
  }
  ustack[argc] = 0;

  // push a copy of ustack[], the array of argv[] pointers.
  sp -= (argc + 1) * വലുപ്പം(uint64);
  sp -= sp % 16;
  എങ്കിൽ (sp < stackbase)
    പോകുക bad;
  എങ്കിൽ (copyout(pagetable, sz, sp, (അക്ഷരം *)ustack, (argc + 1) * വലുപ്പം(uint64)) <
      0)
    പോകുക bad;

  // a0 and a1 contain arguments to user main(argc, argv)
  // argc is returned via the system call return
  // value, which goes in a0.
  p->trapframe->a1 = sp;

  // Save program name for debugging.
  വേണ്ടി (last = s = path; *s; s++)
    എങ്കിൽ (*s == '/')
      last = s + 1;
  safestrcpy(p->name, last, വലുപ്പം(p->name));

  // Commit to the user image.
  oldpagetable = p->pagetable;
  p->pagetable = pagetable;
  p->sz = sz;
  p->trapframe->epc = elf.entry; // initial program counter = ulib.c:start()
  p->trapframe->sp = sp;         // initial stack pointer
  proc_freepagetable(oldpagetable, oldsz);

  തിരികെ argc; // this ends up in a0, the first argument to main(argc, argv)

bad:
  എങ്കിൽ (pagetable)
    proc_freepagetable(pagetable, sz);
  എങ്കിൽ (ip) {
    iunlockput(ip);
    end_op();
  }
  തിരികെ -1;
}

// Load an ELF program segment into pagetable at virtual address va.
// va must be page-aligned
// and the pages from va to va+sz must already be mapped.
// Returns 0 on success, -1 on failure.
സ്ഥിതം പൂർണ്ണസംഖ്യ
loadseg(pagetable_t pagetable, uint64 va, ഘടന inode *ip, uint offset,
        uint sz)
{
  uint i, n;
  uint64 pa;

  വേണ്ടി (i = 0; i < sz; i += PGSIZE) {
    pa = walkaddr(pagetable, va + i);
    എങ്കിൽ (pa == 0)
      panic("loadseg: address should exist");
    എങ്കിൽ (sz - i < PGSIZE)
      n = sz - i;
    അല്ലെങ്കിൽ
      n = PGSIZE;
    എങ്കിൽ (readi(ip, 0, (uint64)pa, offset + i, n) != n)
      തിരികെ -1;
  }

  തിരികെ 0;
}
