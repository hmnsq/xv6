#ಅನ್ತರ್ಭಾವ "param.h"
#ಅನ್ತರ್ಭಾವ "types.h"
#ಅನ್ತರ್ಭಾವ "memlayout.h"
#ಅನ್ತರ್ಭಾವ "elf.h"
#ಅನ್ತರ್ಭಾವ "riscv.h"
#ಅನ್ತರ್ಭಾವ "defs.h"
#ಅನ್ತರ್ಭಾವ "spinlock.h"
#ಅನ್ತರ್ಭಾವ "proc.h"
#ಅನ್ತರ್ಭಾವ "fs.h"

/*
 * the kernel's page table.
 */
pagetable_t kernel_pagetable;

ಬಾಹ್ಯ ವರ್ಣ etext[]; // kernel.ld sets this to end of kernel code.

ಬಾಹ್ಯ ವರ್ಣ trampoline[]; // trampoline.S

// Make a direct-map page table for the kernel.
pagetable_t
kvmmake(ಶೂನ್ಯ)
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

  ಪ್ರತ್ಯಾಗಮ kpgtbl;
}

// add a mapping to the kernel page table.
// only used when booting.
// does not flush TLB or enable paging.
ಶೂನ್ಯ
kvmmap(pagetable_t kpgtbl, uint64 va, uint64 pa, uint64 sz, ಪೂರ್ಣಾಙ್ಕ perm)
{
  ಯದಿ (mappages(kpgtbl, va, sz, pa, perm) != 0)
    panic("kvmmap");
}

// Initialize the kernel_pagetable, shared by all CPUs.
ಶೂನ್ಯ
kvminit(ಶೂನ್ಯ)
{
  kernel_pagetable = kvmmake();
}

// Switch the current CPU's h/w page table register to
// the kernel's page table, and enable paging.
ಶೂನ್ಯ
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
walk(pagetable_t pagetable, uint64 va, ಪೂರ್ಣಾಙ್ಕ alloc)
{
  ಯದಿ (va >= MAXVA)
    panic("walk");

  ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ level = 2; level > 0; level--) {
    pte_t *pte = &pagetable[PX(level, va)];
    ಯದಿ (*pte & PTE_V) {
      pagetable = (pagetable_t)PTE2PA(*pte);
    } ಅನ್ಯಥಾ {
      ಯದಿ (!alloc || (pagetable = (pde_t *)kalloc()) == 0)
        ಪ್ರತ್ಯಾಗಮ 0;
      memset(pagetable, 0, PGSIZE);
      *pte = PA2PTE(pagetable) | PTE_V;
    }
  }
  ಪ್ರತ್ಯಾಗಮ &pagetable[PX(0, va)];
}

// Look up a virtual address, return the physical address,
// or 0 if not mapped.
// Can only be used to look up user pages.
uint64
walkaddr(pagetable_t pagetable, uint64 va)
{
  pte_t *pte;
  uint64 pa;

  ಯದಿ (va >= MAXVA)
    ಪ್ರತ್ಯಾಗಮ 0;

  pte = walk(pagetable, va, 0);
  ಯದಿ (pte == 0)
    ಪ್ರತ್ಯಾಗಮ 0;
  ಯದಿ ((*pte & PTE_V) == 0)
    ಪ್ರತ್ಯಾಗಮ 0;
  ಯದಿ ((*pte & PTE_U) == 0)
    ಪ್ರತ್ಯಾಗಮ 0;
  pa = PTE2PA(*pte);
  ಪ್ರತ್ಯಾಗಮ pa;
}

// Create PTEs for virtual addresses starting at va that refer to
// physical addresses starting at pa.
// va and size MUST be page-aligned.
// Returns 0 on success, -1 if walk() couldn't
// allocate a needed page-table page.
ಪೂರ್ಣಾಙ್ಕ
mappages(pagetable_t pagetable, uint64 va, uint64 size, uint64 pa, ಪೂರ್ಣಾಙ್ಕ perm)
{
  uint64 a, last;
  pte_t *pte;

  ಯದಿ ((va % PGSIZE) != 0)
    panic("mappages: va not aligned");

  ಯದಿ ((size % PGSIZE) != 0)
    panic("mappages: size not aligned");

  ಯದಿ (size == 0)
    panic("mappages: size");

  a = va;
  last = va + size - PGSIZE;
  ಪ್ರತಿ (;;) {
    ಯದಿ ((pte = walk(pagetable, a, 1)) == 0)
      ಪ್ರತ್ಯಾಗಮ -1;
    ಯದಿ (*pte & PTE_V)
      panic("mappages: remap");
    *pte = PA2PTE(pa) | perm | PTE_V;
    ಯದಿ (a == last)
      ಭಙ್ಗ;
    a += PGSIZE;
    pa += PGSIZE;
  }
  ಪ್ರತ್ಯಾಗಮ 0;
}

