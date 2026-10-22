#ਅਨ੍ਤਰ੍ਭਾਵ "param.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "types.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "memlayout.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "elf.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "riscv.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "defs.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "spinlock.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "proc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "fs.h"

/*
 * the kernel's page table.
 */
pagetable_t kernel_pagetable;

ਬਾਹਰੀ ਅੱਖਰ etext[]; // kernel.ld sets this to end of kernel code.

ਬਾਹਰੀ ਅੱਖਰ trampoline[]; // trampoline.S

// Make a direct-map page table for the kernel.
pagetable_t
kvmmake(ਖਾਲੀ)
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

  ਵਾਪਸ kpgtbl;
}

// add a mapping to the kernel page table.
// only used when booting.
// does not flush TLB or enable paging.
ਖਾਲੀ
kvmmap(pagetable_t kpgtbl, uint64 va, uint64 pa, uint64 sz, ਪੂਰਨ perm)
{
  ਜੇ (mappages(kpgtbl, va, sz, pa, perm) != 0)
    panic("kvmmap");
}

// Initialize the kernel_pagetable, shared by all CPUs.
ਖਾਲੀ
kvminit(ਖਾਲੀ)
{
  kernel_pagetable = kvmmake();
}

// Switch the current CPU's h/w page table register to
// the kernel's page table, and enable paging.
ਖਾਲੀ
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
walk(pagetable_t pagetable, uint64 va, ਪੂਰਨ alloc)
{
  ਜੇ (va >= MAXVA)
    panic("walk");

  ਲਈ (ਪੂਰਨ level = 2; level > 0; level--) {
    pte_t *pte = &pagetable[PX(level, va)];
    ਜੇ (*pte & PTE_V) {
      pagetable = (pagetable_t)PTE2PA(*pte);
    } ਨਹੀਂਤਾਂ {
      ਜੇ (!alloc || (pagetable = (pde_t *)kalloc()) == 0)
        ਵਾਪਸ 0;
      memset(pagetable, 0, PGSIZE);
      *pte = PA2PTE(pagetable) | PTE_V;
    }
  }
  ਵਾਪਸ &pagetable[PX(0, va)];
}

// Look up a virtual address, return the physical address,
// or 0 if not mapped.
// Can only be used to look up user pages.
uint64
walkaddr(pagetable_t pagetable, uint64 va)
{
  pte_t *pte;
  uint64 pa;

  ਜੇ (va >= MAXVA)
    ਵਾਪਸ 0;

  pte = walk(pagetable, va, 0);
  ਜੇ (pte == 0)
    ਵਾਪਸ 0;
  ਜੇ ((*pte & PTE_V) == 0)
    ਵਾਪਸ 0;
  ਜੇ ((*pte & PTE_U) == 0)
    ਵਾਪਸ 0;
  pa = PTE2PA(*pte);
  ਵਾਪਸ pa;
}

// Create PTEs for virtual addresses starting at va that refer to
// physical addresses starting at pa.
// va and size MUST be page-aligned.
// Returns 0 on success, -1 if walk() couldn't
// allocate a needed page-table page.
ਪੂਰਨ
mappages(pagetable_t pagetable, uint64 va, uint64 ਮਾਪ, uint64 pa, ਪੂਰਨ perm)
{
  uint64 a, ਆਖਰੀ;
  pte_t *pte;

  ਜੇ ((va % PGSIZE) != 0)
    panic("mappages: va not aligned");

  ਜੇ ((ਮਾਪ % PGSIZE) != 0)
    panic("mappages: size not aligned");

  ਜੇ (ਮਾਪ == 0)
    panic("mappages: size");

  a = va;
  ਆਖਰੀ = va + ਮਾਪ - PGSIZE;
  ਲਈ (;;) {
    ਜੇ ((pte = walk(pagetable, a, 1)) == 0)
      ਵਾਪਸ -1;
    ਜੇ (*pte & PTE_V)
      panic("mappages: remap");
    *pte = PA2PTE(pa) | perm | PTE_V;
    ਜੇ (a == ਆਖਰੀ)
      ਰੋਕੋ;
    a += PGSIZE;
    pa += PGSIZE;
  }
  ਵਾਪਸ 0;
}

