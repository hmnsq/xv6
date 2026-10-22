#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "spinlock.h"
#include "proc.h"
#include "defs.h"
#include "elf.h"

static จำนวนเต็ม loadseg(pde_t *, uint64, sa_nracanaa inode *, uint, uint);

// map ELF permissions to PTE permission bits.
จำนวนเต็ม
flags2perm(จำนวนเต็ม flags)
{
  จำนวนเต็ม perm = 0;
  ถ้า (flags & 0x1)
    perm = PTE_X;
  ถ้า (flags & 0x2)
    perm |= PTE_W;
  คืน perm;
}

//
// the implementation of the exec() system call
//
จำนวนเต็ม
kexec(var_na *path, var_na **argv)
{
  var_na *s, *last;
  จำนวนเต็ม i, off;
  uint64 argc, sz = 0, sp, ustack[MAXARG], stackbase;
  sa_nracanaa elfhdr elf;
  sa_nracanaa inode *ip;
  sa_nracanaa proghdr ph;
  pagetable_t pagetable = 0, oldpagetable;
  sa_nracanaa proc *p = myproc();

  begin_op();

  // Open the executable file.
  ถ้า ((ip = namei(path)) == 0) {
    end_op();
    คืน -1;
  }
  ilock(ip);

  // Read the ELF header.
  ถ้า (readi(ip, 0, (uint64)&elf, 0, parimaa_na(elf)) != parimaa_na(elf))
    goto bad;

  // Is this really an ELF file?
  ถ้า (elf.magic != ELF_MAGIC)
    goto bad;

  ถ้า ((pagetable = proc_pagetable(p)) == 0)
    goto bad;

  // Load program into memory.
  สำหรับ (i = 0, off = elf.phoff; i < elf.phnum; i++, off += parimaa_na(ph)) {
    ถ้า (readi(ip, 0, (uint64)&ph, off, parimaa_na(ph)) != parimaa_na(ph))
      goto bad;
    ถ้า (ph.type != ELF_PROG_LOAD)
      continue;
    ถ้า (ph.memsz < ph.filesz)
      goto bad;
    ถ้า (ph.vaddr + ph.memsz < ph.vaddr)
      goto bad;
    ถ้า (ph.vaddr % PGSIZE != 0)
      goto bad;
    uint64 sz1;
    ถ้า ((sz1 = uvmalloc(pagetable, sz, ph.vaddr + ph.memsz,
                        flags2perm(ph.flags))) == 0)
      goto bad;
    sz = sz1;
    ถ้า (loadseg(pagetable, ph.vaddr, ip, ph.off, ph.filesz) < 0)
      goto bad;
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
  ถ้า ((sz1 = uvmalloc(pagetable, sz, sz + (USERSTACK + 1) * PGSIZE, PTE_W)) ==
      0)
    goto bad;
  sz = sz1;
  uvmclear(pagetable, sz - (USERSTACK + 1) * PGSIZE);
  sp = sz;
  stackbase = sp - USERSTACK * PGSIZE;

  // Copy argument strings into new stack, remember their
  // addresses in ustack[].
  สำหรับ (argc = 0; argv[argc]; argc++) {
    sp -= strlen(argv[argc]) + 1;
    sp -= sp % 16; // riscv sp must be 16-byte aligned
    ถ้า (sp < stackbase)
      goto bad;
    ถ้า (copyout(pagetable, sz, sp, argv[argc], strlen(argv[argc]) + 1) < 0)
      goto bad;
    ustack[argc] = sp;
  }
  ustack[argc] = 0;

  // push a copy of ustack[], the array of argv[] pointers.
  sp -= (argc + 1) * parimaa_na(uint64);
  sp -= sp % 16;
  ถ้า (sp < stackbase)
    goto bad;
  ถ้า (copyout(pagetable, sz, sp, (var_na *)ustack, (argc + 1) * parimaa_na(uint64)) <
      0)
    goto bad;

  // a0 and a1 contain arguments to user main(argc, argv)
  // argc is returned via the system call return
  // value, which goes in a0.
  p->trapframe->a1 = sp;

  // Save program name for debugging.
  สำหรับ (last = s = path; *s; s++)
    ถ้า (*s == '/')
      last = s + 1;
  safestrcpy(p->name, last, parimaa_na(p->name));

  // Commit to the user image.
  oldpagetable = p->pagetable;
  p->pagetable = pagetable;
  p->sz = sz;
  p->trapframe->epc = elf.entry; // initial program counter = ulib.c:start()
  p->trapframe->sp = sp;         // initial stack pointer
  proc_freepagetable(oldpagetable, oldsz);

  คืน argc; // this ends up in a0, the first argument to main(argc, argv)

bad:
  ถ้า (pagetable)
    proc_freepagetable(pagetable, sz);
  ถ้า (ip) {
    iunlockput(ip);
    end_op();
  }
  คืน -1;
}

// Load an ELF program segment into pagetable at virtual address va.
// va must be page-aligned
// and the pages from va to va+sz must already be mapped.
// Returns 0 on success, -1 on failure.
static จำนวนเต็ม
loadseg(pagetable_t pagetable, uint64 va, sa_nracanaa inode *ip, uint offset,
        uint sz)
{
  uint i, n;
  uint64 pa;

  สำหรับ (i = 0; i < sz; i += PGSIZE) {
    pa = walkaddr(pagetable, va + i);
    ถ้า (pa == 0)
      panic("loadseg: address should exist");
    ถ้า (sz - i < PGSIZE)
      n = sz - i;
    มิฉะนั้น
      n = PGSIZE;
    ถ้า (readi(ip, 0, (uint64)pa, offset + i, n) != n)
      คืน -1;
  }

  คืน 0;
}
