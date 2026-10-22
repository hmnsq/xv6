#అన్తర్భావ "param.h"
#అన్తర్భావ "types.h"
#అన్తర్భావ "memlayout.h"
#అన్తర్భావ "elf.h"
#అన్తర్భావ "riscv.h"
#అన్తర్భావ "defs.h"
#అన్తర్భావ "spinlock.h"
#అన్తర్భావ "proc.h"
#అన్తర్భావ "fs.h"

/*
 * the kernel's page table.
 */
pagetable_t kernel_pagetable;

బాహ్య అక్షరం etext[]; // kernel.ld sets this to end of kernel code.

బాహ్య అక్షరం trampoline[]; // trampoline.S

// Make a direct-map page table for the kernel.
pagetable_t
kvmmake(శూన్య)
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

  తిరిగి kpgtbl;
}

// add a mapping to the kernel page table.
// only used when booting.
// does not flush TLB or enable paging.
శూన్య
kvmmap(pagetable_t kpgtbl, uint64 va, uint64 pa, uint64 sz, పూర్ణాంకం perm)
{
  ఒకవేళ (mappages(kpgtbl, va, sz, pa, perm) != 0)
    panic("kvmmap");
}

// Initialize the kernel_pagetable, shared by all CPUs.
శూన్య
kvminit(శూన్య)
{
  kernel_pagetable = kvmmake();
}

// Switch the current CPU's h/w page table register to
// the kernel's page table, and enable paging.
శూన్య
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
walk(pagetable_t pagetable, uint64 va, పూర్ణాంకం alloc)
{
  ఒకవేళ (va >= MAXVA)
    panic("walk");

  కొరకు (పూర్ణాంకం level = 2; level > 0; level--) {
    pte_t *pte = &pagetable[PX(level, va)];
    ఒకవేళ (*pte & PTE_V) {
      pagetable = (pagetable_t)PTE2PA(*pte);
    } లేకపోతే {
      ఒకవేళ (!alloc || (pagetable = (pde_t *)kalloc()) == 0)
        తిరిగి 0;
      memset(pagetable, 0, PGSIZE);
      *pte = PA2PTE(pagetable) | PTE_V;
    }
  }
  తిరిగి &pagetable[PX(0, va)];
}

// Look up a virtual address, return the physical address,
// or 0 if not mapped.
// Can only be used to look up user pages.
uint64
walkaddr(pagetable_t pagetable, uint64 va)
{
  pte_t *pte;
  uint64 pa;

  ఒకవేళ (va >= MAXVA)
    తిరిగి 0;

  pte = walk(pagetable, va, 0);
  ఒకవేళ (pte == 0)
    తిరిగి 0;
  ఒకవేళ ((*pte & PTE_V) == 0)
    తిరిగి 0;
  ఒకవేళ ((*pte & PTE_U) == 0)
    తిరిగి 0;
  pa = PTE2PA(*pte);
  తిరిగి pa;
}

// Create PTEs for virtual addresses starting at va that refer to
// physical addresses starting at pa.
// va and size MUST be page-aligned.
// Returns 0 on success, -1 if walk() couldn't
// allocate a needed page-table page.
పూర్ణాంకం
mappages(pagetable_t pagetable, uint64 va, uint64 కొలత, uint64 pa, పూర్ణాంకం perm)
{
  uint64 a, చివరి;
  pte_t *pte;

  ఒకవేళ ((va % PGSIZE) != 0)
    panic("mappages: va not aligned");

  ఒకవేళ ((కొలత % PGSIZE) != 0)
    panic("mappages: size not aligned");

  ఒకవేళ (కొలత == 0)
    panic("mappages: size");

  a = va;
  చివరి = va + కొలత - PGSIZE;
  కొరకు (;;) {
    ఒకవేళ ((pte = walk(pagetable, a, 1)) == 0)
      తిరిగి -1;
    ఒకవేళ (*pte & PTE_V)
      panic("mappages: remap");
    *pte = PA2PTE(pa) | perm | PTE_V;
    ఒకవేళ (a == చివరి)
      ఆపు;
    a += PGSIZE;
    pa += PGSIZE;
  }
  తిరిగి 0;
}

// create an empty user page table.
// returns 0 if out of memory.
pagetable_t
uvmcreate()
{
  pagetable_t pagetable;
  pagetable = (pagetable_t)kalloc();
  ఒకవేళ (pagetable == 0)
    తిరిగి 0;
  memset(pagetable, 0, PGSIZE);
  తిరిగి pagetable;
}

// Remove npages of mappings starting from va. va must be
// page-aligned. It's OK if the mappings don't exist.
// Optionally free the physical memory.
శూన్య
uvmunmap(pagetable_t pagetable, uint64 va, uint64 npages, పూర్ణాంకం do_free)
{
  uint64 a;
  pte_t *pte;

  ఒకవేళ ((va % PGSIZE) != 0)
    panic("uvmunmap: not aligned");

  కొరకు (a = va; a < va + npages * PGSIZE; a += PGSIZE) {
    ఒకవేళ ((pte = walk(pagetable, a, 0)) == 0) // leaf page table entry allocated?
      కొనసాగు;
    ఒకవేళ ((*pte & PTE_V) == 0) // has physical page been allocated?
      కొనసాగు;
    ఒకవేళ (do_free) {
      uint64 pa = PTE2PA(*pte);
      kfree((శూన్య *)pa);
    }
    *pte = 0;
  }
}