// create an empty user page table.
// returns 0 if out of memory.
pagetable_t
uvmcreate()
{
  pagetable_t pagetable;
  pagetable = (pagetable_t)kalloc();
  ਜੇ (pagetable == 0)
    ਵਾਪਸ 0;
  memset(pagetable, 0, PGSIZE);
  ਵਾਪਸ pagetable;
}

// Remove npages of mappings starting from va. va must be
// page-aligned. It's OK if the mappings don't exist.
// Optionally free the physical memory.
ਖਾਲੀ
uvmunmap(pagetable_t pagetable, uint64 va, uint64 npages, ਪੂਰਨ do_free)
{
  uint64 a;
  pte_t *pte;

  ਜੇ ((va % PGSIZE) != 0)
    panic("uvmunmap: not aligned");

  ਲਈ (a = va; a < va + npages * PGSIZE; a += PGSIZE) {
    ਜੇ ((pte = walk(pagetable, a, 0)) == 0) // leaf page table entry allocated?
      ਜਾਰੀ;
    ਜੇ ((*pte & PTE_V) == 0) // has physical page been allocated?
      ਜਾਰੀ;
    ਜੇ (do_free) {
      uint64 pa = PTE2PA(*pte);
      kfree((ਖਾਲੀ *)pa);
    }
    *pte = 0;
  }
}

// Allocate PTEs and physical memory to grow a process from oldsz to
// newsz, which need not be page aligned.  Returns new size or 0 on error.
uint64
uvmalloc(pagetable_t pagetable, uint64 oldsz, uint64 newsz, ਪੂਰਨ xperm)
{
  ਅੱਖਰ *mem;
  uint64 a;

  ਜੇ (newsz < oldsz)
    ਵਾਪਸ oldsz;

  oldsz = PGROUNDUP(oldsz);
  ਲਈ (a = oldsz; a < newsz; a += PGSIZE) {
    mem = kalloc();
    ਜੇ (mem == 0) {
      uvmdealloc(pagetable, a, oldsz);
      ਵਾਪਸ 0;
    }
    memset(mem, 0, PGSIZE);
    ਜੇ (mappages(pagetable, a, PGSIZE, (uint64)mem, PTE_R | PTE_U | xperm) !=
        0) {
      kfree(mem);
      uvmdealloc(pagetable, a, oldsz);
      ਵਾਪਸ 0;
    }
  }
  ਵਾਪਸ newsz;
}

// Deallocate user pages to bring the process size from oldsz to
// newsz.  oldsz and newsz need not be page-aligned, nor does newsz
// need to be less than oldsz.  oldsz can be larger than the actual
// process size.  Returns the new process size.
uint64
uvmdealloc(pagetable_t pagetable, uint64 oldsz, uint64 newsz)
{
  ਜੇ (newsz >= oldsz)
    ਵਾਪਸ oldsz;

  ਜੇ (PGROUNDUP(newsz) < PGROUNDUP(oldsz)) {
    ਪੂਰਨ npages = (PGROUNDUP(oldsz) - PGROUNDUP(newsz)) / PGSIZE;
    uvmunmap(pagetable, PGROUNDUP(newsz), npages, 1);
  }

  ਵਾਪਸ newsz;
}

