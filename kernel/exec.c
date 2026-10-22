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
  אם (flags & 0x1)
    perm = PTE_X;
  אם (flags & 0x2)
    perm |= PTE_W;
  החזר perm;
}

//
// the implementation of the exec() system call
//
صحيح
kexec(حرف *path, حرف **argv)
{
  حرف *s, *last;
  صحيح i, off;
  uint64 argc, sz = 0, sp, ustack[MAXARG], stackbase;
  بنية elfhdr elf;
  بنية inode *ip;
  بنية proghdr ph;
  pagetable_t pagetable = 0, oldpagetable;
  بنية proc *p = myproc();

  begin_op();

  // Open the executable file.
  אם ((ip = namei(path)) == 0) {
    end_op();
    החזר -1;
  }
  ilock(ip);

  // Read the ELF header.
  אם (readi(ip, 0, (uint64)&elf, 0, حجم(elf)) != حجم(elf))
    اذهب bad;

  // Is this really an ELF file?
  אם (elf.magic != ELF_MAGIC)
    اذهب bad;

  אם ((pagetable = proc_pagetable(p)) == 0)
    اذهب bad;

  // Load program into memory.
  עבור (i = 0, off = elf.phoff; i < elf.phnum; i++, off += حجم(ph)) {
    אם (readi(ip, 0, (uint64)&ph, off, حجم(ph)) != حجم(ph))
      اذهب bad;
    אם (ph.type != ELF_PROG_LOAD)
      استمر;
    אם (ph.memsz < ph.filesz)
      اذهب bad;
    אם (ph.vaddr + ph.memsz < ph.vaddr)
      اذهب bad;
    אם (ph.vaddr % PGSIZE != 0)
      اذهب bad;
    uint64 sz1;
    אם ((sz1 = uvmalloc(pagetable, sz, ph.vaddr + ph.memsz,
                        flags2perm(ph.flags))) == 0)
      اذهب bad;
    sz = sz1;
    אם (loadseg(pagetable, ph.vaddr, ip, ph.off, ph.filesz) < 0)
      اذهب bad;
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
  אם ((sz1 = uvmalloc(pagetable, sz, sz + (USERSTACK + 1) * PGSIZE, PTE_W)) ==
      0)
    اذهب bad;
  sz = sz1;
  uvmclear(pagetable, sz - (USERSTACK + 1) * PGSIZE);
  sp = sz;
  stackbase = sp - USERSTACK * PGSIZE;

  // Copy argument strings into new stack, remember their
  // addresses in ustack[].
  עבור (argc = 0; argv[argc]; argc++) {
    sp -= strlen(argv[argc]) + 1;
    sp -= sp % 16; // riscv sp must be 16-byte aligned
    אם (sp < stackbase)
      اذهب bad;
    אם (copyout(pagetable, sz, sp, argv[argc], strlen(argv[argc]) + 1) < 0)
      اذهب bad;
    ustack[argc] = sp;
  }
  ustack[argc] = 0;

  // push a copy of ustack[], the array of argv[] pointers.
  sp -= (argc + 1) * حجم(uint64);
  sp -= sp % 16;
  אם (sp < stackbase)
    اذهب bad;
  אם (copyout(pagetable, sz, sp, (حرف *)ustack, (argc + 1) * حجم(uint64)) <
      0)
    اذهب bad;

  // a0 and a1 contain arguments to user main(argc, argv)
  // argc is returned via the system call return
  // value, which goes in a0.
  p->trapframe->a1 = sp;

  // Save program name for debugging.
  עבור (last = s = path; *s; s++)
    אם (*s == '/')
      last = s + 1;
  safestrcpy(p->name, last, حجم(p->name));

  // Commit to the user image.
  oldpagetable = p->pagetable;
  p->pagetable = pagetable;
  p->sz = sz;
  p->trapframe->epc = elf.entry; // initial program counter = ulib.c:start()
  p->trapframe->sp = sp;         // initial stack pointer
  proc_freepagetable(oldpagetable, oldsz);

  החזר argc; // this ends up in a0, the first argument to main(argc, argv)

bad:
  אם (pagetable)
    proc_freepagetable(pagetable, sz);
  אם (ip) {
    iunlockput(ip);
    end_op();
  }
  החזר -1;
}

// Load an ELF program segment into pagetable at virtual address va.
// va must be page-aligned
// and the pages from va to va+sz must already be mapped.
// Returns 0 on success, -1 on failure.
ساكن صحيح
loadseg(pagetable_t pagetable, uint64 va, بنية inode *ip, uint offset,
        uint sz)
{
  uint i, n;
  uint64 pa;

  עבור (i = 0; i < sz; i += PGSIZE) {
    pa = walkaddr(pagetable, va + i);
    אם (pa == 0)
      panic("loadseg: address should exist");
    אם (sz - i < PGSIZE)
      n = sz - i;
    אחרת
      n = PGSIZE;
    אם (readi(ip, 0, (uint64)pa, offset + i, n) != n)
      החזר -1;
  }

  החזר 0;
}
