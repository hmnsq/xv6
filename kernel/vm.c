#ଅନ୍ତର୍ଭାଵ "param.h"
#ଅନ୍ତର୍ଭାଵ "types.h"
#ଅନ୍ତର୍ଭାଵ "memlayout.h"
#ଅନ୍ତର୍ଭାଵ "elf.h"
#ଅନ୍ତର୍ଭାଵ "riscv.h"
#ଅନ୍ତର୍ଭାଵ "defs.h"
#ଅନ୍ତର୍ଭାଵ "spinlock.h"
#ଅନ୍ତର୍ଭାଵ "proc.h"
#ଅନ୍ତର୍ଭାଵ "fs.h"

/*
 * the kernel's page table.
 */
pagetable_t kernel_pagetable;

ବାହ୍ୟ ଅକ୍ଷର etext[]; // kernel.ld sets this to end of kernel code.

ବାହ୍ୟ ଅକ୍ଷର trampoline[]; // trampoline.S

// Make a direct-map page table for the kernel.
pagetable_t
kvmmake(ଶୂନ୍ୟ)
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

  ଫେରନ୍ତୁ kpgtbl;
}

// add a mapping to the kernel page table.
// only used when booting.
// does not flush TLB or enable paging.
ଶୂନ୍ୟ
kvmmap(pagetable_t kpgtbl, uint64 va, uint64 pa, uint64 sz, ପୂର୍ଣ୍ଣସଂଖ୍ୟା perm)
{
  ଯଦି (mappages(kpgtbl, va, sz, pa, perm) != 0)
    panic("kvmmap");
}

// Initialize the kernel_pagetable, shared by all CPUs.
ଶୂନ୍ୟ
kvminit(ଶୂନ୍ୟ)
{
  kernel_pagetable = kvmmake();
}

// Switch the current CPU's h/w page table register to
// the kernel's page table, and enable paging.
ଶୂନ୍ୟ
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
walk(pagetable_t pagetable, uint64 va, ପୂର୍ଣ୍ଣସଂଖ୍ୟା alloc)
{
  ଯଦି (va >= MAXVA)
    panic("walk");

  ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା level = 2; level > 0; level--) {
    pte_t *pte = &pagetable[PX(level, va)];
    ଯଦି (*pte & PTE_V) {
      pagetable = (pagetable_t)PTE2PA(*pte);
    } ଅନ୍ୟଥା {
      ଯଦି (!alloc || (pagetable = (pde_t *)kalloc()) == 0)
        ଫେରନ୍ତୁ 0;
      memset(pagetable, 0, PGSIZE);
      *pte = PA2PTE(pagetable) | PTE_V;
    }
  }
  ଫେରନ୍ତୁ &pagetable[PX(0, va)];
}

// Look up a virtual address, return the physical address,
// or 0 if not mapped.
// Can only be used to look up user pages.
uint64
walkaddr(pagetable_t pagetable, uint64 va)
{
  pte_t *pte;
  uint64 pa;

  ଯଦି (va >= MAXVA)
    ଫେରନ୍ତୁ 0;

  pte = walk(pagetable, va, 0);
  ଯଦି (pte == 0)
    ଫେରନ୍ତୁ 0;
  ଯଦି ((*pte & PTE_V) == 0)
    ଫେରନ୍ତୁ 0;
  ଯଦି ((*pte & PTE_U) == 0)
    ଫେରନ୍ତୁ 0;
  pa = PTE2PA(*pte);
  ଫେରନ୍ତୁ pa;
}