// Recursively free page-table pages.
// All leaf mappings must already have been removed.
ਖਾਲੀ
freewalk(pagetable_t pagetable)
{
  // there are 2^9 = 512 PTEs in a page table.
  ਲਈ (ਪੂਰਨ i = 0; i < 512; i++) {
    pte_t pte = pagetable[i];
    ਜੇ ((pte & PTE_V) && (pte & (PTE_R | PTE_W | PTE_X)) == 0) {
      // this PTE points to a lower-level page table.
      uint64 ਬੱਚਾ = PTE2PA(pte);
      freewalk((pagetable_t)ਬੱਚਾ);
      pagetable[i] = 0;
    } ਨਹੀਂਤਾਂ ਜੇ (pte & PTE_V) {
      panic("freewalk: leaf");
    }
  }
  kfree((ਖਾਲੀ *)pagetable);
}

// Free user memory pages,
// then free page-table pages.
ਖਾਲੀ
uvmfree(pagetable_t pagetable, uint64 sz)
{
  ਜੇ (sz > 0)
    uvmunmap(pagetable, 0, PGROUNDUP(sz) / PGSIZE, 1);
  freewalk(pagetable);
}

// Given a parent process's page table, copy
// its memory into a child's page table.
// Copies both the page table and the
// physical memory.
// returns 0 on success, -1 on failure.
// frees any allocated pages on failure.
ਪੂਰਨ
uvmcopy(pagetable_t old, pagetable_t new, uint64 sz)
{
  pte_t *pte;
  uint64 pa, i;
  uint flags;
  ਅੱਖਰ *mem;

  ਲਈ (i = 0; i < sz; i += PGSIZE) {
    ਜੇ ((pte = walk(old, i, 0)) == 0)
      ਜਾਰੀ; // page table entry hasn't been allocated
    ਜੇ ((*pte & PTE_V) == 0)
      ਜਾਰੀ; // physical page hasn't been allocated
    pa = PTE2PA(*pte);
    flags = PTE_FLAGS(*pte);
    ਜੇ ((mem = kalloc()) == 0)
      ਜਾਓ err;
    memmove(mem, (ਅੱਖਰ *)pa, PGSIZE);
    ਜੇ (mappages(new, i, PGSIZE, (uint64)mem, flags) != 0) {
      kfree(mem);
      ਜਾਓ err;
    }
  }
  ਵਾਪਸ 0;

err:
  uvmunmap(new, 0, i / PGSIZE, 1);
  ਵਾਪਸ -1;
}

// mark a PTE invalid for user access.
// used by exec for the user stack guard page.
ਖਾਲੀ
uvmclear(pagetable_t pagetable, uint64 va)
{
  pte_t *pte;

  pte = walk(pagetable, va, 0);
  ਜੇ (pte == 0)
    panic("uvmclear");
  *pte &= ~PTE_U;
}

// Copy from kernel to user.
// Copy len bytes from src to virtual address dstva in a given page table.
// Return 0 on success, -1 on error.
ਪੂਰਨ
copyout(pagetable_t pagetable, uint64 psz, uint64 dstva, ਅੱਖਰ *src, uint64 len)
{
  uint64 n, va0, pa0;
  pte_t *pte;

  ਜਦੋਂ (len > 0) {
    va0 = PGROUNDDOWN(dstva);
    ਜੇ (va0 >= MAXVA)
      ਵਾਪਸ -1;

    pa0 = walkaddr(pagetable, va0);
    ਜੇ (pa0 == 0) {
      ਜੇ ((pa0 = vmfault(pagetable, psz, va0, 0)) == 0) {
        ਵਾਪਸ -1;
      }
    }

    pte = walk(pagetable, va0, 0);
    // forbid copyout over read-only user text pages.
    ਜੇ ((*pte & PTE_W) == 0)
      ਵਾਪਸ -1;

    n = PGSIZE - (dstva - va0);
    ਜੇ (n > len)
      n = len;
    memmove((ਖਾਲੀ *)(pa0 + (dstva - va0)), src, n);

    len -= n;
    src += n;
    dstva = va0 + PGSIZE;
  }
  ਵਾਪਸ 0;
}