// create an empty user page table.
// returns 0 if out of memory.
pagetable_t
uvmcreate()
{
  pagetable_t pagetable;
  pagetable = (pagetable_t)kalloc();
  ಯದಿ (pagetable == 0)
    ಪ್ರತ್ಯಾಗಮ 0;
  memset(pagetable, 0, PGSIZE);
  ಪ್ರತ್ಯಾಗಮ pagetable;
}

// Remove npages of mappings starting from va. va must be
// page-aligned. It's OK if the mappings don't exist.
// Optionally free the physical memory.
ಶೂನ್ಯ
uvmunmap(pagetable_t pagetable, uint64 va, uint64 npages, ಪೂರ್ಣಾಙ್ಕ do_free)
{
  uint64 a;
  pte_t *pte;

  ಯದಿ ((va % PGSIZE) != 0)
    panic("uvmunmap: not aligned");

  ಪ್ರತಿ (a = va; a < va + npages * PGSIZE; a += PGSIZE) {
    ಯದಿ ((pte = walk(pagetable, a, 0)) == 0) // leaf page table entry allocated?
      ಅನುವರ್ತ;
    ಯದಿ ((*pte & PTE_V) == 0) // has physical page been allocated?
      ಅನುವರ್ತ;
    ಯದಿ (do_free) {
      uint64 pa = PTE2PA(*pte);
      kfree((ಶೂನ್ಯ *)pa);
    }
    *pte = 0;
  }
}

// Allocate PTEs and physical memory to grow a process from oldsz to
// newsz, which need not be page aligned.  Returns new size or 0 on error.
uint64
uvmalloc(pagetable_t pagetable, uint64 oldsz, uint64 newsz, ಪೂರ್ಣಾಙ್ಕ xperm)
{
  ವರ್ಣ *mem;
  uint64 a;

  ಯದಿ (newsz < oldsz)
    ಪ್ರತ್ಯಾಗಮ oldsz;

  oldsz = PGROUNDUP(oldsz);
  ಪ್ರತಿ (a = oldsz; a < newsz; a += PGSIZE) {
    mem = kalloc();
    ಯದಿ (mem == 0) {
      uvmdealloc(pagetable, a, oldsz);
      ಪ್ರತ್ಯಾಗಮ 0;
    }
    memset(mem, 0, PGSIZE);
    ಯದಿ (mappages(pagetable, a, PGSIZE, (uint64)mem, PTE_R | PTE_U | xperm) !=
        0) {
      kfree(mem);
      uvmdealloc(pagetable, a, oldsz);
      ಪ್ರತ್ಯಾಗಮ 0;
    }
  }
  ಪ್ರತ್ಯಾಗಮ newsz;
}

// Deallocate user pages to bring the process size from oldsz to
// newsz.  oldsz and newsz need not be page-aligned, nor does newsz
// need to be less than oldsz.  oldsz can be larger than the actual
// process size.  Returns the new process size.
uint64
uvmdealloc(pagetable_t pagetable, uint64 oldsz, uint64 newsz)
{
  ಯದಿ (newsz >= oldsz)
    ಪ್ರತ್ಯಾಗಮ oldsz;

  ಯದಿ (PGROUNDUP(newsz) < PGROUNDUP(oldsz)) {
    ಪೂರ್ಣಾಙ್ಕ npages = (PGROUNDUP(oldsz) - PGROUNDUP(newsz)) / PGSIZE;
    uvmunmap(pagetable, PGROUNDUP(newsz), npages, 1);
  }

  ಪ್ರತ್ಯಾಗಮ newsz;
}