// Allocate PTEs and physical memory to grow a process from oldsz to
// newsz, which need not be page aligned.  Returns new size or 0 on error.
uint64
uvmalloc(pagetable_t pagetable, uint64 oldsz, uint64 newsz, పూర్ణాంకం xperm)
{
  అక్షరం *mem;
  uint64 a;

  ఒకవేళ (newsz < oldsz)
    తిరిగి oldsz;

  oldsz = PGROUNDUP(oldsz);
  కొరకు (a = oldsz; a < newsz; a += PGSIZE) {
    mem = kalloc();
    ఒకవేళ (mem == 0) {
      uvmdealloc(pagetable, a, oldsz);
      తిరిగి 0;
    }
    memset(mem, 0, PGSIZE);
    ఒకవేళ (mappages(pagetable, a, PGSIZE, (uint64)mem, PTE_R | PTE_U | xperm) !=
        0) {
      kfree(mem);
      uvmdealloc(pagetable, a, oldsz);
      తిరిగి 0;
    }
  }
  తిరిగి newsz;
}

// Deallocate user pages to bring the process size from oldsz to
// newsz.  oldsz and newsz need not be page-aligned, nor does newsz
// need to be less than oldsz.  oldsz can be larger than the actual
// process size.  Returns the new process size.
uint64
uvmdealloc(pagetable_t pagetable, uint64 oldsz, uint64 newsz)
{
  ఒకవేళ (newsz >= oldsz)
    తిరిగి oldsz;

  ఒకవేళ (PGROUNDUP(newsz) < PGROUNDUP(oldsz)) {
    పూర్ణాంకం npages = (PGROUNDUP(oldsz) - PGROUNDUP(newsz)) / PGSIZE;
    uvmunmap(pagetable, PGROUNDUP(newsz), npages, 1);
  }

  తిరిగి newsz;
}

// Recursively free page-table pages.
// All leaf mappings must already have been removed.
శూన్య
freewalk(pagetable_t pagetable)
{
  // there are 2^9 = 512 PTEs in a page table.
  కొరకు (పూర్ణాంకం i = 0; i < 512; i++) {
    pte_t pte = pagetable[i];
    ఒకవేళ ((pte & PTE_V) && (pte & (PTE_R | PTE_W | PTE_X)) == 0) {
      // this PTE points to a lower-level page table.
      uint64 బిడ్డ = PTE2PA(pte);
      freewalk((pagetable_t)బిడ్డ);
      pagetable[i] = 0;
    } లేకపోతే ఒకవేళ (pte & PTE_V) {
      panic("freewalk: leaf");
    }
  }
  kfree((శూన్య *)pagetable);
}

// Free user memory pages,
// then free page-table pages.
శూన్య
uvmfree(pagetable_t pagetable, uint64 sz)
{
  ఒకవేళ (sz > 0)
    uvmunmap(pagetable, 0, PGROUNDUP(sz) / PGSIZE, 1);
  freewalk(pagetable);
}

// Given a parent process's page table, copy
// its memory into a child's page table.
// Copies both the page table and the
// physical memory.
// returns 0 on success, -1 on failure.
// frees any allocated pages on failure.
పూర్ణాంకం
uvmcopy(pagetable_t old, pagetable_t new, uint64 sz)
{
  pte_t *pte;
  uint64 pa, i;
  uint flags;
  అక్షరం *mem;

  కొరకు (i = 0; i < sz; i += PGSIZE) {
    ఒకవేళ ((pte = walk(old, i, 0)) == 0)
      కొనసాగు; // page table entry hasn't been allocated
    ఒకవేళ ((*pte & PTE_V) == 0)
      కొనసాగు; // physical page hasn't been allocated
    pa = PTE2PA(*pte);
    flags = PTE_FLAGS(*pte);
    ఒకవేళ ((mem = kalloc()) == 0)
      వెళ్ళు err;
    memmove(mem, (అక్షరం *)pa, PGSIZE);
    ఒకవేళ (mappages(new, i, PGSIZE, (uint64)mem, flags) != 0) {
      kfree(mem);
      వెళ్ళు err;
    }
  }
  తిరిగి 0;

err:
  uvmunmap(new, 0, i / PGSIZE, 1);
  తిరిగి -1;
}

// mark a PTE invalid for user access.
// used by exec for the user stack guard page.
శూన్య
uvmclear(pagetable_t pagetable, uint64 va)
{
  pte_t *pte;

  pte = walk(pagetable, va, 0);
  ఒకవేళ (pte == 0)
    panic("uvmclear");
  *pte &= ~PTE_U;
}