// Copy from user to kernel.
// Copy len bytes to dst from virtual address srcva in a given page table.
// Return 0 on success, -1 on error.
ਪੂਰਨ
copyin(pagetable_t pagetable, uint64 psz, ਅੱਖਰ *dst, uint64 srcva, uint64 len)
{
  uint64 n, va0, pa0;

  ਜਦੋਂ (len > 0) {
    va0 = PGROUNDDOWN(srcva);
    pa0 = walkaddr(pagetable, va0);
    ਜੇ (pa0 == 0) {
      ਜੇ ((pa0 = vmfault(pagetable, psz, va0, 1)) == 0) {
        ਵਾਪਸ -1;
      }
    }
    n = PGSIZE - (srcva - va0);
    ਜੇ (n > len)
      n = len;
    memmove(dst, (ਖਾਲੀ *)(pa0 + (srcva - va0)), n);

    len -= n;
    dst += n;
    srcva = va0 + PGSIZE;
  }
  ਵਾਪਸ 0;
}

// Copy a null-terminated string from user to kernel.
// Copy bytes to dst from virtual address srcva in a given page table,
// until a '\0', or max.
// Return 0 on success, -1 on error.
ਪੂਰਨ
copyinstr(pagetable_t pagetable, uint64 psz, ਅੱਖਰ *dst, uint64 srcva,
          uint64 max)
{
  uint64 n, va0, pa0;
  ਪੂਰਨ got_null = 0;

  ਜਦੋਂ (got_null == 0 && max > 0) {
    va0 = PGROUNDDOWN(srcva);
    pa0 = walkaddr(pagetable, va0);
    ਜੇ (pa0 == 0) {
      ਜੇ ((pa0 = vmfault(pagetable, psz, va0, 1)) == 0) {
        ਵਾਪਸ -1;
      }
    }
    n = PGSIZE - (srcva - va0);
    ਜੇ (n > max)
      n = max;

    ਅੱਖਰ *p = (ਅੱਖਰ *)(pa0 + (srcva - va0));
    ਜਦੋਂ (n > 0) {
      ਜੇ (*p == '\0') {
        *dst = '\0';
        got_null = 1;
        ਰੋਕੋ;
      } ਨਹੀਂਤਾਂ {
        *dst = *p;
      }
      --n;
      --max;
      p++;
      dst++;
    }

    srcva = va0 + PGSIZE;
  }
  ਜੇ (got_null) {
    ਵਾਪਸ 0;
  } ਨਹੀਂਤਾਂ {
    ਵਾਪਸ -1;
  }
}

// allocate and map user memory if process is referencing a page
// that was lazily allocated in sys_sbrk().
// returns 0 if va is invalid or already mapped, or if
// out of physical memory, and physical address if successful.
uint64
vmfault(pagetable_t pagetable, uint64 psz, uint64 va, ਪੂਰਨ ਪਠ)
{
  uint64 mem;

  ਜੇ (va >= psz)
    ਵਾਪਸ 0;
  va = PGROUNDDOWN(va);
  ਜੇ (ismapped(pagetable, va)) {
    ਵਾਪਸ 0;
  }
  mem = (uint64)kalloc();
  ਜੇ (mem == 0)
    ਵਾਪਸ 0;
  memset((ਖਾਲੀ *)mem, 0, PGSIZE);
  ਜੇ (mappages(pagetable, va, PGSIZE, mem, PTE_W | PTE_U | PTE_R) != 0) {
    kfree((ਖਾਲੀ *)mem);
    ਵਾਪਸ 0;
  }
  ਵਾਪਸ mem;
}

ਪੂਰਨ
ismapped(pagetable_t pagetable, uint64 va)
{
  pte_t *pte = walk(pagetable, va, 0);
  ਜੇ (pte == 0) {
    ਵਾਪਸ 0;
  }
  ਜੇ (*pte & PTE_V) {
    ਵਾਪਸ 1;
  }
  ਵਾਪਸ 0;
}
