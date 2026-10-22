#അന്തര്ഭാവ "param.h"
#അന്തര്ഭാവ "types.h"
#അന്തര്ഭാവ "memlayout.h"
#അന്തര്ഭാവ "elf.h"
#അന്തര്ഭാവ "riscv.h"
#അന്തര്ഭാവ "defs.h"
#അന്തര്ഭാവ "spinlock.h"
#അന്തര്ഭാവ "proc.h"
#അന്തര്ഭാവ "fs.h"

/*
 * the kernel's page table.
 */
pagetable_t kernel_pagetable;

ബാഹ്യ അക്ഷരം etext[]; // kernel.ld sets this to end of kernel code.

ബാഹ്യ അക്ഷരം trampoline[]; // trampoline.S

// Make a direct-map page table for the kernel.
pagetable_t
kvmmake(ശൂന്യം)
{
  pagetable_t kpgtbl;

  kpgtbl = (pagetable_t)kalloc();
  memset(kpgtbl, 0, PGSIZE);

  // uart registers
  kvmmap(kpgtbl, UART0, UART0, PGSIZE, PTE_R | PTE_W);

  // virtio mmio disk interface
  kvmmap(kpgtbl, VIRTIO0, VIRTIO0, PGSIZE, PTE_R | PTE_W);

  // PLIC
  kvmmap(kpgtbl, PLIC, PLIC, 0x4000000, PTE_R | PTE_W);

  // map kernel text executable and read-only.
  kvmmap(kpgtbl, KERNBASE, KERNBASE, (uint64)etext - KERNBASE, PTE_R | PTE_X);

  // map kernel data and the physical RAM we'll make use of.
  kvmmap(kpgtbl, (uint64)etext, (uint64)etext, PHYSTOP - (uint64)etext,
         PTE_R | PTE_W);

  // map the trampoline for trap entry/exit to
  // the highest virtual address in the kernel.
  kvmmap(kpgtbl, TRAMPOLINE, (uint64)trampoline, PGSIZE, PTE_R | PTE_X);

  // allocate and map a kernel stack for each process.
  proc_mapstacks(kpgtbl);

  തിരികെ kpgtbl;
}

// add a mapping to the kernel page table.
// only used when booting.
// does not flush TLB or enable paging.
ശൂന്യം
kvmmap(pagetable_t kpgtbl, uint64 va, uint64 pa, uint64 sz, പൂർണ്ണസംഖ്യ perm)
{
  എങ്കിൽ (mappages(kpgtbl, va, sz, pa, perm) != 0)
    panic("kvmmap");
}

// Initialize the kernel_pagetable, shared by all CPUs.
ശൂന്യം
kvminit(ശൂന്യം)
{
  kernel_pagetable = kvmmake();
}

// Switch the current CPU's h/w page table register to
// the kernel's page table, and enable paging.
ശൂന്യം
kvminithart()
{
  // wait for any previous writes to the page table memory to finish.
  sfence_vma();

  w_satp(MAKE_SATP(kernel_pagetable));

  // flush stale entries from the TLB.
  sfence_vma();
}

// Return the address of the PTE in page table pagetable
// that corresponds to virtual address va.  If alloc!=0,
// create any required page-table pages.
//
// The risc-v Sv39 scheme has three levels of page-table
// pages. A page-table page contains 512 64-bit PTEs.
// A 64-bit virtual address is split into five fields:
//   39..63 -- must be zero.
//   30..38 -- 9 bits of level-2 index.
//   21..29 -- 9 bits of level-1 index.
//   12..20 -- 9 bits of level-0 index.
//    0..11 -- 12 bits of byte offset within the page.
pte_t *
walk(pagetable_t pagetable, uint64 va, പൂർണ്ണസംഖ്യ alloc)
{
  എങ്കിൽ (va >= MAXVA)
    panic("walk");

  വേണ്ടി (പൂർണ്ണസംഖ്യ level = 2; level > 0; level--) {
    pte_t *pte = &pagetable[PX(level, va)];
    എങ്കിൽ (*pte & PTE_V) {
      pagetable = (pagetable_t)PTE2PA(*pte);
    } അല്ലെങ്കിൽ {
      എങ്കിൽ (!alloc || (pagetable = (pde_t *)kalloc()) == 0)
        തിരികെ 0;
      memset(pagetable, 0, PGSIZE);
      *pte = PA2PTE(pagetable) | PTE_V;
    }
  }
  തിരികെ &pagetable[PX(0, va)];
}

