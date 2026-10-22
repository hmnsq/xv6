#অন্তর্ভাৱ "param.h"
#অন্তর্ভাৱ "types.h"
#অন্তর্ভাৱ "memlayout.h"
#অন্তর্ভাৱ "elf.h"
#অন্তর্ভাৱ "riscv.h"
#অন্তর্ভাৱ "defs.h"
#অন্তর্ভাৱ "spinlock.h"
#অন্তর্ভাৱ "proc.h"
#অন্তর্ভাৱ "fs.h"

/*
 * the kernel's page table.
 */
pagetable_t kernel_pagetable;

বাহ্য ৱর্ণ etext[]; // kernel.ld sets this to end of kernel code.

বাহ্য ৱর্ণ trampoline[]; // trampoline.S

// Make a direct-map page table for the kernel.
pagetable_t
kvmmake(শূন্য)
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

  ঘূৰাই kpgtbl;
}

// add a mapping to the kernel page table.
// only used when booting.
// does not flush TLB or enable paging.
শূন্য
kvmmap(pagetable_t kpgtbl, uint64 va, uint64 pa, uint64 sz, পূর্ণাঙ্ক perm)
{
  যদি (mappages(kpgtbl, va, sz, pa, perm) != 0)
    panic("kvmmap");
}

// Initialize the kernel_pagetable, shared by all CPUs.
শূন্য
kvminit(শূন্য)
{
  kernel_pagetable = kvmmake();
}

// Switch the current CPU's h/w page table register to
// the kernel's page table, and enable paging.
শূন্য
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
walk(pagetable_t pagetable, uint64 va, পূর্ণাঙ্ক alloc)
{
  যদি (va >= MAXVA)
    panic("walk");

  বাবে (পূর্ণাঙ্ক level = 2; level > 0; level--) {
    pte_t *pte = &pagetable[PX(level, va)];
    যদি (*pte & PTE_V) {
      pagetable = (pagetable_t)PTE2PA(*pte);
    } অন্যথা {
      যদি (!alloc || (pagetable = (pde_t *)kalloc()) == 0)
        ঘূৰাই 0;
      memset(pagetable, 0, PGSIZE);
      *pte = PA2PTE(pagetable) | PTE_V;
    }
  }
  ঘূৰাই &pagetable[PX(0, va)];
}

// Look up a virtual address, return the physical address,
// or 0 if not mapped.
// Can only be used to look up user pages.
uint64
walkaddr(pagetable_t pagetable, uint64 va)
{
  pte_t *pte;
  uint64 pa;

  যদি (va >= MAXVA)
    ঘূৰাই 0;

  pte = walk(pagetable, va, 0);
  যদি (pte == 0)
    ঘূৰাই 0;
  যদি ((*pte & PTE_V) == 0)
    ঘূৰাই 0;
  যদি ((*pte & PTE_U) == 0)
    ঘূৰাই 0;
  pa = PTE2PA(*pte);
  ঘূৰাই pa;
}

// Create PTEs for virtual addresses starting at va that refer to
// physical addresses starting at pa.
// va and size MUST be page-aligned.
// Returns 0 on success, -1 if walk() couldn't
// allocate a needed page-table page.
পূর্ণাঙ্ক
mappages(pagetable_t pagetable, uint64 va, uint64 size, uint64 pa, পূর্ণাঙ্ক perm)
{
  uint64 a, last;
  pte_t *pte;

  যদি ((va % PGSIZE) != 0)
    panic("mappages: va not aligned");

  যদি ((size % PGSIZE) != 0)
    panic("mappages: size not aligned");

  যদি (size == 0)
    panic("mappages: size");

  a = va;
  last = va + size - PGSIZE;
  বাবে (;;) {
    যদি ((pte = walk(pagetable, a, 1)) == 0)
      ঘূৰাই -1;
    যদি (*pte & PTE_V)
      panic("mappages: remap");
    *pte = PA2PTE(pa) | perm | PTE_V;
    যদি (a == last)
      ভঙ্গ;
    a += PGSIZE;
    pa += PGSIZE;
  }
  ঘূৰাই 0;
}

// create an empty user page table.
// returns 0 if out of memory.
pagetable_t
uvmcreate()
{
  pagetable_t pagetable;
  pagetable = (pagetable_t)kalloc();
  যদি (pagetable == 0)
    ঘূৰাই 0;
  memset(pagetable, 0, PGSIZE);
  ঘূৰাই pagetable;
}

// Remove npages of mappings starting from va. va must be
// page-aligned. It's OK if the mappings don't exist.
// Optionally free the physical memory.
শূন্য
uvmunmap(pagetable_t pagetable, uint64 va, uint64 npages, পূর্ণাঙ্ক do_free)
{
  uint64 a;
  pte_t *pte;

  যদি ((va % PGSIZE) != 0)
    panic("uvmunmap: not aligned");

  বাবে (a = va; a < va + npages * PGSIZE; a += PGSIZE) {
    যদি ((pte = walk(pagetable, a, 0)) == 0) // leaf page table entry allocated?
      অনুৱর্ত;
    যদি ((*pte & PTE_V) == 0) // has physical page been allocated?
      অনুৱর্ত;
    যদি (do_free) {
      uint64 pa = PTE2PA(*pte);
      kfree((শূন্য *)pa);
    }
    *pte = 0;
  }
}

