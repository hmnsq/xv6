#અન્તર્ભાવ "types.h"
#અન્તર્ભાવ "param.h"
#અન્તર્ભાવ "memlayout.h"
#અન્તર્ભાવ "riscv.h"
#અન્તર્ભાવ "spinlock.h"
#અન્તર્ભાવ "proc.h"
#અન્તર્ભાવ "defs.h"
#અન્તર્ભાવ "elf.h"

સ્થિર પૂર્ણાંક loadseg(pde_t *, uint64, સંરચના inode *, uint, uint);

// map ELF permissions to PTE permission bits.
પૂર્ણાંક
flags2perm(પૂર્ણાંક flags)
{
  પૂર્ણાંક perm = 0;
  જો (flags & 0x1)
    perm = PTE_X;
  જો (flags & 0x2)
    perm |= PTE_W;
  પાછા perm;
}

//
// the implementation of the exec() system call
//
પૂર્ણાંક
kexec(અક્ષર *path, અક્ષર **argv)
{
  અક્ષર *s, *last;
  પૂર્ણાંક i, off;
  uint64 argc, sz = 0, sp, ustack[MAXARG], stackbase;
  સંરચના elfhdr elf;
  સંરચના inode *ip;
  સંરચના proghdr ph;
  pagetable_t pagetable = 0, oldpagetable;
  સંરચના proc *p = myproc();

  begin_op();

  // Open the executable file.
  જો ((ip = namei(path)) == 0) {
    end_op();
    પાછા -1;
  }
  ilock(ip);

  // Read the ELF header.
  જો (readi(ip, 0, (uint64)&elf, 0, કદ(elf)) != કદ(elf))
    જાઓ bad;

  // Is this really an ELF file?
  જો (elf.magic != ELF_MAGIC)
    જાઓ bad;

  જો ((pagetable = proc_pagetable(p)) == 0)
    જાઓ bad;

  // Load program into memory.
  માટે (i = 0, off = elf.phoff; i < elf.phnum; i++, off += કદ(ph)) {
    જો (readi(ip, 0, (uint64)&ph, off, કદ(ph)) != કદ(ph))
      જાઓ bad;
    જો (ph.type != ELF_PROG_LOAD)
      ચાલુ;
    જો (ph.memsz < ph.filesz)
      જાઓ bad;
    જો (ph.vaddr + ph.memsz < ph.vaddr)
      જાઓ bad;
    જો (ph.vaddr % PGSIZE != 0)
      જાઓ bad;
    uint64 sz1;
    જો ((sz1 = uvmalloc(pagetable, sz, ph.vaddr + ph.memsz,
                        flags2perm(ph.flags))) == 0)
      જાઓ bad;
    sz = sz1;
    જો (loadseg(pagetable, ph.vaddr, ip, ph.off, ph.filesz) < 0)
      જાઓ bad;
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
  જો ((sz1 = uvmalloc(pagetable, sz, sz + (USERSTACK + 1) * PGSIZE, PTE_W)) ==
      0)
    જાઓ bad;
  sz = sz1;
  uvmclear(pagetable, sz - (USERSTACK + 1) * PGSIZE);
  sp = sz;
  stackbase = sp - USERSTACK * PGSIZE;

  // Copy argument strings into new stack, remember their
  // addresses in ustack[].
  માટે (argc = 0; argv[argc]; argc++) {
    sp -= strlen(argv[argc]) + 1;
    sp -= sp % 16; // riscv sp must be 16-byte aligned
    જો (sp < stackbase)
      જાઓ bad;
    જો (copyout(pagetable, sz, sp, argv[argc], strlen(argv[argc]) + 1) < 0)
      જાઓ bad;
    ustack[argc] = sp;
  }
  ustack[argc] = 0;

  // push a copy of ustack[], the array of argv[] pointers.
  sp -= (argc + 1) * કદ(uint64);
  sp -= sp % 16;
  જો (sp < stackbase)
    જાઓ bad;
  જો (copyout(pagetable, sz, sp, (અક્ષર *)ustack, (argc + 1) * કદ(uint64)) <
      0)
    જાઓ bad;

  // a0 and a1 contain arguments to user main(argc, argv)
  // argc is returned via the system call return
  // value, which goes in a0.
  p->trapframe->a1 = sp;

  // Save program name for debugging.
  માટે (last = s = path; *s; s++)
    જો (*s == '/')
      last = s + 1;
  safestrcpy(p->name, last, કદ(p->name));

  // Commit to the user image.
  oldpagetable = p->pagetable;
  p->pagetable = pagetable;
  p->sz = sz;
  p->trapframe->epc = elf.entry; // initial program counter = ulib.c:start()
  p->trapframe->sp = sp;         // initial stack pointer
  proc_freepagetable(oldpagetable, oldsz);

  પાછા argc; // this ends up in a0, the first argument to main(argc, argv)

bad:
  જો (pagetable)
    proc_freepagetable(pagetable, sz);
  જો (ip) {
    iunlockput(ip);
    end_op();
  }
  પાછા -1;
}

// Load an ELF program segment into pagetable at virtual address va.
// va must be page-aligned
// and the pages from va to va+sz must already be mapped.
// Returns 0 on success, -1 on failure.
સ્થિર પૂર્ણાંક
loadseg(pagetable_t pagetable, uint64 va, સંરચના inode *ip, uint offset,
        uint sz)
{
  uint i, n;
  uint64 pa;

  માટે (i = 0; i < sz; i += PGSIZE) {
    pa = walkaddr(pagetable, va + i);
    જો (pa == 0)
      panic("loadseg: address should exist");
    જો (sz - i < PGSIZE)
      n = sz - i;
    નહીંતર
      n = PGSIZE;
    જો (readi(ip, 0, (uint64)pa, offset + i, n) != n)
      પાછા -1;
  }

  પાછા 0;
}