// Look up a virtual address, return the physical address,
// or 0 if not mapped.
// Can only be used to look up user pages.
uint64
walkaddr(pagetable_t pagetable, uint64 va)
{
  pte_t *pte;
  uint64 pa;

  എങ്കിൽ (va >= MAXVA)
    തിരികെ 0;

  pte = walk(pagetable, va, 0);
  എങ്കിൽ (pte == 0)
    തിരികെ 0;
  എങ്കിൽ ((*pte & PTE_V) == 0)
    തിരികെ 0;
  എങ്കിൽ ((*pte & PTE_U) == 0)
    തിരികെ 0;
  pa = PTE2PA(*pte);
  തിരികെ pa;
}

// Create PTEs for virtual addresses starting at va that refer to
// physical addresses starting at pa.
// va and size MUST be page-aligned.
// Returns 0 on success, -1 if walk() couldn't
// allocate a needed page-table page.
പൂർണ്ണസംഖ്യ
mappages(pagetable_t pagetable, uint64 va, uint64 size, uint64 pa, പൂർണ്ണസംഖ്യ perm)
{
  uint64 a, last;
  pte_t *pte;

  എങ്കിൽ ((va % PGSIZE) != 0)
    panic("mappages: va not aligned");

  എങ്കിൽ ((size % PGSIZE) != 0)
    panic("mappages: size not aligned");

  എങ്കിൽ (size == 0)
    panic("mappages: size");

  a = va;
  last = va + size - PGSIZE;
  വേണ്ടി (;;) {
    എങ്കിൽ ((pte = walk(pagetable, a, 1)) == 0)
      തിരികെ -1;
    എങ്കിൽ (*pte & PTE_V)
      panic("mappages: remap");
    *pte = PA2PTE(pa) | perm | PTE_V;
    എങ്കിൽ (a == last)
      നിർത്തുക;
    a += PGSIZE;
    pa += PGSIZE;
  }
  തിരികെ 0;
}

// create an empty user page table.
// returns 0 if out of memory.
pagetable_t
uvmcreate()
{
  pagetable_t pagetable;
  pagetable = (pagetable_t)kalloc();
  എങ്കിൽ (pagetable == 0)
    തിരികെ 0;
  memset(pagetable, 0, PGSIZE);
  തിരികെ pagetable;
}

// Remove npages of mappings starting from va. va must be
// page-aligned. It's OK if the mappings don't exist.
// Optionally free the physical memory.
ശൂന്യം
uvmunmap(pagetable_t pagetable, uint64 va, uint64 npages, പൂർണ്ണസംഖ്യ do_free)
{
  uint64 a;
  pte_t *pte;

  എങ്കിൽ ((va % PGSIZE) != 0)
    panic("uvmunmap: not aligned");

  വേണ്ടി (a = va; a < va + npages * PGSIZE; a += PGSIZE) {
    എങ്കിൽ ((pte = walk(pagetable, a, 0)) == 0) // leaf page table entry allocated?
      തുടരുക;
    എങ്കിൽ ((*pte & PTE_V) == 0) // has physical page been allocated?
      തുടരുക;
    എങ്കിൽ (do_free) {
      uint64 pa = PTE2PA(*pte);
      kfree((ശൂന്യം *)pa);
    }
    *pte = 0;
  }
}

// Allocate PTEs and physical memory to grow a process from oldsz to
// newsz, which need not be page aligned.  Returns new size or 0 on error.
uint64
uvmalloc(pagetable_t pagetable, uint64 oldsz, uint64 newsz, പൂർണ്ണസംഖ്യ xperm)
{
  അക്ഷരം *mem;
  uint64 a;

  എങ്കിൽ (newsz < oldsz)
    തിരികെ oldsz;

  oldsz = PGROUNDUP(oldsz);
  വേണ്ടി (a = oldsz; a < newsz; a += PGSIZE) {
    mem = kalloc();
    എങ്കിൽ (mem == 0) {
      uvmdealloc(pagetable, a, oldsz);
      തിരികെ 0;
    }
    memset(mem, 0, PGSIZE);
    എങ്കിൽ (mappages(pagetable, a, PGSIZE, (uint64)mem, PTE_R | PTE_U | xperm) !=
        0) {
      kfree(mem);
      uvmdealloc(pagetable, a, oldsz);
      തിരികെ 0;
    }
  }
  തിരികെ newsz;
}