// Create PTEs for virtual addresses starting at va that refer to
// physical addresses starting at pa.
// va and size MUST be page-aligned.
// Returns 0 on success, -1 if walk() couldn't
// allocate a needed page-table page.
ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mappages(pagetable_t pagetable, uint64 va, uint64 size, uint64 pa, ପୂର୍ଣ୍ଣସଂଖ୍ୟା perm)
{
  uint64 a, last;
  pte_t *pte;

  ଯଦି ((va % PGSIZE) != 0)
    panic("mappages: va not aligned");

  ଯଦି ((size % PGSIZE) != 0)
    panic("mappages: size not aligned");

  ଯଦି (size == 0)
    panic("mappages: size");

  a = va;
  last = va + size - PGSIZE;
  ପାଇଁ (;;) {
    ଯଦି ((pte = walk(pagetable, a, 1)) == 0)
      ଫେରନ୍ତୁ -1;
    ଯଦି (*pte & PTE_V)
      panic("mappages: remap");
    *pte = PA2PTE(pa) | perm | PTE_V;
    ଯଦି (a == last)
      ଭାଙ୍ଗ;
    a += PGSIZE;
    pa += PGSIZE;
  }
  ଫେରନ୍ତୁ 0;
}

// create an empty user page table.
// returns 0 if out of memory.
pagetable_t
uvmcreate()
{
  pagetable_t pagetable;
  pagetable = (pagetable_t)kalloc();
  ଯଦି (pagetable == 0)
    ଫେରନ୍ତୁ 0;
  memset(pagetable, 0, PGSIZE);
  ଫେରନ୍ତୁ pagetable;
}

// Remove npages of mappings starting from va. va must be
// page-aligned. It's OK if the mappings don't exist.
// Optionally free the physical memory.
ଶୂନ୍ୟ
uvmunmap(pagetable_t pagetable, uint64 va, uint64 npages, ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_free)
{
  uint64 a;
  pte_t *pte;

  ଯଦି ((va % PGSIZE) != 0)
    panic("uvmunmap: not aligned");

  ପାଇଁ (a = va; a < va + npages * PGSIZE; a += PGSIZE) {
    ଯଦି ((pte = walk(pagetable, a, 0)) == 0) // leaf page table entry allocated?
      ଜାରି;
    ଯଦି ((*pte & PTE_V) == 0) // has physical page been allocated?
      ଜାରି;
    ଯଦି (do_free) {
      uint64 pa = PTE2PA(*pte);
      kfree((ଶୂନ୍ୟ *)pa);
    }
    *pte = 0;
  }
}

// Allocate PTEs and physical memory to grow a process from oldsz to
// newsz, which need not be page aligned.  Returns new size or 0 on error.
uint64
uvmalloc(pagetable_t pagetable, uint64 oldsz, uint64 newsz, ପୂର୍ଣ୍ଣସଂଖ୍ୟା xperm)
{
  ଅକ୍ଷର *mem;
  uint64 a;

  ଯଦି (newsz < oldsz)
    ଫେରନ୍ତୁ oldsz;

  oldsz = PGROUNDUP(oldsz);
  ପାଇଁ (a = oldsz; a < newsz; a += PGSIZE) {
    mem = kalloc();
    ଯଦି (mem == 0) {
      uvmdealloc(pagetable, a, oldsz);
      ଫେରନ୍ତୁ 0;
    }
    memset(mem, 0, PGSIZE);
    ଯଦି (mappages(pagetable, a, PGSIZE, (uint64)mem, PTE_R | PTE_U | xperm) !=
        0) {
      kfree(mem);
      uvmdealloc(pagetable, a, oldsz);
      ଫେରନ୍ତୁ 0;
    }
  }
  ଫେରନ୍ତୁ newsz;
}

// Deallocate user pages to bring the process size from oldsz to
// newsz.  oldsz and newsz need not be page-aligned, nor does newsz
// need to be less than oldsz.  oldsz can be larger than the actual
// process size.  Returns the new process size.
uint64
uvmdealloc(pagetable_t pagetable, uint64 oldsz, uint64 newsz)
{
  ଯଦି (newsz >= oldsz)
    ଫେରନ୍ତୁ oldsz;

  ଯଦି (PGROUNDUP(newsz) < PGROUNDUP(oldsz)) {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା npages = (PGROUNDUP(oldsz) - PGROUNDUP(newsz)) / PGSIZE;
    uvmunmap(pagetable, PGROUNDUP(newsz), npages, 1);
  }

  ଫେରନ୍ତୁ newsz;
}

