#அந்தர்பாவ "types.h"
#அந்தர்பாவ "param.h"
#அந்தர்பாவ "memlayout.h"
#அந்தர்பாவ "riscv.h"
#அந்தர்பாவ "spinlock.h"
#அந்தர்பாவ "proc.h"
#அந்தர்பாவ "defs.h"
#அந்தர்பாவ "elf.h"

நிலையான முழு loadseg(pde_t *, uint64, கட்டமைப்பு inode *, uint, uint);

// map ELF permissions to PTE permission bits.
முழு
flags2perm(முழு flags)
{
  முழு perm = 0;
  எனில் (flags & 0x1)
    perm = PTE_X;
  எனில் (flags & 0x2)
    perm |= PTE_W;
  திரும்பு perm;
}

//
// the implementation of the exec() system call
//
முழு
kexec(எழுத்து *path, எழுத்து **argv)
{
  எழுத்து *s, *last;
  முழு i, off;
  uint64 argc, sz = 0, sp, ustack[MAXARG], stackbase;
  கட்டமைப்பு elfhdr elf;
  கட்டமைப்பு inode *ip;
  கட்டமைப்பு proghdr ph;
  pagetable_t pagetable = 0, oldpagetable;
  கட்டமைப்பு proc *p = myproc();

  begin_op();

  // Open the executable file.
  எனில் ((ip = namei(path)) == 0) {
    end_op();
    திரும்பு -1;
  }
  ilock(ip);

  // Read the ELF header.
  எனில் (readi(ip, 0, (uint64)&elf, 0, அளவு(elf)) != அளவு(elf))
    செல் bad;

  // Is this really an ELF file?
  எனில் (elf.magic != ELF_MAGIC)
    செல் bad;

  எனில் ((pagetable = proc_pagetable(p)) == 0)
    செல் bad;

  // Load program into memory.
  க்கு (i = 0, off = elf.phoff; i < elf.phnum; i++, off += அளவு(ph)) {
    எனில் (readi(ip, 0, (uint64)&ph, off, அளவு(ph)) != அளவு(ph))
      செல் bad;
    எனில் (ph.type != ELF_PROG_LOAD)
      தொடர்;
    எனில் (ph.memsz < ph.filesz)
      செல் bad;
    எனில் (ph.vaddr + ph.memsz < ph.vaddr)
      செல் bad;
    எனில் (ph.vaddr % PGSIZE != 0)
      செல் bad;
    uint64 sz1;
    எனில் ((sz1 = uvmalloc(pagetable, sz, ph.vaddr + ph.memsz,
                        flags2perm(ph.flags))) == 0)
      செல் bad;
    sz = sz1;
    எனில் (loadseg(pagetable, ph.vaddr, ip, ph.off, ph.filesz) < 0)
      செல் bad;
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
  எனில் ((sz1 = uvmalloc(pagetable, sz, sz + (USERSTACK + 1) * PGSIZE, PTE_W)) ==
      0)
    செல் bad;
  sz = sz1;
  uvmclear(pagetable, sz - (USERSTACK + 1) * PGSIZE);
  sp = sz;
  stackbase = sp - USERSTACK * PGSIZE;

  // Copy argument strings into new stack, remember their
  // addresses in ustack[].
  க்கு (argc = 0; argv[argc]; argc++) {
    sp -= strlen(argv[argc]) + 1;
    sp -= sp % 16; // riscv sp must be 16-byte aligned
    எனில் (sp < stackbase)
      செல் bad;
    எனில் (copyout(pagetable, sz, sp, argv[argc], strlen(argv[argc]) + 1) < 0)
      செல் bad;
    ustack[argc] = sp;
  }
  ustack[argc] = 0;

  // push a copy of ustack[], the array of argv[] pointers.
  sp -= (argc + 1) * அளவு(uint64);
  sp -= sp % 16;
  எனில் (sp < stackbase)
    செல் bad;
  எனில் (copyout(pagetable, sz, sp, (எழுத்து *)ustack, (argc + 1) * அளவு(uint64)) <
      0)
    செல் bad;

  // a0 and a1 contain arguments to user main(argc, argv)
  // argc is returned via the system call return
  // value, which goes in a0.
  p->trapframe->a1 = sp;

  // Save program name for debugging.
  க்கு (last = s = path; *s; s++)
    எனில் (*s == '/')
      last = s + 1;
  safestrcpy(p->name, last, அளவு(p->name));

  // Commit to the user image.
  oldpagetable = p->pagetable;
  p->pagetable = pagetable;
  p->sz = sz;
  p->trapframe->epc = elf.entry; // initial program counter = ulib.c:start()
  p->trapframe->sp = sp;         // initial stack pointer
  proc_freepagetable(oldpagetable, oldsz);

  திரும்பு argc; // this ends up in a0, the first argument to main(argc, argv)

bad:
  எனில் (pagetable)
    proc_freepagetable(pagetable, sz);
  எனில் (ip) {
    iunlockput(ip);
    end_op();
  }
  திரும்பு -1;
}

// Load an ELF program segment into pagetable at virtual address va.
// va must be page-aligned
// and the pages from va to va+sz must already be mapped.
// Returns 0 on success, -1 on failure.
நிலையான முழு
loadseg(pagetable_t pagetable, uint64 va, கட்டமைப்பு inode *ip, uint offset,
        uint sz)
{
  uint i, n;
  uint64 pa;

  க்கு (i = 0; i < sz; i += PGSIZE) {
    pa = walkaddr(pagetable, va + i);
    எனில் (pa == 0)
      panic("loadseg: address should exist");
    எனில் (sz - i < PGSIZE)
      n = sz - i;
    இல்லையெனில்
      n = PGSIZE;
    எனில் (readi(ip, 0, (uint64)pa, offset + i, n) != n)
      திரும்பு -1;
  }

  திரும்பு 0;
}
