#ಅನ್ತರ್ಭಾವ "types.h"
#ಅನ್ತರ್ಭಾವ "param.h"
#ಅನ್ತರ್ಭಾವ "memlayout.h"
#ಅನ್ತರ್ಭಾವ "riscv.h"
#ಅನ್ತರ್ಭಾವ "spinlock.h"
#ಅನ್ತರ್ಭಾವ "proc.h"
#ಅನ್ತರ್ಭಾವ "defs.h"
#ಅನ್ತರ್ಭಾವ "elf.h"

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ loadseg(pde_t *, uint64, ರಚನೆ inode *, uint, uint);

// map ELF permissions to PTE permission bits.
ಪೂರ್ಣಾಂಕ
flags2perm(ಪೂರ್ಣಾಂಕ flags)
{
  ಪೂರ್ಣಾಂಕ perm = 0;
  ಆದರೆ (flags & 0x1)
    perm = PTE_X;
  ಆದರೆ (flags & 0x2)
    perm |= PTE_W;
  ಹಿಂತಿರುಗಿ perm;
}

//
// the implementation of the exec() system call
//
ಪೂರ್ಣಾಂಕ
kexec(ಅಕ್ಷರ *ಮಾರ್ಗ, ಅಕ್ಷರ **argv)
{
  ಅಕ್ಷರ *s, *ಕೊನೆಯ;
  ಪೂರ್ಣಾಂಕ i, off;
  uint64 argc, sz = 0, sp, ustack[MAXARG], stackbase;
  ರಚನೆ elfhdr elf;
  ರಚನೆ inode *ip;
  ರಚನೆ proghdr ph;
  pagetable_t pagetable = 0, oldpagetable;
  ರಚನೆ proc *p = myproc();

  begin_op();

  // Open the executable file.
  ಆದರೆ ((ip = namei(ಮಾರ್ಗ)) == 0) {
    ಕೊನೆ_op();
    ಹಿಂತಿರುಗಿ -1;
  }
  ilock(ip);

  // Read the ELF header.
  ಆದರೆ (readi(ip, 0, (uint64)&elf, 0, ಗಾತ್ರ(elf)) != ಗಾತ್ರ(elf))
    ಹೋಗು bad;

  // Is this really an ELF file?
  ಆದರೆ (elf.magic != ELF_MAGIC)
    ಹೋಗು bad;

  ಆದರೆ ((pagetable = proc_pagetable(p)) == 0)
    ಹೋಗು bad;

  // Load program into memory.
  ಗಾಗಿ (i = 0, off = elf.phoff; i < elf.phnum; i++, off += ಗಾತ್ರ(ph)) {
    ಆದರೆ (readi(ip, 0, (uint64)&ph, off, ಗಾತ್ರ(ph)) != ಗಾತ್ರ(ph))
      ಹೋಗು bad;
    ಆದರೆ (ph.type != ELF_PROG_ತುಂಬು)
      ಮುಂದುವರಿ;
    ಆದರೆ (ph.memsz < ph.filesz)
      ಹೋಗು bad;
    ಆದರೆ (ph.vaddr + ph.memsz < ph.vaddr)
      ಹೋಗು bad;
    ಆದರೆ (ph.vaddr % PGSIZE != 0)
      ಹೋಗು bad;
    uint64 sz1;
    ಆದರೆ ((sz1 = uvmalloc(pagetable, sz, ph.vaddr + ph.memsz,
                        flags2perm(ph.flags))) == 0)
      ಹೋಗು bad;
    sz = sz1;
    ಆದರೆ (loadseg(pagetable, ph.vaddr, ip, ph.off, ph.filesz) < 0)
      ಹೋಗು bad;
  }
  iunlockput(ip);
  ಕೊನೆ_op();
  ip = 0;

  p = myproc();
  uint64 oldsz = p->sz;

  // Allocate some pages at the next page boundary.
  // Make the first inaccessible as a stack guard.
  // Use the rest as the user stack.
  sz = PGROUNDUP(sz);
  uint64 sz1;
  ಆದರೆ ((sz1 = uvmalloc(pagetable, sz, sz + (USERSTACK + 1) * PGSIZE, PTE_W)) ==
      0)
    ಹೋಗು bad;
  sz = sz1;
  uvmclear(pagetable, sz - (USERSTACK + 1) * PGSIZE);
  sp = sz;
  stackbase = sp - USERSTACK * PGSIZE;

  // Copy argument strings into new stack, remember their
  // addresses in ustack[].
  ಗಾಗಿ (argc = 0; argv[argc]; argc++) {
    sp -= ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(argv[argc]) + 1;
    sp -= sp % 16; // riscv sp must be 16-byte aligned
    ಆದರೆ (sp < stackbase)
      ಹೋಗು bad;
    ಆದರೆ (copyout(pagetable, sz, sp, argv[argc], ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(argv[argc]) + 1) < 0)
      ಹೋಗು bad;
    ustack[argc] = sp;
  }
  ustack[argc] = 0;

  // push a copy of ustack[], the array of argv[] pointers.
  sp -= (argc + 1) * ಗಾತ್ರ(uint64);
  sp -= sp % 16;
  ಆದರೆ (sp < stackbase)
    ಹೋಗು bad;
  ಆದರೆ (copyout(pagetable, sz, sp, (ಅಕ್ಷರ *)ustack, (argc + 1) * ಗಾತ್ರ(uint64)) <
      0)
    ಹೋಗು bad;

  // a0 and a1 contain arguments to user main(argc, argv)
  // argc is returned via the system call return
  // value, which goes in a0.
  p->trapframe->a1 = sp;

  // Save program name for debugging.
  ಗಾಗಿ (ಕೊನೆಯ = s = ಮಾರ್ಗ; *s; s++)
    ಆದರೆ (*s == '/')
      ಕೊನೆಯ = s + 1;
  safestrcpy(p->name, ಕೊನೆಯ, ಗಾತ್ರ(p->name));

  // Commit to the user image.
  oldpagetable = p->pagetable;
  p->pagetable = pagetable;
  p->sz = sz;
  p->trapframe->epc = elf.ನಮೂದು; // initial program counter = ulib.c:start()
  p->trapframe->sp = sp;         // initial stack pointer
  proc_freepagetable(oldpagetable, oldsz);

  ಹಿಂತಿರುಗಿ argc; // this ends up in a0, the first argument to main(argc, argv)

bad:
  ಆದರೆ (pagetable)
    proc_freepagetable(pagetable, sz);
  ಆದರೆ (ip) {
    iunlockput(ip);
    ಕೊನೆ_op();
  }
  ಹಿಂತಿರುಗಿ -1;
}

// Load an ELF program segment into pagetable at virtual address va.
// va must be page-aligned
// and the pages from va to va+sz must already be mapped.
// Returns 0 on success, -1 on failure.
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
loadseg(pagetable_t pagetable, uint64 va, ರಚನೆ inode *ip, uint ಸ್ಥಾನಾಂತರ,
        uint sz)
{
  uint i, n;
  uint64 pa;

  ಗಾಗಿ (i = 0; i < sz; i += PGSIZE) {
    pa = walkaddr(pagetable, va + i);
    ಆದರೆ (pa == 0)
      panic("loadseg: address should exist");
    ಆದರೆ (sz - i < PGSIZE)
      n = sz - i;
    ಇಲ್ಲದಿದ್ದರೆ
      n = PGSIZE;
    ಆದರೆ (readi(ip, 0, (uint64)pa, ಸ್ಥಾನಾಂತರ + i, n) != n)
      ಹಿಂತಿರುಗಿ -1;
  }

  ಹಿಂತಿರುಗಿ 0;
}
