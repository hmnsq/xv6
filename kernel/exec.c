#ضمن "types.h"
#ضمن "param.h"
#ضمن "memlayout.h"
#ضمن "riscv.h"
#ضمن "spinlock.h"
#ضمن "proc.h"
#ضمن "defs.h"
#ضمن "elf.h"

ساكن صحيح loadseg(pde_t *, uint64, بنية inode *, uint, uint);

// map ELF permissions to PTE permission bits.
صحيح
flags2perm(صحيح flags)
{
  صحيح perm = 0;
  إذا (flags & 0x1)
    perm = PTE_X;
  إذا (flags & 0x2)
    perm |= PTE_W;
  أرجع perm;
}

//
// the implementation of the exec() system call
//
صحيح
kexec(حرف *رستہ, حرف **argv)
{
  حرف *s, *آخری;
  صحيح i, off;
  uint64 argc, sz = 0, sp, ustack[MAXARG], stackbase;
  بنية elfhdr elf;
  بنية inode *ip;
  بنية proghdr ph;
  pagetable_t pagetable = 0, oldpagetable;
  بنية proc *p = myproc();

  begin_op();

  // Open the executable file.
  إذا ((ip = namei(رستہ)) == 0) {
    سرا_op();
    أرجع -1;
  }
  ilock(ip);

  // Read the ELF header.
  إذا (readi(ip, 0, (uint64)&elf, 0, حجم(elf)) != حجم(elf))
    اذهب bad;

  // Is this really an ELF file?
  إذا (elf.magic != ELF_MAGIC)
    اذهب bad;

  إذا ((pagetable = proc_pagetable(p)) == 0)
    اذهب bad;

  // Load program into memory.
  لكل (i = 0, off = elf.phoff; i < elf.phnum; i++, off += حجم(ph)) {
    إذا (readi(ip, 0, (uint64)&ph, off, حجم(ph)) != حجم(ph))
      اذهب bad;
    إذا (ph.type != ELF_PROG_لدو)
      استمر;
    إذا (ph.memsz < ph.filesz)
      اذهب bad;
    إذا (ph.vaddr + ph.memsz < ph.vaddr)
      اذهب bad;
    إذا (ph.vaddr % PGSIZE != 0)
      اذهب bad;
    uint64 sz1;
    إذا ((sz1 = uvmalloc(pagetable, sz, ph.vaddr + ph.memsz,
                        flags2perm(ph.flags))) == 0)
      اذهب bad;
    sz = sz1;
    إذا (loadseg(pagetable, ph.vaddr, ip, ph.off, ph.filesz) < 0)
      اذهب bad;
  }
  iunlockput(ip);
  سرا_op();
  ip = 0;

  p = myproc();
  uint64 oldsz = p->sz;

  // Allocate some pages at the next page boundary.
  // Make the first inaccessible as a stack guard.
  // Use the rest as the user stack.
  sz = PGROUNDUP(sz);
  uint64 sz1;
  إذا ((sz1 = uvmalloc(pagetable, sz, sz + (USERSTACK + 1) * PGSIZE, PTE_W)) ==
      0)
    اذهب bad;
  sz = sz1;
  uvmclear(pagetable, sz - (USERSTACK + 1) * PGSIZE);
  sp = sz;
  stackbase = sp - USERSTACK * PGSIZE;

  // Copy argument strings into new stack, remember their
  // addresses in ustack[].
  لكل (argc = 0; argv[argc]; argc++) {
    sp -= لڑی_لمبائی(argv[argc]) + 1;
    sp -= sp % 16; // riscv sp must be 16-byte aligned
    إذا (sp < stackbase)
      اذهب bad;
    إذا (copyout(pagetable, sz, sp, argv[argc], لڑی_لمبائی(argv[argc]) + 1) < 0)
      اذهب bad;
    ustack[argc] = sp;
  }
  ustack[argc] = 0;

  // push a copy of ustack[], the array of argv[] pointers.
  sp -= (argc + 1) * حجم(uint64);
  sp -= sp % 16;
  إذا (sp < stackbase)
    اذهب bad;
  إذا (copyout(pagetable, sz, sp, (حرف *)ustack, (argc + 1) * حجم(uint64)) <
      0)
    اذهب bad;

  // a0 and a1 contain arguments to user main(argc, argv)
  // argc is returned via the system call return
  // value, which goes in a0.
  p->trapframe->a1 = sp;

  // Save program name for debugging.
  لكل (آخری = s = رستہ; *s; s++)
    إذا (*s == '/')
      آخری = s + 1;
  safestrcpy(p->name, آخری, حجم(p->name));

  // Commit to the user image.
  oldpagetable = p->pagetable;
  p->pagetable = pagetable;
  p->sz = sz;
  p->trapframe->epc = elf.اندراج; // initial program counter = ulib.c:start()
  p->trapframe->sp = sp;         // initial stack pointer
  proc_freepagetable(oldpagetable, oldsz);

  أرجع argc; // this ends up in a0, the first argument to main(argc, argv)

bad:
  إذا (pagetable)
    proc_freepagetable(pagetable, sz);
  إذا (ip) {
    iunlockput(ip);
    سرا_op();
  }
  أرجع -1;
}

// Load an ELF program segment into pagetable at virtual address va.
// va must be page-aligned
// and the pages from va to va+sz must already be mapped.
// Returns 0 on success, -1 on failure.
ساكن صحيح
loadseg(pagetable_t pagetable, uint64 va, بنية inode *ip, uint وتھ,
        uint sz)
{
  uint i, n;
  uint64 pa;

  لكل (i = 0; i < sz; i += PGSIZE) {
    pa = walkaddr(pagetable, va + i);
    إذا (pa == 0)
      panic("loadseg: address should exist");
    إذا (sz - i < PGSIZE)
      n = sz - i;
    وإلا
      n = PGSIZE;
    إذا (readi(ip, 0, (uint64)pa, وتھ + i, n) != n)
      أرجع -1;
  }

  أرجع 0;
}