// Allocate PTEs and physical memory to grow a process from oldsz to
// newsz, which need not be page aligned.  Returns new size or 0 on error.
uint64
uvmalloc(pagetable_t pagetable, uint64 oldsz, uint64 newsz, পূর্ণাঙ্ক xperm)
{
  ৱর্ণ *mem;
  uint64 a;

  যদি (newsz < oldsz)
    ঘূৰাই oldsz;

  oldsz = PGROUNDUP(oldsz);
  বাবে (a = oldsz; a < newsz; a += PGSIZE) {
    mem = kalloc();
    যদি (mem == 0) {
      uvmdealloc(pagetable, a, oldsz);
      ঘূৰাই 0;
    }
    memset(mem, 0, PGSIZE);
    যদি (mappages(pagetable, a, PGSIZE, (uint64)mem, PTE_R | PTE_U | xperm) !=
        0) {
      kfree(mem);
      uvmdealloc(pagetable, a, oldsz);
      ঘূৰাই 0;
    }
  }
  ঘূৰাই newsz;
}

// Deallocate user pages to bring the process size from oldsz to
// newsz.  oldsz and newsz need not be page-aligned, nor does newsz
// need to be less than oldsz.  oldsz can be larger than the actual
// process size.  Returns the new process size.
uint64
uvmdealloc(pagetable_t pagetable, uint64 oldsz, uint64 newsz)
{
  যদি (newsz >= oldsz)
    ঘূৰাই oldsz;

  যদি (PGROUNDUP(newsz) < PGROUNDUP(oldsz)) {
    পূর্ণাঙ্ক npages = (PGROUNDUP(oldsz) - PGROUNDUP(newsz)) / PGSIZE;
    uvmunmap(pagetable, PGROUNDUP(newsz), npages, 1);
  }

  ঘূৰাই newsz;
}

// Recursively free page-table pages.
// All leaf mappings must already have been removed.
শূন্য
freewalk(pagetable_t pagetable)
{
  // there are 2^9 = 512 PTEs in a page table.
  বাবে (পূর্ণাঙ্ক i = 0; i < 512; i++) {
    pte_t pte = pagetable[i];
    যদি ((pte & PTE_V) && (pte & (PTE_R | PTE_W | PTE_X)) == 0) {
      // this PTE points to a lower-level page table.
      uint64 child = PTE2PA(pte);
      freewalk((pagetable_t)child);
      pagetable[i] = 0;
    } অন্যথা যদি (pte & PTE_V) {
      panic("freewalk: leaf");
    }
  }
  kfree((শূন্য *)pagetable);
}

// Free user memory pages,
// then free page-table pages.
শূন্য
uvmfree(pagetable_t pagetable, uint64 sz)
{
  যদি (sz > 0)
    uvmunmap(pagetable, 0, PGROUNDUP(sz) / PGSIZE, 1);
  freewalk(pagetable);
}

// Given a parent process's page table, copy
// its memory into a child's page table.
// Copies both the page table and the
// physical memory.
// returns 0 on success, -1 on failure.
// frees any allocated pages on failure.
পূর্ণাঙ্ক
uvmcopy(pagetable_t old, pagetable_t new, uint64 sz)
{
  pte_t *pte;
  uint64 pa, i;
  uint flags;
  ৱর্ণ *mem;

  বাবে (i = 0; i < sz; i += PGSIZE) {
    যদি ((pte = walk(old, i, 0)) == 0)
      অনুৱর্ত; // page table entry hasn't been allocated
    যদি ((*pte & PTE_V) == 0)
      অনুৱর্ত; // physical page hasn't been allocated
    pa = PTE2PA(*pte);
    flags = PTE_FLAGS(*pte);
    যদি ((mem = kalloc()) == 0)
      গচ্ছ err;
    memmove(mem, (ৱর্ণ *)pa, PGSIZE);
    যদি (mappages(new, i, PGSIZE, (uint64)mem, flags) != 0) {
      kfree(mem);
      গচ্ছ err;
    }
  }
  ঘূৰাই 0;

err:
  uvmunmap(new, 0, i / PGSIZE, 1);
  ঘূৰাই -1;
}

// mark a PTE invalid for user access.
// used by exec for the user stack guard page.
শূন্য
uvmclear(pagetable_t pagetable, uint64 va)
{
  pte_t *pte;

  pte = walk(pagetable, va, 0);
  যদি (pte == 0)
    panic("uvmclear");
  *pte &= ~PTE_U;
}

