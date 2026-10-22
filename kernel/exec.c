#ಅನ್ತರ್ಭಾವ "types.h"
#ಅನ್ತರ್ಭಾವ "param.h"
#ಅನ್ತರ್ಭಾವ "memlayout.h"
#ಅನ್ತರ್ಭಾವ "riscv.h"
#ಅನ್ತರ್ಭಾವ "spinlock.h"
#ಅನ್ತರ್ಭಾವ "proc.h"
#ಅನ್ತರ್ಭಾವ "defs.h"
#ಅನ್ತರ್ಭಾವ "elf.h"

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ loadseg(pde_t *, uint64, ಸಂರಚನಾ inode *, uint, uint);

// map ELF permissions to PTE permission bits.
ಪೂರ್ಣಾಙ್ಕ
flags2perm(ಪೂರ್ಣಾಙ್ಕ flags)
{
  ಪೂರ್ಣಾಙ್ಕ perm = 0;
  ಯದಿ (flags & 0x1)
    perm = PTE_X;
  ಯದಿ (flags & 0x2)
    perm |= PTE_W;
  ಪ್ರತ್ಯಾಗಮ perm;
}

//
// the implementation of the exec() system call
//
ಪೂರ್ಣಾಙ್ಕ
kexec(ವರ್ಣ *path, ವರ್ಣ **argv)
{
  ವರ್ಣ *s, *last;
  ಪೂರ್ಣಾಙ್ಕ i, off;
  uint64 argc, sz = 0, sp, ustack[MAXARG], stackbase;
  ಸಂರಚನಾ elfhdr elf;
  ಸಂರಚನಾ inode *ip;
  ಸಂರಚನಾ proghdr ph;
  pagetable_t pagetable = 0, oldpagetable;
  ಸಂರಚನಾ proc *p = myproc();

  begin_op();

  // Open the executable file.
  ಯದಿ ((ip = namei(path)) == 0) {
    end_op();
    ಪ್ರತ್ಯಾಗಮ -1;
  }
  ilock(ip);

  // Read the ELF header.
  ಯದಿ (readi(ip, 0, (uint64)&elf, 0, ಪರಿಮಾಣ(elf)) != ಪರಿಮಾಣ(elf))
    ಗಚ್ಛ bad;

  // Is this really an ELF file?
  ಯದಿ (elf.magic != ELF_MAGIC)
    ಗಚ್ಛ bad;

  ಯದಿ ((pagetable = proc_pagetable(p)) == 0)
    ಗಚ್ಛ bad;

  // Load program into memory.
  ಪ್ರತಿ (i = 0, off = elf.phoff; i < elf.phnum; i++, off += ಪರಿಮಾಣ(ph)) {
    ಯದಿ (readi(ip, 0, (uint64)&ph, off, ಪರಿಮಾಣ(ph)) != ಪರಿಮಾಣ(ph))
      ಗಚ್ಛ bad;
    ಯದಿ (ph.type != ELF_PROG_LOAD)
      ಅನುವರ್ತ;
    ಯದಿ (ph.memsz < ph.filesz)
      ಗಚ್ಛ bad;
    ಯದಿ (ph.vaddr + ph.memsz < ph.vaddr)
      ಗಚ್ಛ bad;
    ಯದಿ (ph.vaddr % PGSIZE != 0)
      ಗಚ್ಛ bad;
    uint64 sz1;
    ಯದಿ ((sz1 = uvmalloc(pagetable, sz, ph.vaddr + ph.memsz,
                        flags2perm(ph.flags))) == 0)
      ಗಚ್ಛ bad;
    sz = sz1;
    ಯದಿ (loadseg(pagetable, ph.vaddr, ip, ph.off, ph.filesz) < 0)
      ಗಚ್ಛ bad;
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
  ಯದಿ ((sz1 = uvmalloc(pagetable, sz, sz + (USERSTACK + 1) * PGSIZE, PTE_W)) ==
      0)
    ಗಚ್ಛ bad;
  sz = sz1;
  uvmclear(pagetable, sz - (USERSTACK + 1) * PGSIZE);
  sp = sz;
  stackbase = sp - USERSTACK * PGSIZE;

  // Copy argument strings into new stack, remember their
  // addresses in ustack[].
  ಪ್ರತಿ (argc = 0; argv[argc]; argc++) {
    sp -= strlen(argv[argc]) + 1;
    sp -= sp % 16; // riscv sp must be 16-byte aligned
    ಯದಿ (sp < stackbase)
      ಗಚ್ಛ bad;
    ಯದಿ (copyout(pagetable, sz, sp, argv[argc], strlen(argv[argc]) + 1) < 0)
      ಗಚ್ಛ bad;
    ustack[argc] = sp;
  }
  ustack[argc] = 0;

  // push a copy of ustack[], the array of argv[] pointers.
  sp -= (argc + 1) * ಪರಿಮಾಣ(uint64);
  sp -= sp % 16;
  ಯದಿ (sp < stackbase)
    ಗಚ್ಛ bad;
  ಯದಿ (copyout(pagetable, sz, sp, (ವರ್ಣ *)ustack, (argc + 1) * ಪರಿಮಾಣ(uint64)) <
      0)
    ಗಚ್ಛ bad;

  // a0 and a1 contain arguments to user main(argc, argv)
  // argc is returned via the system call return
  // value, which goes in a0.
  p->trapframe->a1 = sp;

  // Save program name for debugging.
  ಪ್ರತಿ (last = s = path; *s; s++)
    ಯದಿ (*s == '/')
      last = s + 1;
  safestrcpy(p->name, last, ಪರಿಮಾಣ(p->name));

  // Commit to the user image.
  oldpagetable = p->pagetable;
  p->pagetable = pagetable;
  p->sz = sz;
  p->trapframe->epc = elf.entry; // initial program counter = ulib.c:start()
  p->trapframe->sp = sp;         // initial stack pointer
  proc_freepagetable(oldpagetable, oldsz);

  ಪ್ರತ್ಯಾಗಮ argc; // this ends up in a0, the first argument to main(argc, argv)

bad:
  ಯದಿ (pagetable)
    proc_freepagetable(pagetable, sz);
  ಯದಿ (ip) {
    iunlockput(ip);
    end_op();
  }
  ಪ್ರತ್ಯಾಗಮ -1;
}

// Load an ELF program segment into pagetable at virtual address va.
// va must be page-aligned
// and the pages from va to va+sz must already be mapped.
// Returns 0 on success, -1 on failure.
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
loadseg(pagetable_t pagetable, uint64 va, ಸಂರಚನಾ inode *ip, uint offset,
        uint sz)
{
  uint i, n;
  uint64 pa;

  ಪ್ರತಿ (i = 0; i < sz; i += PGSIZE) {
    pa = walkaddr(pagetable, va + i);
    ಯದಿ (pa == 0)
      panic("loadseg: address should exist");
    ಯದಿ (sz - i < PGSIZE)
      n = sz - i;
    ಅನ್ಯಥಾ
      n = PGSIZE;
    ಯದಿ (readi(ip, 0, (uint64)pa, offset + i, n) != n)
      ಪ್ರತ್ಯಾಗಮ -1;
  }

  ಪ್ರತ್ಯಾಗಮ 0;
}