// Deallocate user pages to bring the process size from oldsz to
// newsz.  oldsz and newsz need not be page-aligned, nor does newsz
// need to be less than oldsz.  oldsz can be larger than the actual
// process size.  Returns the new process size.
uint64
uvmdealloc(pagetable_t pagetable, uint64 oldsz, uint64 newsz)
{
  എങ്കിൽ (newsz >= oldsz)
    തിരികെ oldsz;

  എങ്കിൽ (PGROUNDUP(newsz) < PGROUNDUP(oldsz)) {
    പൂർണ്ണസംഖ്യ npages = (PGROUNDUP(oldsz) - PGROUNDUP(newsz)) / PGSIZE;
    uvmunmap(pagetable, PGROUNDUP(newsz), npages, 1);
  }

  തിരികെ newsz;
}

// Recursively free page-table pages.
// All leaf mappings must already have been removed.
ശൂന്യം
freewalk(pagetable_t pagetable)
{
  // there are 2^9 = 512 PTEs in a page table.
  വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < 512; i++) {
    pte_t pte = pagetable[i];
    എങ്കിൽ ((pte & PTE_V) && (pte & (PTE_R | PTE_W | PTE_X)) == 0) {
      // this PTE points to a lower-level page table.
      uint64 child = PTE2PA(pte);
      freewalk((pagetable_t)child);
      pagetable[i] = 0;
    } അല്ലെങ്കിൽ എങ്കിൽ (pte & PTE_V) {
      panic("freewalk: leaf");
    }
  }
  kfree((ശൂന്യം *)pagetable);
}

// Free user memory pages,
// then free page-table pages.
ശൂന്യം
uvmfree(pagetable_t pagetable, uint64 sz)
{
  എങ്കിൽ (sz > 0)
    uvmunmap(pagetable, 0, PGROUNDUP(sz) / PGSIZE, 1);
  freewalk(pagetable);
}

// Given a parent process's page table, copy
// its memory into a child's page table.
// Copies both the page table and the
// physical memory.
// returns 0 on success, -1 on failure.
// frees any allocated pages on failure.
പൂർണ്ണസംഖ്യ
uvmcopy(pagetable_t old, pagetable_t new, uint64 sz)
{
  pte_t *pte;
  uint64 pa, i;
  uint flags;
  അക്ഷരം *mem;

  വേണ്ടി (i = 0; i < sz; i += PGSIZE) {
    എങ്കിൽ ((pte = walk(old, i, 0)) == 0)
      തുടരുക; // page table entry hasn't been allocated
    എങ്കിൽ ((*pte & PTE_V) == 0)
      തുടരുക; // physical page hasn't been allocated
    pa = PTE2PA(*pte);
    flags = PTE_FLAGS(*pte);
    എങ്കിൽ ((mem = kalloc()) == 0)
      പോകുക err;
    memmove(mem, (അക്ഷരം *)pa, PGSIZE);
    എങ്കിൽ (mappages(new, i, PGSIZE, (uint64)mem, flags) != 0) {
      kfree(mem);
      പോകുക err;
    }
  }
  തിരികെ 0;

err:
  uvmunmap(new, 0, i / PGSIZE, 1);
  തിരികെ -1;
}

// mark a PTE invalid for user access.
// used by exec for the user stack guard page.
ശൂന്യം
uvmclear(pagetable_t pagetable, uint64 va)
{
  pte_t *pte;

  pte = walk(pagetable, va, 0);
  എങ്കിൽ (pte == 0)
    panic("uvmclear");
  *pte &= ~PTE_U;
}

// Copy from kernel to user.
// Copy len bytes from src to virtual address dstva in a given page table.
// Return 0 on success, -1 on error.
പൂർണ്ണസംഖ്യ
copyout(pagetable_t pagetable, uint64 psz, uint64 dstva, അക്ഷരം *src, uint64 len)
{
  uint64 n, va0, pa0;
  pte_t *pte;

  ആകുമ്പോൾ (len > 0) {
    va0 = PGROUNDDOWN(dstva);
    എങ്കിൽ (va0 >= MAXVA)
      തിരികെ -1;

    pa0 = walkaddr(pagetable, va0);
    എങ്കിൽ (pa0 == 0) {
      എങ്കിൽ ((pa0 = vmfault(pagetable, psz, va0, 0)) == 0) {
        തിരികെ -1;
      }
    }

    pte = walk(pagetable, va0, 0);
    // forbid copyout over read-only user text pages.
    എങ്കിൽ ((*pte & PTE_W) == 0)
      തിരികെ -1;

    n = PGSIZE - (dstva - va0);
    എങ്കിൽ (n > len)
      n = len;
    memmove((ശൂന്യം *)(pa0 + (dstva - va0)), src, n);

    len -= n;
    src += n;
    dstva = va0 + PGSIZE;
  }
  തിരികെ 0;
}