// Copy from kernel to user.
// Copy len bytes from src to virtual address dstva in a given page table.
// Return 0 on success, -1 on error.
পূর্ণাঙ্ক
copyout(pagetable_t pagetable, uint64 psz, uint64 dstva, ৱর্ণ *src, uint64 len)
{
  uint64 n, va0, pa0;
  pte_t *pte;

  যেতিয়ালৈ (len > 0) {
    va0 = PGROUNDDOWN(dstva);
    যদি (va0 >= MAXVA)
      ঘূৰাই -1;

    pa0 = walkaddr(pagetable, va0);
    যদি (pa0 == 0) {
      যদি ((pa0 = vmfault(pagetable, psz, va0, 0)) == 0) {
        ঘূৰাই -1;
      }
    }

    pte = walk(pagetable, va0, 0);
    // forbid copyout over read-only user text pages.
    যদি ((*pte & PTE_W) == 0)
      ঘূৰাই -1;

    n = PGSIZE - (dstva - va0);
    যদি (n > len)
      n = len;
    memmove((শূন্য *)(pa0 + (dstva - va0)), src, n);

    len -= n;
    src += n;
    dstva = va0 + PGSIZE;
  }
  ঘূৰাই 0;
}

// Copy from user to kernel.
// Copy len bytes to dst from virtual address srcva in a given page table.
// Return 0 on success, -1 on error.
পূর্ণাঙ্ক
copyin(pagetable_t pagetable, uint64 psz, ৱর্ণ *dst, uint64 srcva, uint64 len)
{
  uint64 n, va0, pa0;

  যেতিয়ালৈ (len > 0) {
    va0 = PGROUNDDOWN(srcva);
    pa0 = walkaddr(pagetable, va0);
    যদি (pa0 == 0) {
      যদি ((pa0 = vmfault(pagetable, psz, va0, 1)) == 0) {
        ঘূৰাই -1;
      }
    }
    n = PGSIZE - (srcva - va0);
    যদি (n > len)
      n = len;
    memmove(dst, (শূন্য *)(pa0 + (srcva - va0)), n);

    len -= n;
    dst += n;
    srcva = va0 + PGSIZE;
  }
  ঘূৰাই 0;
}

// Copy a null-terminated string from user to kernel.
// Copy bytes to dst from virtual address srcva in a given page table,
// until a '\0', or max.
// Return 0 on success, -1 on error.
পূর্ণাঙ্ক
copyinstr(pagetable_t pagetable, uint64 psz, ৱর্ণ *dst, uint64 srcva,
          uint64 max)
{
  uint64 n, va0, pa0;
  পূর্ণাঙ্ক got_null = 0;

  যেতিয়ালৈ (got_null == 0 && max > 0) {
    va0 = PGROUNDDOWN(srcva);
    pa0 = walkaddr(pagetable, va0);
    যদি (pa0 == 0) {
      যদি ((pa0 = vmfault(pagetable, psz, va0, 1)) == 0) {
        ঘূৰাই -1;
      }
    }
    n = PGSIZE - (srcva - va0);
    যদি (n > max)
      n = max;

    ৱর্ণ *p = (ৱর্ণ *)(pa0 + (srcva - va0));
    যেতিয়ালৈ (n > 0) {
      যদি (*p == '\0') {
        *dst = '\0';
        got_null = 1;
        ভঙ্গ;
      } অন্যথা {
        *dst = *p;
      }
      --n;
      --max;
      p++;
      dst++;
    }

    srcva = va0 + PGSIZE;
  }
  যদি (got_null) {
    ঘূৰাই 0;
  } অন্যথা {
    ঘূৰাই -1;
  }
}

// allocate and map user memory if process is referencing a page
// that was lazily allocated in sys_sbrk().
// returns 0 if va is invalid or already mapped, or if
// out of physical memory, and physical address if successful.
uint64
vmfault(pagetable_t pagetable, uint64 psz, uint64 va, পূর্ণাঙ্ক পঠ)
{
  uint64 mem;

  যদি (va >= psz)
    ঘূৰাই 0;
  va = PGROUNDDOWN(va);
  যদি (ismapped(pagetable, va)) {
    ঘূৰাই 0;
  }
  mem = (uint64)kalloc();
  যদি (mem == 0)
    ঘূৰাই 0;
  memset((শূন্য *)mem, 0, PGSIZE);
  যদি (mappages(pagetable, va, PGSIZE, mem, PTE_W | PTE_U | PTE_R) != 0) {
    kfree((শূন্য *)mem);
    ঘূৰাই 0;
  }
  ঘূৰাই mem;
}

পূর্ণাঙ্ক
ismapped(pagetable_t pagetable, uint64 va)
{
  pte_t *pte = walk(pagetable, va, 0);
  যদি (pte == 0) {
    ঘূৰাই 0;
  }
  যদি (*pte & PTE_V) {
    ঘূৰাই 1;
  }
  ঘূৰাই 0;
}
