#شامل "types.h"
#شامل "param.h"
#شامل "memlayout.h"
#شامل "riscv.h"
#شامل "spinlock.h"
#شامل "proc.h"
#شامل "defs.h"
#شامل "elf.h"

ایستا صحیح loadseg(pde_t *, uint64, ساختار inode *, uint, uint);

// map ELF permissions to PTE permission bits.
صحیح
flags2perm(صحیح flags)
{
  صحیح perm = 0;
  اگر (flags & 0x1)
    perm = PTE_X;
  اگر (flags & 0x2)
    perm |= PTE_W;
  بازگشت perm;
}

//
// the implementation of the exec() system call
//
صحیح
kexec(نویسه *path, نویسه **argv)
{
  نویسه *s, *last;
  صحیح i, off;
  uint64 argc, sz = 0, sp, ustack[MAXARG], stackbase;
  ساختار elfhdr elf;
  ساختار inode *ip;
  ساختار proghdr ph;
  pagetable_t pagetable = 0, oldpagetable;
  ساختار proc *p = myproc();

  begin_op();

  // Open the executable file.
  اگر ((ip = namei(path)) == 0) {
    end_op();
    بازگشت -1;
  }
  ilock(ip);

  // Read the ELF header.
  اگر (readi(ip, 0, (uint64)&elf, 0, اندازه(elf)) != اندازه(elf))
    برو bad;

  // Is this really an ELF file?
  اگر (elf.magic != ELF_MAGIC)
    برو bad;

  اگر ((pagetable = proc_pagetable(p)) == 0)
    برو bad;

  // Load program into memory.
  برای (i = 0, off = elf.phoff; i < elf.phnum; i++, off += اندازه(ph)) {
    اگر (readi(ip, 0, (uint64)&ph, off, اندازه(ph)) != اندازه(ph))
      برو bad;
    اگر (ph.type != ELF_PROG_LOAD)
      ادامه;
    اگر (ph.memsz < ph.filesz)
      برو bad;
    اگر (ph.vaddr + ph.memsz < ph.vaddr)
      برو bad;
    اگر (ph.vaddr % PGSIZE != 0)
      برو bad;
    uint64 sz1;
    اگر ((sz1 = uvmalloc(pagetable, sz, ph.vaddr + ph.memsz,
                        flags2perm(ph.flags))) == 0)
      برو bad;
    sz = sz1;
    اگر (loadseg(pagetable, ph.vaddr, ip, ph.off, ph.filesz) < 0)
      برو bad;
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
  اگر ((sz1 = uvmalloc(pagetable, sz, sz + (USERSTACK + 1) * PGSIZE, PTE_W)) ==
      0)
    برو bad;
  sz = sz1;
  uvmclear(pagetable, sz - (USERSTACK + 1) * PGSIZE);
  sp = sz;
  stackbase = sp - USERSTACK * PGSIZE;

  // Copy argument strings into new stack, remember their
  // addresses in ustack[].
  برای (argc = 0; argv[argc]; argc++) {
    sp -= strlen(argv[argc]) + 1;
    sp -= sp % 16; // riscv sp must be 16-byte aligned
    اگر (sp < stackbase)
      برو bad;
    اگر (copyout(pagetable, sz, sp, argv[argc], strlen(argv[argc]) + 1) < 0)
      برو bad;
    ustack[argc] = sp;
  }
  ustack[argc] = 0;

  // push a copy of ustack[], the array of argv[] pointers.
  sp -= (argc + 1) * اندازه(uint64);
  sp -= sp % 16;
  اگر (sp < stackbase)
    برو bad;
  اگر (copyout(pagetable, sz, sp, (نویسه *)ustack, (argc + 1) * اندازه(uint64)) <
      0)
    برو bad;

  // a0 and a1 contain arguments to user main(argc, argv)
  // argc is returned via the system call return
  // value, which goes in a0.
  p->trapframe->a1 = sp;

  // Save program name for debugging.
  برای (last = s = path; *s; s++)
    اگر (*s == '/')
      last = s + 1;
  safestrcpy(p->name, last, اندازه(p->name));

  // Commit to the user image.
  oldpagetable = p->pagetable;
  p->pagetable = pagetable;
  p->sz = sz;
  p->trapframe->epc = elf.entry; // initial program counter = ulib.c:start()
  p->trapframe->sp = sp;         // initial stack pointer
  proc_freepagetable(oldpagetable, oldsz);

  بازگشت argc; // this ends up in a0, the first argument to main(argc, argv)

bad:
  اگر (pagetable)
    proc_freepagetable(pagetable, sz);
  اگر (ip) {
    iunlockput(ip);
    end_op();
  }
  بازگشت -1;
}

// Load an ELF program segment into pagetable at virtual address va.
// va must be page-aligned
// and the pages from va to va+sz must already be mapped.
// Returns 0 on success, -1 on failure.
ایستا صحیح
loadseg(pagetable_t pagetable, uint64 va, ساختار inode *ip, uint offset,
        uint sz)
{
  uint i, n;
  uint64 pa;

  برای (i = 0; i < sz; i += PGSIZE) {
    pa = walkaddr(pagetable, va + i);
    اگر (pa == 0)
      panic("loadseg: address should exist");
    اگر (sz - i < PGSIZE)
      n = sz - i;
    وگرنه
      n = PGSIZE;
    اگر (readi(ip, 0, (uint64)pa, offset + i, n) != n)
      بازگشت -1;
  }

  بازگشت 0;
}