// Copy from user to kernel.
// Copy len bytes to dst from virtual address srcva in a given page table.
// Return 0 on success, -1 on error.
പൂർണ്ണസംഖ്യ
copyin(pagetable_t pagetable, uint64 psz, അക്ഷരം *dst, uint64 srcva, uint64 len)
{
  uint64 n, va0, pa0;

  ആകുമ്പോൾ (len > 0) {
    va0 = PGROUNDDOWN(srcva);
    pa0 = walkaddr(pagetable, va0);
    എങ്കിൽ (pa0 == 0) {
      എങ്കിൽ ((pa0 = vmfault(pagetable, psz, va0, 1)) == 0) {
        തിരികെ -1;
      }
    }
    n = PGSIZE - (srcva - va0);
    എങ്കിൽ (n > len)
      n = len;
    memmove(dst, (ശൂന്യം *)(pa0 + (srcva - va0)), n);

    len -= n;
    dst += n;
    srcva = va0 + PGSIZE;
  }
  തിരികെ 0;
}

// Copy a null-terminated string from user to kernel.
// Copy bytes to dst from virtual address srcva in a given page table,
// until a '\0', or max.
// Return 0 on success, -1 on error.
പൂർണ്ണസംഖ്യ
copyinstr(pagetable_t pagetable, uint64 psz, അക്ഷരം *dst, uint64 srcva,
          uint64 max)
{
  uint64 n, va0, pa0;
  പൂർണ്ണസംഖ്യ got_null = 0;

  ആകുമ്പോൾ (got_null == 0 && max > 0) {
    va0 = PGROUNDDOWN(srcva);
    pa0 = walkaddr(pagetable, va0);
    എങ്കിൽ (pa0 == 0) {
      എങ്കിൽ ((pa0 = vmfault(pagetable, psz, va0, 1)) == 0) {
        തിരികെ -1;
      }
    }
    n = PGSIZE - (srcva - va0);
    എങ്കിൽ (n > max)
      n = max;

    അക്ഷരം *p = (അക്ഷരം *)(pa0 + (srcva - va0));
    ആകുമ്പോൾ (n > 0) {
      എങ്കിൽ (*p == '\0') {
        *dst = '\0';
        got_null = 1;
        നിർത്തുക;
      } അല്ലെങ്കിൽ {
        *dst = *p;
      }
      --n;
      --max;
      p++;
      dst++;
    }

    srcva = va0 + PGSIZE;
  }
  എങ്കിൽ (got_null) {
    തിരികെ 0;
  } അല്ലെങ്കിൽ {
    തിരികെ -1;
  }
}

// allocate and map user memory if process is referencing a page
// that was lazily allocated in sys_sbrk().
// returns 0 if va is invalid or already mapped, or if
// out of physical memory, and physical address if successful.
uint64
vmfault(pagetable_t pagetable, uint64 psz, uint64 va, പൂർണ്ണസംഖ്യ പഠ)
{
  uint64 mem;

  എങ്കിൽ (va >= psz)
    തിരികെ 0;
  va = PGROUNDDOWN(va);
  എങ്കിൽ (ismapped(pagetable, va)) {
    തിരികെ 0;
  }
  mem = (uint64)kalloc();
  എങ്കിൽ (mem == 0)
    തിരികെ 0;
  memset((ശൂന്യം *)mem, 0, PGSIZE);
  എങ്കിൽ (mappages(pagetable, va, PGSIZE, mem, PTE_W | PTE_U | PTE_R) != 0) {
    kfree((ശൂന്യം *)mem);
    തിരികെ 0;
  }
  തിരികെ mem;
}

പൂർണ്ണസംഖ്യ
ismapped(pagetable_t pagetable, uint64 va)
{
  pte_t *pte = walk(pagetable, va, 0);
  എങ്കിൽ (pte == 0) {
    തിരികെ 0;
  }
  എങ്കിൽ (*pte & PTE_V) {
    തിരികെ 1;
  }
  തിരികെ 0;
}