// Recursively free page-table pages.
// All leaf mappings must already have been removed.
ଶୂନ୍ୟ
freewalk(pagetable_t pagetable)
{
  // there are 2^9 = 512 PTEs in a page table.
  ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < 512; i++) {
    pte_t pte = pagetable[i];
    ଯଦି ((pte & PTE_V) && (pte & (PTE_R | PTE_W | PTE_X)) == 0) {
      // this PTE points to a lower-level page table.
      uint64 child = PTE2PA(pte);
      freewalk((pagetable_t)child);
      pagetable[i] = 0;
    } ଅନ୍ୟଥା ଯଦି (pte & PTE_V) {
      panic("freewalk: leaf");
    }
  }
  kfree((ଶୂନ୍ୟ *)pagetable);
}

// Free user memory pages,
// then free page-table pages.
ଶୂନ୍ୟ
uvmfree(pagetable_t pagetable, uint64 sz)
{
  ଯଦି (sz > 0)
    uvmunmap(pagetable, 0, PGROUNDUP(sz) / PGSIZE, 1);
  freewalk(pagetable);
}

// Given a parent process's page table, copy
// its memory into a child's page table.
// Copies both the page table and the
// physical memory.
// returns 0 on success, -1 on failure.
// frees any allocated pages on failure.
ପୂର୍ଣ୍ଣସଂଖ୍ୟା
uvmcopy(pagetable_t old, pagetable_t new, uint64 sz)
{
  pte_t *pte;
  uint64 pa, i;
  uint flags;
  ଅକ୍ଷର *mem;

  ପାଇଁ (i = 0; i < sz; i += PGSIZE) {
    ଯଦି ((pte = walk(old, i, 0)) == 0)
      ଜାରି; // page table entry hasn't been allocated
    ଯଦି ((*pte & PTE_V) == 0)
      ଜାରି; // physical page hasn't been allocated
    pa = PTE2PA(*pte);
    flags = PTE_FLAGS(*pte);
    ଯଦି ((mem = kalloc()) == 0)
      ଯାଅ err;
    memmove(mem, (ଅକ୍ଷର *)pa, PGSIZE);
    ଯଦି (mappages(new, i, PGSIZE, (uint64)mem, flags) != 0) {
      kfree(mem);
      ଯାଅ err;
    }
  }
  ଫେରନ୍ତୁ 0;

err:
  uvmunmap(new, 0, i / PGSIZE, 1);
  ଫେରନ୍ତୁ -1;
}

// mark a PTE invalid for user access.
// used by exec for the user stack guard page.
ଶୂନ୍ୟ
uvmclear(pagetable_t pagetable, uint64 va)
{
  pte_t *pte;

  pte = walk(pagetable, va, 0);
  ଯଦି (pte == 0)
    panic("uvmclear");
  *pte &= ~PTE_U;
}

// Copy from kernel to user.
// Copy len bytes from src to virtual address dstva in a given page table.
// Return 0 on success, -1 on error.
ପୂର୍ଣ୍ଣସଂଖ୍ୟା
copyout(pagetable_t pagetable, uint64 psz, uint64 dstva, ଅକ୍ଷର *src, uint64 len)
{
  uint64 n, va0, pa0;
  pte_t *pte;

  ଯେତେବେଳେ (len > 0) {
    va0 = PGROUNDDOWN(dstva);
    ଯଦି (va0 >= MAXVA)
      ଫେରନ୍ତୁ -1;

    pa0 = walkaddr(pagetable, va0);
    ଯଦି (pa0 == 0) {
      ଯଦି ((pa0 = vmfault(pagetable, psz, va0, 0)) == 0) {
        ଫେରନ୍ତୁ -1;
      }
    }

    pte = walk(pagetable, va0, 0);
    // forbid copyout over read-only user text pages.
    ଯଦି ((*pte & PTE_W) == 0)
      ଫେରନ୍ତୁ -1;

    n = PGSIZE - (dstva - va0);
    ଯଦି (n > len)
      n = len;
    memmove((ଶୂନ୍ୟ *)(pa0 + (dstva - va0)), src, n);

    len -= n;
    src += n;
    dstva = va0 + PGSIZE;
  }
  ଫେରନ୍ତୁ 0;
}

