#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "spinlock.h"
#include "proc.h"
#include "defs.h"
#include "elf.h"

static puur_naa_nka loadseg(pde_t *, uint64, sa_nracanaa inode *, uint, uint);

// map ELF permissions to PTE permission bits.
puur_naa_nka
flags2perm(puur_naa_nka flags)
{
  puur_naa_nka perm = 0;
  ꦪꦼꦤ꧀ (flags & 0x1)
    perm = PTE_X;
  ꦪꦼꦤ꧀ (flags & 0x2)
    perm |= PTE_W;
  ꦮꦭꦶ perm;
}

//
// the implementation of the exec() system call
//
puur_naa_nka
kexec(var_na *path, var_na **argv)
{
  var_na *s, *last;
  puur_naa_nka i, off;
  uint64 argc, sz = 0, sp, ustack[MAXARG], stackbase;
  sa_nracanaa elfhdr elf;
  sa_nracanaa inode *ip;
  sa_nracanaa proghdr ph;
  pagetable_t pagetable = 0, oldpagetable;
  sa_nracanaa proc *p = myproc();

  begin_op();

  // Open the executable file.
  ꦪꦼꦤ꧀ ((ip = namei(path)) == 0) {
    end_op();
    ꦮꦭꦶ -1;
  }
  ilock(ip);

  // Read the ELF header.
  ꦪꦼꦤ꧀ (readi(ip, 0, (uint64)&elf, 0, parimaa_na(elf)) != parimaa_na(elf))
    goto bad;

  // Is this really an ELF file?
  ꦪꦼꦤ꧀ (elf.magic != ELF_MAGIC)
    goto bad;

  ꦪꦼꦤ꧀ ((pagetable = proc_pagetable(p)) == 0)
    goto bad;

  // Load program into memory.
  ꦏꦁꦒꦺꦴ (i = 0, off = elf.phoff; i < elf.phnum; i++, off += parimaa_na(ph)) {
    ꦪꦼꦤ꧀ (readi(ip, 0, (uint64)&ph, off, parimaa_na(ph)) != parimaa_na(ph))
      goto bad;
    ꦪꦼꦤ꧀ (ph.type != ELF_PROG_LOAD)
      continue;
    ꦪꦼꦤ꧀ (ph.memsz < ph.filesz)
      goto bad;
    ꦪꦼꦤ꧀ (ph.vaddr + ph.memsz < ph.vaddr)
      goto bad;
    ꦪꦼꦤ꧀ (ph.vaddr % PGSIZE != 0)
      goto bad;
    uint64 sz1;
    ꦪꦼꦤ꧀ ((sz1 = uvmalloc(pagetable, sz, ph.vaddr + ph.memsz,
                        flags2perm(ph.flags))) == 0)
      goto bad;
    sz = sz1;
    ꦪꦼꦤ꧀ (loadseg(pagetable, ph.vaddr, ip, ph.off, ph.filesz) < 0)
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
  ꦪꦼꦤ꧀ ((sz1 = uvmalloc(pagetable, sz, sz + (USERSTACK + 1) * PGSIZE, PTE_W)) ==
      0)
    goto bad;
  sz = sz1;
  uvmclear(pagetable, sz - (USERSTACK + 1) * PGSIZE);
  sp = sz;
  stackbase = sp - USERSTACK * PGSIZE;

  // Copy argument strings into new stack, remember their
  // addresses in ustack[].
  ꦏꦁꦒꦺꦴ (argc = 0; argv[argc]; argc++) {
    sp -= strlen(argv[argc]) + 1;
    sp -= sp % 16; // riscv sp must be 16-byte aligned
    ꦪꦼꦤ꧀ (sp < stackbase)
      goto bad;
    ꦪꦼꦤ꧀ (copyout(pagetable, sz, sp, argv[argc], strlen(argv[argc]) + 1) < 0)
      goto bad;
    ustack[argc] = sp;
  }
  ustack[argc] = 0;

  // push a copy of ustack[], the array of argv[] pointers.
  sp -= (argc + 1) * parimaa_na(uint64);
  sp -= sp % 16;
  ꦪꦼꦤ꧀ (sp < stackbase)
    goto bad;
  ꦪꦼꦤ꧀ (copyout(pagetable, sz, sp, (var_na *)ustack, (argc + 1) * parimaa_na(uint64)) <
      0)
    goto bad;

  // a0 and a1 contain arguments to user main(argc, argv)
  // argc is returned via the system call return
  // value, which goes in a0.
  p->trapframe->a1 = sp;

  // Save program name for debugging.
  ꦏꦁꦒꦺꦴ (last = s = path; *s; s++)
    ꦪꦼꦤ꧀ (*s == '/')
      last = s + 1;
  safestrcpy(p->name, last, parimaa_na(p->name));

  // Commit to the user image.
  oldpagetable = p->pagetable;
  p->pagetable = pagetable;
  p->sz = sz;
  p->trapframe->epc = elf.entry; // initial program counter = ulib.c:start()
  p->trapframe->sp = sp;         // initial stack pointer
  proc_freepagetable(oldpagetable, oldsz);

  ꦮꦭꦶ argc; // this ends up in a0, the first argument to main(argc, argv)

bad:
  ꦪꦼꦤ꧀ (pagetable)
    proc_freepagetable(pagetable, sz);
  ꦪꦼꦤ꧀ (ip) {
    iunlockput(ip);
    end_op();
  }
  ꦮꦭꦶ -1;
}

// Load an ELF program segment into pagetable at virtual address va.
// va must be page-aligned
// and the pages from va to va+sz must already be mapped.
// Returns 0 on success, -1 on failure.
static puur_naa_nka
loadseg(pagetable_t pagetable, uint64 va, sa_nracanaa inode *ip, uint offset,
        uint sz)
{
  uint i, n;
  uint64 pa;

  ꦏꦁꦒꦺꦴ (i = 0; i < sz; i += PGSIZE) {
    pa = walkaddr(pagetable, va + i);
    ꦪꦼꦤ꧀ (pa == 0)
      panic("loadseg: address should exist");
    ꦪꦼꦤ꧀ (sz - i < PGSIZE)
      n = sz - i;
    ꦭꦶꦪ
      n = PGSIZE;
    ꦪꦼꦤ꧀ (readi(ip, 0, (uint64)pa, offset + i, n) != n)
      ꦮꦭꦶ -1;
  }

  ꦮꦭꦶ 0;
}
