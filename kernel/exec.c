#ਅਨ੍ਤਰ੍ਭਾਵ "types.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "param.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "memlayout.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "riscv.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "spinlock.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "proc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "defs.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "elf.h"

ਸਥਿਰ ਪੂਰਨ loadseg(pde_t *, uint64, ਬਣਤਰ inode *, uint, uint);

// map ELF permissions to PTE permission bits.
ਪੂਰਨ
flags2perm(ਪੂਰਨ flags)
{
  ਪੂਰਨ perm = 0;
  ਜੇ (flags & 0x1)
    perm = PTE_X;
  ਜੇ (flags & 0x2)
    perm |= PTE_W;
  ਵਾਪਸ perm;
}

//
// the implementation of the exec() system call
//
ਪੂਰਨ
kexec(ਅੱਖਰ *ਰਸਤਾ, ਅੱਖਰ **argv)
{
  ਅੱਖਰ *s, *ਆਖਰੀ;
  ਪੂਰਨ i, off;
  uint64 argc, sz = 0, sp, ustack[MAXARG], stackbase;
  ਬਣਤਰ elfhdr elf;
  ਬਣਤਰ inode *ip;
  ਬਣਤਰ proghdr ph;
  pagetable_t pagetable = 0, oldpagetable;
  ਬਣਤਰ proc *p = myproc();

  begin_op();

  // Open the executable file.
  ਜੇ ((ip = namei(ਰਸਤਾ)) == 0) {
    ਸਿਰਾ_op();
    ਵਾਪਸ -1;
  }
  ilock(ip);

  // Read the ELF header.
  ਜੇ (readi(ip, 0, (uint64)&elf, 0, ਆਕਾਰ(elf)) != ਆਕਾਰ(elf))
    ਜਾਓ bad;

  // Is this really an ELF file?
  ਜੇ (elf.magic != ELF_MAGIC)
    ਜਾਓ bad;

  ਜੇ ((pagetable = proc_pagetable(p)) == 0)
    ਜਾਓ bad;

  // Load program into memory.
  ਲਈ (i = 0, off = elf.phoff; i < elf.phnum; i++, off += ਆਕਾਰ(ph)) {
    ਜੇ (readi(ip, 0, (uint64)&ph, off, ਆਕਾਰ(ph)) != ਆਕਾਰ(ph))
      ਜਾਓ bad;
    ਜੇ (ph.type != ELF_PROG_ਲੱਦੋ)
      ਜਾਰੀ;
    ਜੇ (ph.memsz < ph.filesz)
      ਜਾਓ bad;
    ਜੇ (ph.vaddr + ph.memsz < ph.vaddr)
      ਜਾਓ bad;
    ਜੇ (ph.vaddr % PGSIZE != 0)
      ਜਾਓ bad;
    uint64 sz1;
    ਜੇ ((sz1 = uvmalloc(pagetable, sz, ph.vaddr + ph.memsz,
                        flags2perm(ph.flags))) == 0)
      ਜਾਓ bad;
    sz = sz1;
    ਜੇ (loadseg(pagetable, ph.vaddr, ip, ph.off, ph.filesz) < 0)
      ਜਾਓ bad;
  }
  iunlockput(ip);
  ਸਿਰਾ_op();
  ip = 0;

  p = myproc();
  uint64 oldsz = p->sz;

  // Allocate some pages at the next page boundary.
  // Make the first inaccessible as a stack guard.
  // Use the rest as the user stack.
  sz = PGROUNDUP(sz);
  uint64 sz1;
  ਜੇ ((sz1 = uvmalloc(pagetable, sz, sz + (USERSTACK + 1) * PGSIZE, PTE_W)) ==
      0)
    ਜਾਓ bad;
  sz = sz1;
  uvmclear(pagetable, sz - (USERSTACK + 1) * PGSIZE);
  sp = sz;
  stackbase = sp - USERSTACK * PGSIZE;

  // Copy argument strings into new stack, remember their
  // addresses in ustack[].
  ਲਈ (argc = 0; argv[argc]; argc++) {
    sp -= ਲੜੀ_ਲੰਬਾਈ(argv[argc]) + 1;
    sp -= sp % 16; // riscv sp must be 16-byte aligned
    ਜੇ (sp < stackbase)
      ਜਾਓ bad;
    ਜੇ (copyout(pagetable, sz, sp, argv[argc], ਲੜੀ_ਲੰਬਾਈ(argv[argc]) + 1) < 0)
      ਜਾਓ bad;
    ustack[argc] = sp;
  }
  ustack[argc] = 0;

  // push a copy of ustack[], the array of argv[] pointers.
  sp -= (argc + 1) * ਆਕਾਰ(uint64);
  sp -= sp % 16;
  ਜੇ (sp < stackbase)
    ਜਾਓ bad;
  ਜੇ (copyout(pagetable, sz, sp, (ਅੱਖਰ *)ustack, (argc + 1) * ਆਕਾਰ(uint64)) <
      0)
    ਜਾਓ bad;

  // a0 and a1 contain arguments to user main(argc, argv)
  // argc is returned via the system call return
  // value, which goes in a0.
  p->trapframe->a1 = sp;

  // Save program name for debugging.
  ਲਈ (ਆਖਰੀ = s = ਰਸਤਾ; *s; s++)
    ਜੇ (*s == '/')
      ਆਖਰੀ = s + 1;
  safestrcpy(p->name, ਆਖਰੀ, ਆਕਾਰ(p->name));

  // Commit to the user image.
  oldpagetable = p->pagetable;
  p->pagetable = pagetable;
  p->sz = sz;
  p->trapframe->epc = elf.ਇੰਦਰਾਜ; // initial program counter = ulib.c:start()
  p->trapframe->sp = sp;         // initial stack pointer
  proc_freepagetable(oldpagetable, oldsz);

  ਵਾਪਸ argc; // this ends up in a0, the first argument to main(argc, argv)

bad:
  ਜੇ (pagetable)
    proc_freepagetable(pagetable, sz);
  ਜੇ (ip) {
    iunlockput(ip);
    ਸਿਰਾ_op();
  }
  ਵਾਪਸ -1;
}

// Load an ELF program segment into pagetable at virtual address va.
// va must be page-aligned
// and the pages from va to va+sz must already be mapped.
// Returns 0 on success, -1 on failure.
ਸਥਿਰ ਪੂਰਨ
loadseg(pagetable_t pagetable, uint64 va, ਬਣਤਰ inode *ip, uint ਵਿੱਥ,
        uint sz)
{
  uint i, n;
  uint64 pa;

  ਲਈ (i = 0; i < sz; i += PGSIZE) {
    pa = walkaddr(pagetable, va + i);
    ਜੇ (pa == 0)
      panic("loadseg: address should exist");
    ਜੇ (sz - i < PGSIZE)
      n = sz - i;
    ਨਹੀਂਤਾਂ
      n = PGSIZE;
    ਜੇ (readi(ip, 0, (uint64)pa, ਵਿੱਥ + i, n) != n)
      ਵਾਪਸ -1;
  }

  ਵਾਪਸ 0;
}