// Recursively free page-table pages.
// All leaf mappings must already have been removed.
ಶೂನ್ಯ
freewalk(pagetable_t pagetable)
{
  // there are 2^9 = 512 PTEs in a page table.
  ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < 512; i++) {
    pte_t pte = pagetable[i];
    ಯದಿ ((pte & PTE_V) && (pte & (PTE_R | PTE_W | PTE_X)) == 0) {
      // this PTE points to a lower-level page table.
      uint64 child = PTE2PA(pte);
      freewalk((pagetable_t)child);
      pagetable[i] = 0;
    } ಅನ್ಯಥಾ ಯದಿ (pte & PTE_V) {
      panic("freewalk: leaf");
    }
  }
  kfree((ಶೂನ್ಯ *)pagetable);
}

// Free user memory pages,
// then free page-table pages.
ಶೂನ್ಯ
uvmfree(pagetable_t pagetable, uint64 sz)
{
  ಯದಿ (sz > 0)
    uvmunmap(pagetable, 0, PGROUNDUP(sz) / PGSIZE, 1);
  freewalk(pagetable);
}

// Given a parent process's page table, copy
// its memory into a child's page table.
// Copies both the page table and the
// physical memory.
// returns 0 on success, -1 on failure.
// frees any allocated pages on failure.
ಪೂರ್ಣಾಙ್ಕ
uvmcopy(pagetable_t old, pagetable_t new, uint64 sz)
{
  pte_t *pte;
  uint64 pa, i;
  uint flags;
  ವರ್ಣ *mem;

  ಪ್ರತಿ (i = 0; i < sz; i += PGSIZE) {
    ಯದಿ ((pte = walk(old, i, 0)) == 0)
      ಅನುವರ್ತ; // page table entry hasn't been allocated
    ಯದಿ ((*pte & PTE_V) == 0)
      ಅನುವರ್ತ; // physical page hasn't been allocated
    pa = PTE2PA(*pte);
    flags = PTE_FLAGS(*pte);
    ಯದಿ ((mem = kalloc()) == 0)
      ಗಚ್ಛ err;
    memmove(mem, (ವರ್ಣ *)pa, PGSIZE);
    ಯದಿ (mappages(new, i, PGSIZE, (uint64)mem, flags) != 0) {
      kfree(mem);
      ಗಚ್ಛ err;
    }
  }
  ಪ್ರತ್ಯಾಗಮ 0;

err:
  uvmunmap(new, 0, i / PGSIZE, 1);
  ಪ್ರತ್ಯಾಗಮ -1;
}

// mark a PTE invalid for user access.
// used by exec for the user stack guard page.
ಶೂನ್ಯ
uvmclear(pagetable_t pagetable, uint64 va)
{
  pte_t *pte;

  pte = walk(pagetable, va, 0);
  ಯದಿ (pte == 0)
    panic("uvmclear");
  *pte &= ~PTE_U;
}

// Copy from kernel to user.
// Copy len bytes from src to virtual address dstva in a given page table.
// Return 0 on success, -1 on error.
ಪೂರ್ಣಾಙ್ಕ
copyout(pagetable_t pagetable, uint64 psz, uint64 dstva, ವರ್ಣ *src, uint64 len)
{
  uint64 n, va0, pa0;
  pte_t *pte;

  ಯಾವತ್ (len > 0) {
    va0 = PGROUNDDOWN(dstva);
    ಯದಿ (va0 >= MAXVA)
      ಪ್ರತ್ಯಾಗಮ -1;

    pa0 = walkaddr(pagetable, va0);
    ಯದಿ (pa0 == 0) {
      ಯದಿ ((pa0 = vmfault(pagetable, psz, va0, 0)) == 0) {
        ಪ್ರತ್ಯಾಗಮ -1;
      }
    }

    pte = walk(pagetable, va0, 0);
    // forbid copyout over read-only user text pages.
    ಯದಿ ((*pte & PTE_W) == 0)
      ಪ್ರತ್ಯಾಗಮ -1;

    n = PGSIZE - (dstva - va0);
    ಯದಿ (n > len)
      n = len;
    memmove((ಶೂನ್ಯ *)(pa0 + (dstva - va0)), src, n);

    len -= n;
    src += n;
    dstva = va0 + PGSIZE;
  }
  ಪ್ರತ್ಯಾಗಮ 0;
}

