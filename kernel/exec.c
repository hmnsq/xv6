#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "spinlock.h"
#include "proc.h"
#include "defs.h"
#include "elf.h"

static 整数 loadseg(pde_t *, uint64, struct inode *, uint, uint);

// map ELF permissions to PTE permission bits.
整数
flags2perm(整数 flags)
{
  整数 perm = 0;
  もし (flags & 0x1)
    perm = PTE_X;
  もし (flags & 0x2)
    perm |= PTE_W;
  戻る perm;
}

//
// the implementation of the exec() system call
//
整数
kexec(char *path, char **argv)
{
  char *s, *last;
  整数 i, off;
  uint64 argc, sz = 0, sp, ustack[MAXARG], stackbase;
  struct elfhdr elf;
  struct inode *ip;
  struct proghdr ph;
  pagetable_t pagetable = 0, oldpagetable;
  struct proc *p = myproc();

  begin_op();

  // Open the executable file.
  もし ((ip = namei(path)) == 0) {
    end_op();
    戻る -1;
  }
  ilock(ip);

  // Read the ELF header.
  もし (readi(ip, 0, (uint64)&elf, 0, sizeof(elf)) != sizeof(elf))
    goto bad;

  // Is this really an ELF file?
  もし (elf.magic != ELF_MAGIC)
    goto bad;

  もし ((pagetable = proc_pagetable(p)) == 0)
    goto bad;

  // Load program into memory.
  ために (i = 0, off = elf.phoff; i < elf.phnum; i++, off += sizeof(ph)) {
    もし (readi(ip, 0, (uint64)&ph, off, sizeof(ph)) != sizeof(ph))
      goto bad;
    もし (ph.type != ELF_PROG_LOAD)
      continue;
    もし (ph.memsz < ph.filesz)
      goto bad;
    もし (ph.vaddr + ph.memsz < ph.vaddr)
      goto bad;
    もし (ph.vaddr % PGSIZE != 0)
      goto bad;
    uint64 sz1;
    もし ((sz1 = uvmalloc(pagetable, sz, ph.vaddr + ph.memsz,
                        flags2perm(ph.flags))) == 0)
      goto bad;
    sz = sz1;
    もし (loadseg(pagetable, ph.vaddr, ip, ph.off, ph.filesz) < 0)
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
  もし ((sz1 = uvmalloc(pagetable, sz, sz + (USERSTACK + 1) * PGSIZE, PTE_W)) ==
      0)
    goto bad;
  sz = sz1;
  uvmclear(pagetable, sz - (USERSTACK + 1) * PGSIZE);
  sp = sz;
  stackbase = sp - USERSTACK * PGSIZE;

  // Copy argument strings into new stack, remember their
  // addresses in ustack[].
  ために (argc = 0; argv[argc]; argc++) {
    sp -= strlen(argv[argc]) + 1;
    sp -= sp % 16; // riscv sp must be 16-byte aligned
    もし (sp < stackbase)
      goto bad;
    もし (copyout(pagetable, sz, sp, argv[argc], strlen(argv[argc]) + 1) < 0)
      goto bad;
    ustack[argc] = sp;
  }
  ustack[argc] = 0;

  // push a copy of ustack[], the array of argv[] pointers.
  sp -= (argc + 1) * sizeof(uint64);
  sp -= sp % 16;
  もし (sp < stackbase)
    goto bad;
  もし (copyout(pagetable, sz, sp, (char *)ustack, (argc + 1) * sizeof(uint64)) <
      0)
    goto bad;

  // a0 and a1 contain arguments to user main(argc, argv)
  // argc is returned via the system call return
  // value, which goes in a0.
  p->trapframe->a1 = sp;

  // Save program name for debugging.
  ために (last = s = path; *s; s++)
    もし (*s == '/')
      last = s + 1;
  safestrcpy(p->name, last, sizeof(p->name));

  // Commit to the user image.
  oldpagetable = p->pagetable;
  p->pagetable = pagetable;
  p->sz = sz;
  p->trapframe->epc = elf.entry; // initial program counter = ulib.c:start()
  p->trapframe->sp = sp;         // initial stack pointer
  proc_freepagetable(oldpagetable, oldsz);

  戻る argc; // this ends up in a0, the first argument to main(argc, argv)

bad:
  もし (pagetable)
    proc_freepagetable(pagetable, sz);
  もし (ip) {
    iunlockput(ip);
    end_op();
  }
  戻る -1;
}

// Load an ELF program segment into pagetable at virtual address va.
// va must be page-aligned
// and the pages from va to va+sz must already be mapped.
// Returns 0 on success, -1 on failure.
static 整数
loadseg(pagetable_t pagetable, uint64 va, struct inode *ip, uint offset,
        uint sz)
{
  uint i, n;
  uint64 pa;

  ために (i = 0; i < sz; i += PGSIZE) {
    pa = walkaddr(pagetable, va + i);
    もし (pa == 0)
      panic("loadseg: address should exist");
    もし (sz - i < PGSIZE)
      n = sz - i;
    否则
      n = PGSIZE;
    もし (readi(ip, 0, (uint64)pa, offset + i, n) != n)
      戻る -1;
  }

  戻る 0;
}
