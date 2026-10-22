#అన్తర్భావ "types.h"
#అన్తర్భావ "param.h"
#అన్తర్భావ "memlayout.h"
#అన్తర్భావ "riscv.h"
#అన్తర్భావ "spinlock.h"
#అన్తర్భావ "proc.h"
#అన్తర్భావ "defs.h"
#అన్తర్భావ "elf.h"

స్థిర పూర్ణాంకం loadseg(pde_t *, uint64, నిర్మాణం inode *, uint, uint);

// map ELF permissions to PTE permission bits.
పూర్ణాంకం
flags2perm(పూర్ణాంకం flags)
{
  పూర్ణాంకం perm = 0;
  ఒకవేళ (flags & 0x1)
    perm = PTE_X;
  ఒకవేళ (flags & 0x2)
    perm |= PTE_W;
  తిరిగి perm;
}

//
// the implementation of the exec() system call
//
పూర్ణాంకం
kexec(అక్షరం *మార్గం, అక్షరం **argv)
{
  అక్షరం *s, *చివరి;
  పూర్ణాంకం i, off;
  uint64 argc, sz = 0, sp, ustack[MAXARG], stackbase;
  నిర్మాణం elfhdr elf;
  నిర్మాణం inode *ip;
  నిర్మాణం proghdr ph;
  pagetable_t pagetable = 0, oldpagetable;
  నిర్మాణం proc *p = myproc();

  begin_op();

  // Open the executable file.
  ఒకవేళ ((ip = namei(మార్గం)) == 0) {
    చివర_op();
    తిరిగి -1;
  }
  ilock(ip);

  // Read the ELF header.
  ఒకవేళ (readi(ip, 0, (uint64)&elf, 0, పరిమాణం(elf)) != పరిమాణం(elf))
    వెళ్ళు bad;

  // Is this really an ELF file?
  ఒకవేళ (elf.magic != ELF_MAGIC)
    వెళ్ళు bad;

  ఒకవేళ ((pagetable = proc_pagetable(p)) == 0)
    వెళ్ళు bad;

  // Load program into memory.
  కొరకు (i = 0, off = elf.phoff; i < elf.phnum; i++, off += పరిమాణం(ph)) {
    ఒకవేళ (readi(ip, 0, (uint64)&ph, off, పరిమాణం(ph)) != పరిమాణం(ph))
      వెళ్ళు bad;
    ఒకవేళ (ph.type != ELF_PROG_ఎక్కించు)
      కొనసాగు;
    ఒకవేళ (ph.memsz < ph.filesz)
      వెళ్ళు bad;
    ఒకవేళ (ph.vaddr + ph.memsz < ph.vaddr)
      వెళ్ళు bad;
    ఒకవేళ (ph.vaddr % PGSIZE != 0)
      వెళ్ళు bad;
    uint64 sz1;
    ఒకవేళ ((sz1 = uvmalloc(pagetable, sz, ph.vaddr + ph.memsz,
                        flags2perm(ph.flags))) == 0)
      వెళ్ళు bad;
    sz = sz1;
    ఒకవేళ (loadseg(pagetable, ph.vaddr, ip, ph.off, ph.filesz) < 0)
      వెళ్ళు bad;
  }
  iunlockput(ip);
  చివర_op();
  ip = 0;

  p = myproc();
  uint64 oldsz = p->sz;

  // Allocate some pages at the next page boundary.
  // Make the first inaccessible as a stack guard.
  // Use the rest as the user stack.
  sz = PGROUNDUP(sz);
  uint64 sz1;
  ఒకవేళ ((sz1 = uvmalloc(pagetable, sz, sz + (USERSTACK + 1) * PGSIZE, PTE_W)) ==
      0)
    వెళ్ళు bad;
  sz = sz1;
  uvmclear(pagetable, sz - (USERSTACK + 1) * PGSIZE);
  sp = sz;
  stackbase = sp - USERSTACK * PGSIZE;

  // Copy argument strings into new stack, remember their
  // addresses in ustack[].
  కొరకు (argc = 0; argv[argc]; argc++) {
    sp -= అక్షరశ్రేణి_పొడవు(argv[argc]) + 1;
    sp -= sp % 16; // riscv sp must be 16-byte aligned
    ఒకవేళ (sp < stackbase)
      వెళ్ళు bad;
    ఒకవేళ (copyout(pagetable, sz, sp, argv[argc], అక్షరశ్రేణి_పొడవు(argv[argc]) + 1) < 0)
      వెళ్ళు bad;
    ustack[argc] = sp;
  }
  ustack[argc] = 0;

  // push a copy of ustack[], the array of argv[] pointers.
  sp -= (argc + 1) * పరిమాణం(uint64);
  sp -= sp % 16;
  ఒకవేళ (sp < stackbase)
    వెళ్ళు bad;
  ఒకవేళ (copyout(pagetable, sz, sp, (అక్షరం *)ustack, (argc + 1) * పరిమాణం(uint64)) <
      0)
    వెళ్ళు bad;

  // a0 and a1 contain arguments to user main(argc, argv)
  // argc is returned via the system call return
  // value, which goes in a0.
  p->trapframe->a1 = sp;

  // Save program name for debugging.
  కొరకు (చివరి = s = మార్గం; *s; s++)
    ఒకవేళ (*s == '/')
      చివరి = s + 1;
  safestrcpy(p->name, చివరి, పరిమాణం(p->name));

  // Commit to the user image.
  oldpagetable = p->pagetable;
  p->pagetable = pagetable;
  p->sz = sz;
  p->trapframe->epc = elf.నమోదు_2; // initial program counter = ulib.c:start()
  p->trapframe->sp = sp;         // initial stack pointer
  proc_freepagetable(oldpagetable, oldsz);

  తిరిగి argc; // this ends up in a0, the first argument to main(argc, argv)

bad:
  ఒకవేళ (pagetable)
    proc_freepagetable(pagetable, sz);
  ఒకవేళ (ip) {
    iunlockput(ip);
    చివర_op();
  }
  తిరిగి -1;
}

// Load an ELF program segment into pagetable at virtual address va.
// va must be page-aligned
// and the pages from va to va+sz must already be mapped.
// Returns 0 on success, -1 on failure.
స్థిర పూర్ణాంకం
loadseg(pagetable_t pagetable, uint64 va, నిర్మాణం inode *ip, uint స్థానభ్రంశం,
        uint sz)
{
  uint i, n;
  uint64 pa;

  కొరకు (i = 0; i < sz; i += PGSIZE) {
    pa = walkaddr(pagetable, va + i);
    ఒకవేళ (pa == 0)
      panic("loadseg: address should exist");
    ఒకవేళ (sz - i < PGSIZE)
      n = sz - i;
    లేకపోతే
      n = PGSIZE;
    ఒకవేళ (readi(ip, 0, (uint64)pa, స్థానభ్రంశం + i, n) != n)
      తిరిగి -1;
  }

  తిరిగి 0;
}
