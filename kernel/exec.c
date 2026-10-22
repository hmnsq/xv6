#ଅନ୍ତର୍ଭାଵ "types.h"
#ଅନ୍ତର୍ଭାଵ "param.h"
#ଅନ୍ତର୍ଭାଵ "memlayout.h"
#ଅନ୍ତର୍ଭାଵ "riscv.h"
#ଅନ୍ତର୍ଭାଵ "spinlock.h"
#ଅନ୍ତର୍ଭାଵ "proc.h"
#ଅନ୍ତର୍ଭାଵ "defs.h"
#ଅନ୍ତର୍ଭାଵ "elf.h"

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା loadseg(pde_t *, uint64, ସଂରଚନା inode *, uint, uint);

// map ELF permissions to PTE permission bits.
ପୂର୍ଣ୍ଣସଂଖ୍ୟା
flags2perm(ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା perm = 0;
  ଯଦି (flags & 0x1)
    perm = PTE_X;
  ଯଦି (flags & 0x2)
    perm |= PTE_W;
  ଫେରନ୍ତୁ perm;
}

//
// the implementation of the exec() system call
//
ପୂର୍ଣ୍ଣସଂଖ୍ୟା
kexec(ଅକ୍ଷର *path, ଅକ୍ଷର **argv)
{
  ଅକ୍ଷର *s, *last;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା i, off;
  uint64 argc, sz = 0, sp, ustack[MAXARG], stackbase;
  ସଂରଚନା elfhdr elf;
  ସଂରଚନା inode *ip;
  ସଂରଚନା proghdr ph;
  pagetable_t pagetable = 0, oldpagetable;
  ସଂରଚନା proc *p = myproc();

  begin_op();

  // Open the executable file.
  ଯଦି ((ip = namei(path)) == 0) {
    end_op();
    ଫେରନ୍ତୁ -1;
  }
  ilock(ip);

  // Read the ELF header.
  ଯଦି (readi(ip, 0, (uint64)&elf, 0, ଆକାର(elf)) != ଆକାର(elf))
    ଯାଅ bad;

  // Is this really an ELF file?
  ଯଦି (elf.magic != ELF_MAGIC)
    ଯାଅ bad;

  ଯଦି ((pagetable = proc_pagetable(p)) == 0)
    ଯାଅ bad;

  // Load program into memory.
  ପାଇଁ (i = 0, off = elf.phoff; i < elf.phnum; i++, off += ଆକାର(ph)) {
    ଯଦି (readi(ip, 0, (uint64)&ph, off, ଆକାର(ph)) != ଆକାର(ph))
      ଯାଅ bad;
    ଯଦି (ph.type != ELF_PROG_LOAD)
      ଜାରି;
    ଯଦି (ph.memsz < ph.filesz)
      ଯାଅ bad;
    ଯଦି (ph.vaddr + ph.memsz < ph.vaddr)
      ଯାଅ bad;
    ଯଦି (ph.vaddr % PGSIZE != 0)
      ଯାଅ bad;
    uint64 sz1;
    ଯଦି ((sz1 = uvmalloc(pagetable, sz, ph.vaddr + ph.memsz,
                        flags2perm(ph.flags))) == 0)
      ଯାଅ bad;
    sz = sz1;
    ଯଦି (loadseg(pagetable, ph.vaddr, ip, ph.off, ph.filesz) < 0)
      ଯାଅ bad;
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
  ଯଦି ((sz1 = uvmalloc(pagetable, sz, sz + (USERSTACK + 1) * PGSIZE, PTE_W)) ==
      0)
    ଯାଅ bad;
  sz = sz1;
  uvmclear(pagetable, sz - (USERSTACK + 1) * PGSIZE);
  sp = sz;
  stackbase = sp - USERSTACK * PGSIZE;

  // Copy argument strings into new stack, remember their
  // addresses in ustack[].
  ପାଇଁ (argc = 0; argv[argc]; argc++) {
    sp -= strlen(argv[argc]) + 1;
    sp -= sp % 16; // riscv sp must be 16-byte aligned
    ଯଦି (sp < stackbase)
      ଯାଅ bad;
    ଯଦି (copyout(pagetable, sz, sp, argv[argc], strlen(argv[argc]) + 1) < 0)
      ଯାଅ bad;
    ustack[argc] = sp;
  }
  ustack[argc] = 0;

  // push a copy of ustack[], the array of argv[] pointers.
  sp -= (argc + 1) * ଆକାର(uint64);
  sp -= sp % 16;
  ଯଦି (sp < stackbase)
    ଯାଅ bad;
  ଯଦି (copyout(pagetable, sz, sp, (ଅକ୍ଷର *)ustack, (argc + 1) * ଆକାର(uint64)) <
      0)
    ଯାଅ bad;

  // a0 and a1 contain arguments to user main(argc, argv)
  // argc is returned via the system call return
  // value, which goes in a0.
  p->trapframe->a1 = sp;

  // Save program name for debugging.
  ପାଇଁ (last = s = path; *s; s++)
    ଯଦି (*s == '/')
      last = s + 1;
  safestrcpy(p->name, last, ଆକାର(p->name));

  // Commit to the user image.
  oldpagetable = p->pagetable;
  p->pagetable = pagetable;
  p->sz = sz;
  p->trapframe->epc = elf.entry; // initial program counter = ulib.c:start()
  p->trapframe->sp = sp;         // initial stack pointer
  proc_freepagetable(oldpagetable, oldsz);

  ଫେରନ୍ତୁ argc; // this ends up in a0, the first argument to main(argc, argv)

bad:
  ଯଦି (pagetable)
    proc_freepagetable(pagetable, sz);
  ଯଦି (ip) {
    iunlockput(ip);
    end_op();
  }
  ଫେରନ୍ତୁ -1;
}

// Load an ELF program segment into pagetable at virtual address va.
// va must be page-aligned
// and the pages from va to va+sz must already be mapped.
// Returns 0 on success, -1 on failure.
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
loadseg(pagetable_t pagetable, uint64 va, ସଂରଚନା inode *ip, uint offset,
        uint sz)
{
  uint i, n;
  uint64 pa;

  ପାଇଁ (i = 0; i < sz; i += PGSIZE) {
    pa = walkaddr(pagetable, va + i);
    ଯଦି (pa == 0)
      panic("loadseg: address should exist");
    ଯଦି (sz - i < PGSIZE)
      n = sz - i;
    ଅନ୍ୟଥା
      n = PGSIZE;
    ଯଦି (readi(ip, 0, (uint64)pa, offset + i, n) != n)
      ଫେରନ୍ତୁ -1;
  }

  ଫେରନ୍ତୁ 0;
}
