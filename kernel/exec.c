#شامل "types.h"
#شامل "param.h"
#شامل "memlayout.h"
#شامل "riscv.h"
#شامل "spinlock.h"
#شامل "proc.h"
#شامل "defs.h"
#شامل "elf.h"

ساکن صحیح loadseg(pde_t *, uint64, ساخت inode *, uint, uint);

// map ELF permissions to PTE permission bits.
صحیح
flags2perm(صحیح flags)
{
  صحیح perm = 0;
  اگر (flags & 0x1)
    perm = PTE_X;
  اگر (flags & 0x2)
    perm |= PTE_W;
  واپس perm;
}

//
// the implementation of the exec() system call
//
صحیح
kexec(حرف *راستہ, حرف **argv)
{
  حرف *s, *آخری;
  صحیح i, off;
  uint64 argc, sz = 0, sp, ustack[MAXARG], stackbase;
  ساخت elfhdr elf;
  ساخت inode *ip;
  ساخت proghdr ph;
  pagetable_t pagetable = 0, oldpagetable;
  ساخت proc *p = myproc();

  begin_op();

  // Open the executable file.
  اگر ((ip = namei(راستہ)) == 0) {
    انتہا_op();
    واپس -1;
  }
  ilock(ip);

  // Read the ELF header.
  اگر (readi(ip, 0, (uint64)&elf, 0, حجم(elf)) != حجم(elf))
    جاؤ bad;

  // Is this really an ELF file?
  اگر (elf.magic != ELF_MAGIC)
    جاؤ bad;

  اگر ((pagetable = proc_pagetable(p)) == 0)
    جاؤ bad;

  // Load program into memory.
  برائے (i = 0, off = elf.phoff; i < elf.phnum; i++, off += حجم(ph)) {
    اگر (readi(ip, 0, (uint64)&ph, off, حجم(ph)) != حجم(ph))
      جاؤ bad;
    اگر (ph.type != ELF_PROG_بھرو)
      جاری;
    اگر (ph.memsz < ph.filesz)
      جاؤ bad;
    اگر (ph.vaddr + ph.memsz < ph.vaddr)
      جاؤ bad;
    اگر (ph.vaddr % PGSIZE != 0)
      جاؤ bad;
    uint64 sz1;
    اگر ((sz1 = uvmalloc(pagetable, sz, ph.vaddr + ph.memsz,
                        flags2perm(ph.flags))) == 0)
      جاؤ bad;
    sz = sz1;
    اگر (loadseg(pagetable, ph.vaddr, ip, ph.off, ph.filesz) < 0)
      جاؤ bad;
  }
  iunlockput(ip);
  انتہا_op();
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
    جاؤ bad;
  sz = sz1;
  uvmclear(pagetable, sz - (USERSTACK + 1) * PGSIZE);
  sp = sz;
  stackbase = sp - USERSTACK * PGSIZE;

  // Copy argument strings into new stack, remember their
  // addresses in ustack[].
  برائے (argc = 0; argv[argc]; argc++) {
    sp -= لڑی_لمبائی(argv[argc]) + 1;
    sp -= sp % 16; // riscv sp must be 16-byte aligned
    اگر (sp < stackbase)
      جاؤ bad;
    اگر (copyout(pagetable, sz, sp, argv[argc], لڑی_لمبائی(argv[argc]) + 1) < 0)
      جاؤ bad;
    ustack[argc] = sp;
  }
  ustack[argc] = 0;

  // push a copy of ustack[], the array of argv[] pointers.
  sp -= (argc + 1) * حجم(uint64);
  sp -= sp % 16;
  اگر (sp < stackbase)
    جاؤ bad;
  اگر (copyout(pagetable, sz, sp, (حرف *)ustack, (argc + 1) * حجم(uint64)) <
      0)
    جاؤ bad;

  // a0 and a1 contain arguments to user main(argc, argv)
  // argc is returned via the system call return
  // value, which goes in a0.
  p->trapframe->a1 = sp;

  // Save program name for debugging.
  برائے (آخری = s = راستہ; *s; s++)
    اگر (*s == '/')
      آخری = s + 1;
  safestrcpy(p->name, آخری, حجم(p->name));

  // Commit to the user image.
  oldpagetable = p->pagetable;
  p->pagetable = pagetable;
  p->sz = sz;
  p->trapframe->epc = elf.اندراج_2; // initial program counter = ulib.c:start()
  p->trapframe->sp = sp;         // initial stack pointer
  proc_freepagetable(oldpagetable, oldsz);

  واپس argc; // this ends up in a0, the first argument to main(argc, argv)

bad:
  اگر (pagetable)
    proc_freepagetable(pagetable, sz);
  اگر (ip) {
    iunlockput(ip);
    انتہا_op();
  }
  واپس -1;
}

// Load an ELF program segment into pagetable at virtual address va.
// va must be page-aligned
// and the pages from va to va+sz must already be mapped.
// Returns 0 on success, -1 on failure.
ساکن صحیح
loadseg(pagetable_t pagetable, uint64 va, ساخت inode *ip, uint انحراف,
        uint sz)
{
  uint i, n;
  uint64 pa;

  برائے (i = 0; i < sz; i += PGSIZE) {
    pa = walkaddr(pagetable, va + i);
    اگر (pa == 0)
      panic("loadseg: address should exist");
    اگر (sz - i < PGSIZE)
      n = sz - i;
    ورنہ
      n = PGSIZE;
    اگر (readi(ip, 0, (uint64)pa, انحراف + i, n) != n)
      واپس -1;
  }

  واپس 0;
}