// Copy from kernel to user.
// Copy len bytes from src to virtual address dstva in a given page table.
// Return 0 on success, -1 on error.
పూర్ణాంకం
copyout(pagetable_t pagetable, uint64 psz, uint64 dstva, అక్షరం *src, uint64 len)
{
  uint64 n, va0, pa0;
  pte_t *pte;

  అయితే (len > 0) {
    va0 = PGROUNDDOWN(dstva);
    ఒకవేళ (va0 >= MAXVA)
      తిరిగి -1;

    pa0 = walkaddr(pagetable, va0);
    ఒకవేళ (pa0 == 0) {
      ఒకవేళ ((pa0 = vmfault(pagetable, psz, va0, 0)) == 0) {
        తిరిగి -1;
      }
    }

    pte = walk(pagetable, va0, 0);
    // forbid copyout over read-only user text pages.
    ఒకవేళ ((*pte & PTE_W) == 0)
      తిరిగి -1;

    n = PGSIZE - (dstva - va0);
    ఒకవేళ (n > len)
      n = len;
    memmove((శూన్య *)(pa0 + (dstva - va0)), src, n);

    len -= n;
    src += n;
    dstva = va0 + PGSIZE;
  }
  తిరిగి 0;
}

// Copy from user to kernel.
// Copy len bytes to dst from virtual address srcva in a given page table.
// Return 0 on success, -1 on error.
పూర్ణాంకం
copyin(pagetable_t pagetable, uint64 psz, అక్షరం *dst, uint64 srcva, uint64 len)
{
  uint64 n, va0, pa0;

  అయితే (len > 0) {
    va0 = PGROUNDDOWN(srcva);
    pa0 = walkaddr(pagetable, va0);
    ఒకవేళ (pa0 == 0) {
      ఒకవేళ ((pa0 = vmfault(pagetable, psz, va0, 1)) == 0) {
        తిరిగి -1;
      }
    }
    n = PGSIZE - (srcva - va0);
    ఒకవేళ (n > len)
      n = len;
    memmove(dst, (శూన్య *)(pa0 + (srcva - va0)), n);

    len -= n;
    dst += n;
    srcva = va0 + PGSIZE;
  }
  తిరిగి 0;
}

// Copy a null-terminated string from user to kernel.
// Copy bytes to dst from virtual address srcva in a given page table,
// until a '\0', or max.
// Return 0 on success, -1 on error.
పూర్ణాంకం
copyinstr(pagetable_t pagetable, uint64 psz, అక్షరం *dst, uint64 srcva,
          uint64 max)
{
  uint64 n, va0, pa0;
  పూర్ణాంకం got_null = 0;

  అయితే (got_null == 0 && max > 0) {
    va0 = PGROUNDDOWN(srcva);
    pa0 = walkaddr(pagetable, va0);
    ఒకవేళ (pa0 == 0) {
      ఒకవేళ ((pa0 = vmfault(pagetable, psz, va0, 1)) == 0) {
        తిరిగి -1;
      }
    }
    n = PGSIZE - (srcva - va0);
    ఒకవేళ (n > max)
      n = max;

    అక్షరం *p = (అక్షరం *)(pa0 + (srcva - va0));
    అయితే (n > 0) {
      ఒకవేళ (*p == '\0') {
        *dst = '\0';
        got_null = 1;
        ఆపు;
      } లేకపోతే {
        *dst = *p;
      }
      --n;
      --max;
      p++;
      dst++;
    }

    srcva = va0 + PGSIZE;
  }
  ఒకవేళ (got_null) {
    తిరిగి 0;
  } లేకపోతే {
    తిరిగి -1;
  }
}

// allocate and map user memory if process is referencing a page
// that was lazily allocated in sys_sbrk().
// returns 0 if va is invalid or already mapped, or if
// out of physical memory, and physical address if successful.
uint64
vmfault(pagetable_t pagetable, uint64 psz, uint64 va, పూర్ణాంకం పఠ)
{
  uint64 mem;

  ఒకవేళ (va >= psz)
    తిరిగి 0;
  va = PGROUNDDOWN(va);
  ఒకవేళ (ismapped(pagetable, va)) {
    తిరిగి 0;
  }
  mem = (uint64)kalloc();
  ఒకవేళ (mem == 0)
    తిరిగి 0;
  memset((శూన్య *)mem, 0, PGSIZE);
  ఒకవేళ (mappages(pagetable, va, PGSIZE, mem, PTE_W | PTE_U | PTE_R) != 0) {
    kfree((శూన్య *)mem);
    తిరిగి 0;
  }
  తిరిగి mem;
}

పూర్ణాంకం
ismapped(pagetable_t pagetable, uint64 va)
{
  pte_t *pte = walk(pagetable, va, 0);
  ఒకవేళ (pte == 0) {
    తిరిగి 0;
  }
  ఒకవేళ (*pte & PTE_V) {
    తిరిగి 1;
  }
  తిరిగి 0;
}
