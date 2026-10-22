#ضمن "param.h"
#ضمن "types.h"
#ضمن "memlayout.h"
#ضمن "elf.h"
#ضمن "riscv.h"
#ضمن "defs.h"
#ضمن "spinlock.h"
#ضمن "proc.h"
#ضمن "fs.h"

/*
 * the kernel's page table.
 */
pagetable_t kernel_pagetable;

خارجي حرف etext[]; // kernel.ld sets this to end of kernel code.

خارجي حرف trampoline[]; // trampoline.S

// Make a direct-map page table for the kernel.
pagetable_t
kvmmake(فراغ)
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

  أرجع kpgtbl;
}

// add a mapping to the kernel page table.
// only used when booting.
// does not flush TLB or enable paging.
فراغ
kvmmap(pagetable_t kpgtbl, uint64 va, uint64 pa, uint64 sz, صحيح perm)
{
  إذا (mappages(kpgtbl, va, sz, pa, perm) != 0)
    panic("kvmmap");
}

// Initialize the kernel_pagetable, shared by all CPUs.
فراغ
kvminit(فراغ)
{
  kernel_pagetable = kvmmake();
}

// Switch the current CPU's h/w page table register to
// the kernel's page table, and enable paging.
فراغ
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
walk(pagetable_t pagetable, uint64 va, صحيح alloc)
{
  إذا (va >= MAXVA)
    panic("walk");

  لكل (صحيح level = 2; level > 0; level--) {
    pte_t *pte = &pagetable[PX(level, va)];
    إذا (*pte & PTE_V) {
      pagetable = (pagetable_t)PTE2PA(*pte);
    } وإلا {
      إذا (!alloc || (pagetable = (pde_t *)kalloc()) == 0)
        أرجع 0;
      memset(pagetable, 0, PGSIZE);
      *pte = PA2PTE(pagetable) | PTE_V;
    }
  }
  أرجع &pagetable[PX(0, va)];
}

// Look up a virtual address, return the physical address,
// or 0 if not mapped.
// Can only be used to look up user pages.
uint64
walkaddr(pagetable_t pagetable, uint64 va)
{
  pte_t *pte;
  uint64 pa;

  إذا (va >= MAXVA)
    أرجع 0;

  pte = walk(pagetable, va, 0);
  إذا (pte == 0)
    أرجع 0;
  إذا ((*pte & PTE_V) == 0)
    أرجع 0;
  إذا ((*pte & PTE_U) == 0)
    أرجع 0;
  pa = PTE2PA(*pte);
  أرجع pa;
}

// Create PTEs for virtual addresses starting at va that refer to
// physical addresses starting at pa.
// va and size MUST be page-aligned.
// Returns 0 on success, -1 if walk() couldn't
// allocate a needed page-table page.
صحيح
mappages(pagetable_t pagetable, uint64 va, uint64 سائيز, uint64 pa, صحيح perm)
{
  uint64 a, آخري;
  pte_t *pte;

  إذا ((va % PGSIZE) != 0)
    panic("mappages: va not aligned");

  إذا ((سائيز % PGSIZE) != 0)
    panic("mappages: size not aligned");

  إذا (سائيز == 0)
    panic("mappages: size");

  a = va;
  آخري = va + سائيز - PGSIZE;
  لكل (;;) {
    إذا ((pte = walk(pagetable, a, 1)) == 0)
      أرجع -1;
    إذا (*pte & PTE_V)
      panic("mappages: remap");
    *pte = PA2PTE(pa) | perm | PTE_V;
    إذا (a == آخري)
      اكسر;
    a += PGSIZE;
    pa += PGSIZE;
  }
  أرجع 0;
}

// create an empty user page table.
// returns 0 if out of memory.
pagetable_t
uvmcreate()
{
  pagetable_t pagetable;
  pagetable = (pagetable_t)kalloc();
  إذا (pagetable == 0)
    أرجع 0;
  memset(pagetable, 0, PGSIZE);
  أرجع pagetable;
}

// Remove npages of mappings starting from va. va must be
// page-aligned. It's OK if the mappings don't exist.
// Optionally free the physical memory.
فراغ
uvmunmap(pagetable_t pagetable, uint64 va, uint64 npages, صحيح do_free)
{
  uint64 a;
  pte_t *pte;

  إذا ((va % PGSIZE) != 0)
    panic("uvmunmap: not aligned");

  لكل (a = va; a < va + npages * PGSIZE; a += PGSIZE) {
    إذا ((pte = walk(pagetable, a, 0)) == 0) // leaf page table entry allocated?
      استمر;
    إذا ((*pte & PTE_V) == 0) // has physical page been allocated?
      استمر;
    إذا (do_free) {
      uint64 pa = PTE2PA(*pte);
      kfree((فراغ *)pa);
    }
    *pte = 0;
  }
}