// Copy from user to kernel.
// Copy len bytes to dst from virtual address srcva in a given page table.
// Return 0 on success, -1 on error.
ପୂର୍ଣ୍ଣସଂଖ୍ୟା
copyin(pagetable_t pagetable, uint64 psz, ଅକ୍ଷର *dst, uint64 srcva, uint64 len)
{
  uint64 n, va0, pa0;

  ଯେତେବେଳେ (len > 0) {
    va0 = PGROUNDDOWN(srcva);
    pa0 = walkaddr(pagetable, va0);
    ଯଦି (pa0 == 0) {
      ଯଦି ((pa0 = vmfault(pagetable, psz, va0, 1)) == 0) {
        ଫେରନ୍ତୁ -1;
      }
    }
    n = PGSIZE - (srcva - va0);
    ଯଦି (n > len)
      n = len;
    memmove(dst, (ଶୂନ୍ୟ *)(pa0 + (srcva - va0)), n);

    len -= n;
    dst += n;
    srcva = va0 + PGSIZE;
  }
  ଫେରନ୍ତୁ 0;
}

// Copy a null-terminated string from user to kernel.
// Copy bytes to dst from virtual address srcva in a given page table,
// until a '\0', or max.
// Return 0 on success, -1 on error.
ପୂର୍ଣ୍ଣସଂଖ୍ୟା
copyinstr(pagetable_t pagetable, uint64 psz, ଅକ୍ଷର *dst, uint64 srcva,
          uint64 max)
{
  uint64 n, va0, pa0;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା got_null = 0;

  ଯେତେବେଳେ (got_null == 0 && max > 0) {
    va0 = PGROUNDDOWN(srcva);
    pa0 = walkaddr(pagetable, va0);
    ଯଦି (pa0 == 0) {
      ଯଦି ((pa0 = vmfault(pagetable, psz, va0, 1)) == 0) {
        ଫେରନ୍ତୁ -1;
      }
    }
    n = PGSIZE - (srcva - va0);
    ଯଦି (n > max)
      n = max;

    ଅକ୍ଷର *p = (ଅକ୍ଷର *)(pa0 + (srcva - va0));
    ଯେତେବେଳେ (n > 0) {
      ଯଦି (*p == '\0') {
        *dst = '\0';
        got_null = 1;
        ଭାଙ୍ଗ;
      } ଅନ୍ୟଥା {
        *dst = *p;
      }
      --n;
      --max;
      p++;
      dst++;
    }

    srcva = va0 + PGSIZE;
  }
  ଯଦି (got_null) {
    ଫେରନ୍ତୁ 0;
  } ଅନ୍ୟଥା {
    ଫେରନ୍ତୁ -1;
  }
}

// allocate and map user memory if process is referencing a page
// that was lazily allocated in sys_sbrk().
// returns 0 if va is invalid or already mapped, or if
// out of physical memory, and physical address if successful.
uint64
vmfault(pagetable_t pagetable, uint64 psz, uint64 va, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ପଠ)
{
  uint64 mem;

  ଯଦି (va >= psz)
    ଫେରନ୍ତୁ 0;
  va = PGROUNDDOWN(va);
  ଯଦି (ismapped(pagetable, va)) {
    ଫେରନ୍ତୁ 0;
  }
  mem = (uint64)kalloc();
  ଯଦି (mem == 0)
    ଫେରନ୍ତୁ 0;
  memset((ଶୂନ୍ୟ *)mem, 0, PGSIZE);
  ଯଦି (mappages(pagetable, va, PGSIZE, mem, PTE_W | PTE_U | PTE_R) != 0) {
    kfree((ଶୂନ୍ୟ *)mem);
    ଫେରନ୍ତୁ 0;
  }
  ଫେରନ୍ତୁ mem;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ismapped(pagetable_t pagetable, uint64 va)
{
  pte_t *pte = walk(pagetable, va, 0);
  ଯଦି (pte == 0) {
    ଫେରନ୍ତୁ 0;
  }
  ଯଦି (*pte & PTE_V) {
    ଫେରନ୍ତୁ 1;
  }
  ଫେରନ୍ତୁ 0;
}
