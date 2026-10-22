#समावेश "types.h"
#समावेश "param.h"
#समावेश "memlayout.h"
#समावेश "riscv.h"
#समावेश "spinlock.h"
#समावेश "proc.h"
#समावेश "defs.h"
#समावेश "elf.h"

अटल पूर्णांक loadseg(pde_t *, uint64, काष्ठा inode *, uint, uint);

// map ELF permissions to PTE permission bits.
पूर्णांक
flags2perm(पूर्णांक flags)
{
  पूर्णांक perm = 0;
  अगर (flags & 0x1)
    perm = PTE_X;
  अगर (flags & 0x2)
    perm |= PTE_W;
  वापस perm;
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
  काष्ठा elfhdr elf;
  काष्ठा inode *ip;
  काष्ठा proghdr ph;
  pagetable_t pagetable = 0, oldpagetable;
  काष्ठा proc *p = myproc();

  begin_op();

  // Open the executable file.
  अगर ((ip = namei(path)) == 0) {
    end_op();
    वापस -1;
  }
  ilock(ip);

  // Read the ELF header.
  अगर (readi(ip, 0, (uint64)&elf, 0, माप(elf)) != माप(elf))
    जाओ bad;

  // Is this really an ELF file?
  अगर (elf.magic != ELF_MAGIC)
    जाओ bad;

  अगर ((pagetable = proc_pagetable(p)) == 0)
    जाओ bad;

  // Load program into memory.
  क्रम (i = 0, off = elf.phoff; i < elf.phnum; i++, off += माप(ph)) {
    अगर (readi(ip, 0, (uint64)&ph, off, माप(ph)) != माप(ph))
      जाओ bad;
    अगर (ph.type != ELF_PROG_LOAD)
      जारी;
    अगर (ph.memsz < ph.filesz)
      जाओ bad;
    अगर (ph.vaddr + ph.memsz < ph.vaddr)
      जाओ bad;
    अगर (ph.vaddr % PGSIZE != 0)
      जाओ bad;
    uint64 sz1;
    अगर ((sz1 = uvmalloc(pagetable, sz, ph.vaddr + ph.memsz,
                        flags2perm(ph.flags))) == 0)
      जाओ bad;
    sz = sz1;
    अगर (loadseg(pagetable, ph.vaddr, ip, ph.off, ph.filesz) < 0)
      जाओ bad;
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
  अगर ((sz1 = uvmalloc(pagetable, sz, sz + (USERSTACK + 1) * PGSIZE, PTE_W)) ==
      0)
    जाओ bad;
  sz = sz1;
  uvmclear(pagetable, sz - (USERSTACK + 1) * PGSIZE);
  sp = sz;
  stackbase = sp - USERSTACK * PGSIZE;

  // Copy argument strings into new stack, remember their
  // addresses in ustack[].
  क्रम (argc = 0; argv[argc]; argc++) {
    sp -= म_माप(argv[argc]) + 1;
    sp -= sp % 16; // riscv sp must be 16-byte aligned
    अगर (sp < stackbase)
      जाओ bad;
    अगर (copyout(pagetable, sz, sp, argv[argc], म_माप(argv[argc]) + 1) < 0)
      जाओ bad;
    ustack[argc] = sp;
  }
  ustack[argc] = 0;

  // push a copy of ustack[], the array of argv[] pointers.
  sp -= (argc + 1) * माप(uint64);
  sp -= sp % 16;
  अगर (sp < stackbase)
    जाओ bad;
  अगर (copyout(pagetable, sz, sp, (अक्षर *)ustack, (argc + 1) * माप(uint64)) <
      0)
    जाओ bad;

  // a0 and a1 contain arguments to user main(argc, argv)
  // argc is returned via the system call return
  // value, which goes in a0.
  p->trapframe->a1 = sp;

  // Save program name for debugging.
  क्रम (last = s = path; *s; s++)
    अगर (*s == '/')
      last = s + 1;
  safestrcpy(p->name, last, माप(p->name));

  // Commit to the user image.
  oldpagetable = p->pagetable;
  p->pagetable = pagetable;
  p->sz = sz;
  p->trapframe->epc = elf.entry; // initial program counter = ulib.c:start()
  p->trapframe->sp = sp;         // initial stack pointer
  proc_freepagetable(oldpagetable, oldsz);

  वापस argc; // this ends up in a0, the first argument to main(argc, argv)

bad:
  अगर (pagetable)
    proc_freepagetable(pagetable, sz);
  अगर (ip) {
    iunlockput(ip);
    end_op();
  }
  वापस -1;
}

// Load an ELF program segment into pagetable at virtual address va.
// va must be page-aligned
// and the pages from va to va+sz must already be mapped.
// Returns 0 on success, -1 on failure.
अटल पूर्णांक
loadseg(pagetable_t pagetable, uint64 va, काष्ठा inode *ip, uint offset,
        uint sz)
{
  uint i, n;
  uint64 pa;

  क्रम (i = 0; i < sz; i += PGSIZE) {
    pa = walkaddr(pagetable, va + i);
    अगर (pa == 0)
      panic("loadseg: address should exist");
    अगर (sz - i < PGSIZE)
      n = sz - i;
    अन्यथा
      n = PGSIZE;
    अगर (readi(ip, 0, (uint64)pa, offset + i, n) != n)
      वापस -1;
  }

  वापस 0;
}