// Allocate PTEs and physical memory to grow a process from oldsz to
// newsz, which need not be page aligned.  Returns new size or 0 on error.
uint64
uvmalloc(pagetable_t pagetable, uint64 oldsz, uint64 newsz, صحيح xperm)
{
  حرف *mem;
  uint64 a;

  إذا (newsz < oldsz)
    أرجع oldsz;

  oldsz = PGROUNDUP(oldsz);
  لكل (a = oldsz; a < newsz; a += PGSIZE) {
    mem = kalloc();
    إذا (mem == 0) {
      uvmdealloc(pagetable, a, oldsz);
      أرجع 0;
    }
    memset(mem, 0, PGSIZE);
    إذا (mappages(pagetable, a, PGSIZE, (uint64)mem, PTE_R | PTE_U | xperm) !=
        0) {
      kfree(mem);
      uvmdealloc(pagetable, a, oldsz);
      أرجع 0;
    }
  }
  أرجع newsz;
}

// Deallocate user pages to bring the process size from oldsz to
// newsz.  oldsz and newsz need not be page-aligned, nor does newsz
// need to be less than oldsz.  oldsz can be larger than the actual
// process size.  Returns the new process size.
uint64
uvmdealloc(pagetable_t pagetable, uint64 oldsz, uint64 newsz)
{
  إذا (newsz >= oldsz)
    أرجع oldsz;

  إذا (PGROUNDUP(newsz) < PGROUNDUP(oldsz)) {
    صحيح npages = (PGROUNDUP(oldsz) - PGROUNDUP(newsz)) / PGSIZE;
    uvmunmap(pagetable, PGROUNDUP(newsz), npages, 1);
  }

  أرجع newsz;
}

// Recursively free page-table pages.
// All leaf mappings must already have been removed.
فراغ
freewalk(pagetable_t pagetable)
{
  // there are 2^9 = 512 PTEs in a page table.
  لكل (صحيح i = 0; i < 512; i++) {
    pte_t pte = pagetable[i];
    إذا ((pte & PTE_V) && (pte & (PTE_R | PTE_W | PTE_X)) == 0) {
      // this PTE points to a lower-level page table.
      uint64 ٻار = PTE2PA(pte);
      freewalk((pagetable_t)ٻار);
      pagetable[i] = 0;
    } وإلا إذا (pte & PTE_V) {
      panic("freewalk: leaf");
    }
  }
  kfree((فراغ *)pagetable);
}

// Free user memory pages,
// then free page-table pages.
فراغ
uvmfree(pagetable_t pagetable, uint64 sz)
{
  إذا (sz > 0)
    uvmunmap(pagetable, 0, PGROUNDUP(sz) / PGSIZE, 1);
  freewalk(pagetable);
}

// Given a parent process's page table, copy
// its memory into a child's page table.
// Copies both the page table and the
// physical memory.
// returns 0 on success, -1 on failure.
// frees any allocated pages on failure.
صحيح
uvmcopy(pagetable_t old, pagetable_t new, uint64 sz)
{
  pte_t *pte;
  uint64 pa, i;
  uint flags;
  حرف *mem;

  لكل (i = 0; i < sz; i += PGSIZE) {
    إذا ((pte = walk(old, i, 0)) == 0)
      استمر; // page table entry hasn't been allocated
    إذا ((*pte & PTE_V) == 0)
      استمر; // physical page hasn't been allocated
    pa = PTE2PA(*pte);
    flags = PTE_FLAGS(*pte);
    إذا ((mem = kalloc()) == 0)
      اذهب err;
    memmove(mem, (حرف *)pa, PGSIZE);
    إذا (mappages(new, i, PGSIZE, (uint64)mem, flags) != 0) {
      kfree(mem);
      اذهب err;
    }
  }
  أرجع 0;

err:
  uvmunmap(new, 0, i / PGSIZE, 1);
  أرجع -1;
}

// mark a PTE invalid for user access.
// used by exec for the user stack guard page.
فراغ
uvmclear(pagetable_t pagetable, uint64 va)
{
  pte_t *pte;

  pte = walk(pagetable, va, 0);
  إذا (pte == 0)
    panic("uvmclear");
  *pte &= ~PTE_U;
}

