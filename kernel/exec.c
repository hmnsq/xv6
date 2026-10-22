#অন্তর্ভাৱ "types.h"
#অন্তর্ভাৱ "param.h"
#অন্তর্ভাৱ "memlayout.h"
#অন্তর্ভাৱ "riscv.h"
#অন্তর্ভাৱ "spinlock.h"
#অন্তর্ভাৱ "proc.h"
#অন্তর্ভাৱ "defs.h"
#অন্তর্ভাৱ "elf.h"

স্থির পূর্ণাঙ্ক loadseg(pde_t *, uint64, সংরচনা inode *, uint, uint);

// map ELF permissions to PTE permission bits.
পূর্ণাঙ্ক
flags2perm(পূর্ণাঙ্ক flags)
{
  পূর্ণাঙ্ক perm = 0;
  যদি (flags & 0x1)
    perm = PTE_X;
  যদি (flags & 0x2)
    perm |= PTE_W;
  ঘূৰাই perm;
}

//
// the implementation of the exec() system call
//
পূর্ণাঙ্ক
kexec(ৱর্ণ *path, ৱর্ণ **argv)
{
  ৱর্ণ *s, *last;
  পূর্ণাঙ্ক i, off;
  uint64 argc, sz = 0, sp, ustack[MAXARG], stackbase;
  সংরচনা elfhdr elf;
  সংরচনা inode *ip;
  সংরচনা proghdr ph;
  pagetable_t pagetable = 0, oldpagetable;
  সংরচনা proc *p = myproc();

  begin_op();

  // Open the executable file.
  যদি ((ip = namei(path)) == 0) {
    end_op();
    ঘূৰাই -1;
  }
  ilock(ip);

  // Read the ELF header.
  যদি (readi(ip, 0, (uint64)&elf, 0, পরিমাণ(elf)) != পরিমাণ(elf))
    গচ্ছ bad;

  // Is this really an ELF file?
  যদি (elf.magic != ELF_MAGIC)
    গচ্ছ bad;

  যদি ((pagetable = proc_pagetable(p)) == 0)
    গচ্ছ bad;

  // Load program into memory.
  বাবে (i = 0, off = elf.phoff; i < elf.phnum; i++, off += পরিমাণ(ph)) {
    যদি (readi(ip, 0, (uint64)&ph, off, পরিমাণ(ph)) != পরিমাণ(ph))
      গচ্ছ bad;
    যদি (ph.type != ELF_PROG_LOAD)
      অনুৱর্ত;
    যদি (ph.memsz < ph.filesz)
      গচ্ছ bad;
    যদি (ph.vaddr + ph.memsz < ph.vaddr)
      গচ্ছ bad;
    যদি (ph.vaddr % PGSIZE != 0)
      গচ্ছ bad;
    uint64 sz1;
    যদি ((sz1 = uvmalloc(pagetable, sz, ph.vaddr + ph.memsz,
                        flags2perm(ph.flags))) == 0)
      গচ্ছ bad;
    sz = sz1;
    যদি (loadseg(pagetable, ph.vaddr, ip, ph.off, ph.filesz) < 0)
      গচ্ছ bad;
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
  যদি ((sz1 = uvmalloc(pagetable, sz, sz + (USERSTACK + 1) * PGSIZE, PTE_W)) ==
      0)
    গচ্ছ bad;
  sz = sz1;
  uvmclear(pagetable, sz - (USERSTACK + 1) * PGSIZE);
  sp = sz;
  stackbase = sp - USERSTACK * PGSIZE;

  // Copy argument strings into new stack, remember their
  // addresses in ustack[].
  বাবে (argc = 0; argv[argc]; argc++) {
    sp -= strlen(argv[argc]) + 1;
    sp -= sp % 16; // riscv sp must be 16-byte aligned
    যদি (sp < stackbase)
      গচ্ছ bad;
    যদি (copyout(pagetable, sz, sp, argv[argc], strlen(argv[argc]) + 1) < 0)
      গচ্ছ bad;
    ustack[argc] = sp;
  }
  ustack[argc] = 0;

  // push a copy of ustack[], the array of argv[] pointers.
  sp -= (argc + 1) * পরিমাণ(uint64);
  sp -= sp % 16;
  যদি (sp < stackbase)
    গচ্ছ bad;
  যদি (copyout(pagetable, sz, sp, (ৱর্ণ *)ustack, (argc + 1) * পরিমাণ(uint64)) <
      0)
    গচ্ছ bad;

  // a0 and a1 contain arguments to user main(argc, argv)
  // argc is returned via the system call return
  // value, which goes in a0.
  p->trapframe->a1 = sp;

  // Save program name for debugging.
  বাবে (last = s = path; *s; s++)
    যদি (*s == '/')
      last = s + 1;
  safestrcpy(p->name, last, পরিমাণ(p->name));

  // Commit to the user image.
  oldpagetable = p->pagetable;
  p->pagetable = pagetable;
  p->sz = sz;
  p->trapframe->epc = elf.entry; // initial program counter = ulib.c:start()
  p->trapframe->sp = sp;         // initial stack pointer
  proc_freepagetable(oldpagetable, oldsz);

  ঘূৰাই argc; // this ends up in a0, the first argument to main(argc, argv)

bad:
  যদি (pagetable)
    proc_freepagetable(pagetable, sz);
  যদি (ip) {
    iunlockput(ip);
    end_op();
  }
  ঘূৰাই -1;
}

// Load an ELF program segment into pagetable at virtual address va.
// va must be page-aligned
// and the pages from va to va+sz must already be mapped.
// Returns 0 on success, -1 on failure.
স্থির পূর্ণাঙ্ক
loadseg(pagetable_t pagetable, uint64 va, সংরচনা inode *ip, uint offset,
        uint sz)
{
  uint i, n;
  uint64 pa;

  বাবে (i = 0; i < sz; i += PGSIZE) {
    pa = walkaddr(pagetable, va + i);
    যদি (pa == 0)
      panic("loadseg: address should exist");
    যদি (sz - i < PGSIZE)
      n = sz - i;
    অন্যথা
      n = PGSIZE;
    যদি (readi(ip, 0, (uint64)pa, offset + i, n) != n)
      ঘূৰাই -1;
  }

  ঘূৰাই 0;
}