// Copy from user to kernel.
// Copy len bytes to dst from virtual address srcva in a given page table.
// Return 0 on success, -1 on error.
ಪೂರ್ಣಾಙ್ಕ
copyin(pagetable_t pagetable, uint64 psz, ವರ್ಣ *dst, uint64 srcva, uint64 len)
{
  uint64 n, va0, pa0;

  ಯಾವತ್ (len > 0) {
    va0 = PGROUNDDOWN(srcva);
    pa0 = walkaddr(pagetable, va0);
    ಯದಿ (pa0 == 0) {
      ಯದಿ ((pa0 = vmfault(pagetable, psz, va0, 1)) == 0) {
        ಪ್ರತ್ಯಾಗಮ -1;
      }
    }
    n = PGSIZE - (srcva - va0);
    ಯದಿ (n > len)
      n = len;
    memmove(dst, (ಶೂನ್ಯ *)(pa0 + (srcva - va0)), n);

    len -= n;
    dst += n;
    srcva = va0 + PGSIZE;
  }
  ಪ್ರತ್ಯಾಗಮ 0;
}

// Copy a null-terminated string from user to kernel.
// Copy bytes to dst from virtual address srcva in a given page table,
// until a '\0', or max.
// Return 0 on success, -1 on error.
ಪೂರ್ಣಾಙ್ಕ
copyinstr(pagetable_t pagetable, uint64 psz, ವರ್ಣ *dst, uint64 srcva,
          uint64 max)
{
  uint64 n, va0, pa0;
  ಪೂರ್ಣಾಙ್ಕ got_null = 0;

  ಯಾವತ್ (got_null == 0 && max > 0) {
    va0 = PGROUNDDOWN(srcva);
    pa0 = walkaddr(pagetable, va0);
    ಯದಿ (pa0 == 0) {
      ಯದಿ ((pa0 = vmfault(pagetable, psz, va0, 1)) == 0) {
        ಪ್ರತ್ಯಾಗಮ -1;
      }
    }
    n = PGSIZE - (srcva - va0);
    ಯದಿ (n > max)
      n = max;

    ವರ್ಣ *p = (ವರ್ಣ *)(pa0 + (srcva - va0));
    ಯಾವತ್ (n > 0) {
      ಯದಿ (*p == '\0') {
        *dst = '\0';
        got_null = 1;
        ಭಙ್ಗ;
      } ಅನ್ಯಥಾ {
        *dst = *p;
      }
      --n;
      --max;
      p++;
      dst++;
    }

    srcva = va0 + PGSIZE;
  }
  ಯದಿ (got_null) {
    ಪ್ರತ್ಯಾಗಮ 0;
  } ಅನ್ಯಥಾ {
    ಪ್ರತ್ಯಾಗಮ -1;
  }
}

// allocate and map user memory if process is referencing a page
// that was lazily allocated in sys_sbrk().
// returns 0 if va is invalid or already mapped, or if
// out of physical memory, and physical address if successful.
uint64
vmfault(pagetable_t pagetable, uint64 psz, uint64 va, ಪೂರ್ಣಾಙ್ಕ ಪಠ)
{
  uint64 mem;

  ಯದಿ (va >= psz)
    ಪ್ರತ್ಯಾಗಮ 0;
  va = PGROUNDDOWN(va);
  ಯದಿ (ismapped(pagetable, va)) {
    ಪ್ರತ್ಯಾಗಮ 0;
  }
  mem = (uint64)kalloc();
  ಯದಿ (mem == 0)
    ಪ್ರತ್ಯಾಗಮ 0;
  memset((ಶೂನ್ಯ *)mem, 0, PGSIZE);
  ಯದಿ (mappages(pagetable, va, PGSIZE, mem, PTE_W | PTE_U | PTE_R) != 0) {
    kfree((ಶೂನ್ಯ *)mem);
    ಪ್ರತ್ಯಾಗಮ 0;
  }
  ಪ್ರತ್ಯಾಗಮ mem;
}

ಪೂರ್ಣಾಙ್ಕ
ismapped(pagetable_t pagetable, uint64 va)
{
  pte_t *pte = walk(pagetable, va, 0);
  ಯದಿ (pte == 0) {
    ಪ್ರತ್ಯಾಗಮ 0;
  }
  ಯದಿ (*pte & PTE_V) {
    ಪ್ರತ್ಯಾಗಮ 1;
  }
  ಪ್ರತ್ಯಾಗಮ 0;
}