// Copy from kernel to user.
// Copy len bytes from src to virtual address dstva in a given page table.
// Return 0 on success, -1 on error.
صحيح
copyout(pagetable_t pagetable, uint64 psz, uint64 dstva, حرف *src, uint64 len)
{
  uint64 n, va0, pa0;
  pte_t *pte;

  طالما (len > 0) {
    va0 = PGROUNDDOWN(dstva);
    إذا (va0 >= MAXVA)
      أرجع -1;

    pa0 = walkaddr(pagetable, va0);
    إذا (pa0 == 0) {
      إذا ((pa0 = vmfault(pagetable, psz, va0, 0)) == 0) {
        أرجع -1;
      }
    }

    pte = walk(pagetable, va0, 0);
    // forbid copyout over read-only user text pages.
    إذا ((*pte & PTE_W) == 0)
      أرجع -1;

    n = PGSIZE - (dstva - va0);
    إذا (n > len)
      n = len;
    memmove((فراغ *)(pa0 + (dstva - va0)), src, n);

    len -= n;
    src += n;
    dstva = va0 + PGSIZE;
  }
  أرجع 0;
}

// Copy from user to kernel.
// Copy len bytes to dst from virtual address srcva in a given page table.
// Return 0 on success, -1 on error.
صحيح
copyin(pagetable_t pagetable, uint64 psz, حرف *dst, uint64 srcva, uint64 len)
{
  uint64 n, va0, pa0;

  طالما (len > 0) {
    va0 = PGROUNDDOWN(srcva);
    pa0 = walkaddr(pagetable, va0);
    إذا (pa0 == 0) {
      إذا ((pa0 = vmfault(pagetable, psz, va0, 1)) == 0) {
        أرجع -1;
      }
    }
    n = PGSIZE - (srcva - va0);
    إذا (n > len)
      n = len;
    memmove(dst, (فراغ *)(pa0 + (srcva - va0)), n);

    len -= n;
    dst += n;
    srcva = va0 + PGSIZE;
  }
  أرجع 0;
}

// Copy a null-terminated string from user to kernel.
// Copy bytes to dst from virtual address srcva in a given page table,
// until a '\0', or max.
// Return 0 on success, -1 on error.
صحيح
copyinstr(pagetable_t pagetable, uint64 psz, حرف *dst, uint64 srcva,
          uint64 max)
{
  uint64 n, va0, pa0;
  صحيح got_null = 0;

  طالما (got_null == 0 && max > 0) {
    va0 = PGROUNDDOWN(srcva);
    pa0 = walkaddr(pagetable, va0);
    إذا (pa0 == 0) {
      إذا ((pa0 = vmfault(pagetable, psz, va0, 1)) == 0) {
        أرجع -1;
      }
    }
    n = PGSIZE - (srcva - va0);
    إذا (n > max)
      n = max;

    حرف *p = (حرف *)(pa0 + (srcva - va0));
    طالما (n > 0) {
      إذا (*p == '\0') {
        *dst = '\0';
        got_null = 1;
        اكسر;
      } وإلا {
        *dst = *p;
      }
      --n;
      --max;
      p++;
      dst++;
    }

    srcva = va0 + PGSIZE;
  }
  إذا (got_null) {
    أرجع 0;
  } وإلا {
    أرجع -1;
  }
}

// allocate and map user memory if process is referencing a page
// that was lazily allocated in sys_sbrk().
// returns 0 if va is invalid or already mapped, or if
// out of physical memory, and physical address if successful.
uint64
vmfault(pagetable_t pagetable, uint64 psz, uint64 va, صحيح اقرأ)
{
  uint64 mem;

  إذا (va >= psz)
    أرجع 0;
  va = PGROUNDDOWN(va);
  إذا (ismapped(pagetable, va)) {
    أرجع 0;
  }
  mem = (uint64)kalloc();
  إذا (mem == 0)
    أرجع 0;
  memset((فراغ *)mem, 0, PGSIZE);
  إذا (mappages(pagetable, va, PGSIZE, mem, PTE_W | PTE_U | PTE_R) != 0) {
    kfree((فراغ *)mem);
    أرجع 0;
  }
  أرجع mem;
}

صحيح
ismapped(pagetable_t pagetable, uint64 va)
{
  pte_t *pte = walk(pagetable, va, 0);
  إذا (pte == 0) {
    أرجع 0;
  }
  إذا (*pte & PTE_V) {
    أرجع 1;
  }
  أرجع 0;
}
