#अन्तर्भाव "param.h"
#अन्तर्भाव "types.h"
#अन्तर्भाव "memlayout.h"
#अन्तर्भाव "elf.h"
#अन्तर्भाव "riscv.h"
#अन्तर्भाव "defs.h"
#अन्तर्भाव "spinlock.h"
#अन्तर्भाव "proc.h"
#अन्तर्भाव "fs.h"

/*
 * the kernel's page table.
 */
pagetable_t kernel_pagetable;

बाह्य अक्षर etext[]; // kernel.ld sets this to end of kernel code.

बाह्य अक्षर trampoline[]; // trampoline.S

// Make a direct-map page table for the kernel.
pagetable_t
kvmmake(शून्य)
{
  pagetable_t kpgtbl;

  kpgtbl = (pagetable_t)kalloc();
  स_रखो(kpgtbl, 0, PGSIZE);

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

  परत kpgtbl;
}

// add a mapping to the kernel page table.
// only used when booting.
// does not flush TLB or enable paging.
शून्य
kvmmap(pagetable_t kpgtbl, uint64 va, uint64 pa, uint64 sz, पूर्णांक perm)
{
  जर (mappages(kpgtbl, va, sz, pa, perm) != 0)
    panic("kvmmap");
}

// Initialize the kernel_pagetable, shared by all CPUs.
शून्य
kvminit(शून्य)
{
  kernel_pagetable = kvmmake();
}

// Switch the current CPU's h/w page table register to
// the kernel's page table, and enable paging.
शून्य
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
walk(pagetable_t pagetable, uint64 va, पूर्णांक alloc)
{
  जर (va >= MAXVA)
    panic("walk");

  साठी (पूर्णांक level = 2; level > 0; level--) {
    pte_t *pte = &pagetable[PX(level, va)];
    जर (*pte & PTE_V) {
      pagetable = (pagetable_t)PTE2PA(*pte);
    } अन्यथा {
      जर (!alloc || (pagetable = (pde_t *)kalloc()) == 0)
        परत 0;
      स_रखो(pagetable, 0, PGSIZE);
      *pte = PA2PTE(pagetable) | PTE_V;
    }
  }
  परत &pagetable[PX(0, va)];
}

// Look up a virtual address, return the physical address,
// or 0 if not mapped.
// Can only be used to look up user pages.
uint64
walkaddr(pagetable_t pagetable, uint64 va)
{
  pte_t *pte;
  uint64 pa;

  जर (va >= MAXVA)
    परत 0;

  pte = walk(pagetable, va, 0);
  जर (pte == 0)
    परत 0;
  जर ((*pte & PTE_V) == 0)
    परत 0;
  जर ((*pte & PTE_U) == 0)
    परत 0;
  pa = PTE2PA(*pte);
  परत pa;
}

// Create PTEs for virtual addresses starting at va that refer to
// physical addresses starting at pa.
// va and size MUST be page-aligned.
// Returns 0 on success, -1 if walk() couldn't
// allocate a needed page-table page.
पूर्णांक
mappages(pagetable_t pagetable, uint64 va, uint64 size, uint64 pa, पूर्णांक perm)
{
  uint64 a, last;
  pte_t *pte;

  जर ((va % PGSIZE) != 0)
    panic("mappages: va not aligned");

  जर ((size % PGSIZE) != 0)
    panic("mappages: size not aligned");

  जर (size == 0)
    panic("mappages: size");

  a = va;
  last = va + size - PGSIZE;
  साठी (;;) {
    जर ((pte = walk(pagetable, a, 1)) == 0)
      परत -1;
    जर (*pte & PTE_V)
      panic("mappages: remap");
    *pte = PA2PTE(pa) | perm | PTE_V;
    जर (a == last)
      थांबा;
    a += PGSIZE;
    pa += PGSIZE;
  }
  परत 0;
}

// create an empty user page table.
// returns 0 if out of memory.
pagetable_t
uvmcreate()
{
  pagetable_t pagetable;
  pagetable = (pagetable_t)kalloc();
  जर (pagetable == 0)
    परत 0;
  स_रखो(pagetable, 0, PGSIZE);
  परत pagetable;
}

// Remove npages of mappings starting from va. va must be
// page-aligned. It's OK if the mappings don't exist.
// Optionally free the physical memory.
शून्य
uvmunmap(pagetable_t pagetable, uint64 va, uint64 npages, पूर्णांक do_free)
{
  uint64 a;
  pte_t *pte;

  जर ((va % PGSIZE) != 0)
    panic("uvmunmap: not aligned");

  साठी (a = va; a < va + npages * PGSIZE; a += PGSIZE) {
    जर ((pte = walk(pagetable, a, 0)) == 0) // leaf page table entry allocated?
      सुरूठेवा;
    जर ((*pte & PTE_V) == 0) // has physical page been allocated?
      सुरूठेवा;
    जर (do_free) {
      uint64 pa = PTE2PA(*pte);
      kfree((शून्य *)pa);
    }
    *pte = 0;
  }
}

// Allocate PTEs and physical memory to grow a process from oldsz to
// newsz, which need not be page aligned.  Returns new size or 0 on error.
uint64
uvmalloc(pagetable_t pagetable, uint64 oldsz, uint64 newsz, पूर्णांक xperm)
{
  अक्षर *mem;
  uint64 a;

  जर (newsz < oldsz)
    परत oldsz;

  oldsz = PGROUNDUP(oldsz);
  साठी (a = oldsz; a < newsz; a += PGSIZE) {
    mem = kalloc();
    जर (mem == 0) {
      uvmdealloc(pagetable, a, oldsz);
      परत 0;
    }
    स_रखो(mem, 0, PGSIZE);
    जर (mappages(pagetable, a, PGSIZE, (uint64)mem, PTE_R | PTE_U | xperm) !=
        0) {
      kfree(mem);
      uvmdealloc(pagetable, a, oldsz);
      परत 0;
    }
  }
  परत newsz;
}

// Deallocate user pages to bring the process size from oldsz to
// newsz.  oldsz and newsz need not be page-aligned, nor does newsz
// need to be less than oldsz.  oldsz can be larger than the actual
// process size.  Returns the new process size.
uint64
uvmdealloc(pagetable_t pagetable, uint64 oldsz, uint64 newsz)
{
  जर (newsz >= oldsz)
    परत oldsz;

  जर (PGROUNDUP(newsz) < PGROUNDUP(oldsz)) {
    पूर्णांक npages = (PGROUNDUP(oldsz) - PGROUNDUP(newsz)) / PGSIZE;
    uvmunmap(pagetable, PGROUNDUP(newsz), npages, 1);
  }

  परत newsz;
}

// Recursively free page-table pages.
// All leaf mappings must already have been removed.
शून्य
freewalk(pagetable_t pagetable)
{
  // there are 2^9 = 512 PTEs in a page table.
  साठी (पूर्णांक i = 0; i < 512; i++) {
    pte_t pte = pagetable[i];
    जर ((pte & PTE_V) && (pte & (PTE_R | PTE_W | PTE_X)) == 0) {
      // this PTE points to a lower-level page table.
      uint64 child = PTE2PA(pte);
      freewalk((pagetable_t)child);
      pagetable[i] = 0;
    } अन्यथा जर (pte & PTE_V) {
      panic("freewalk: leaf");
    }
  }
  kfree((शून्य *)pagetable);
}

// Free user memory pages,
// then free page-table pages.
शून्य
uvmfree(pagetable_t pagetable, uint64 sz)
{
  जर (sz > 0)
    uvmunmap(pagetable, 0, PGROUNDUP(sz) / PGSIZE, 1);
  freewalk(pagetable);
}

// Given a parent process's page table, copy
// its memory into a child's page table.
// Copies both the page table and the
// physical memory.
// returns 0 on success, -1 on failure.
// frees any allocated pages on failure.
पूर्णांक
uvmcopy(pagetable_t old, pagetable_t new, uint64 sz)
{
  pte_t *pte;
  uint64 pa, i;
  uint flags;
  अक्षर *mem;

  साठी (i = 0; i < sz; i += PGSIZE) {
    जर ((pte = walk(old, i, 0)) == 0)
      सुरूठेवा; // page table entry hasn't been allocated
    जर ((*pte & PTE_V) == 0)
      सुरूठेवा; // physical page hasn't been allocated
    pa = PTE2PA(*pte);
    flags = PTE_FLAGS(*pte);
    जर ((mem = kalloc()) == 0)
      जा err;
    स_हटाओ(mem, (अक्षर *)pa, PGSIZE);
    जर (mappages(new, i, PGSIZE, (uint64)mem, flags) != 0) {
      kfree(mem);
      जा err;
    }
  }
  परत 0;

err:
  uvmunmap(new, 0, i / PGSIZE, 1);
  परत -1;
}

// mark a PTE invalid for user access.
// used by exec for the user stack guard page.
शून्य
uvmclear(pagetable_t pagetable, uint64 va)
{
  pte_t *pte;

  pte = walk(pagetable, va, 0);
  जर (pte == 0)
    panic("uvmclear");
  *pte &= ~PTE_U;
}

// Copy from kernel to user.
// Copy len bytes from src to virtual address dstva in a given page table.
// Return 0 on success, -1 on error.
पूर्णांक
copyout(pagetable_t pagetable, uint64 psz, uint64 dstva, अक्षर *src, uint64 len)
{
  uint64 n, va0, pa0;
  pte_t *pte;

  पर्यंत (len > 0) {
    va0 = PGROUNDDOWN(dstva);
    जर (va0 >= MAXVA)
      परत -1;

    pa0 = walkaddr(pagetable, va0);
    जर (pa0 == 0) {
      जर ((pa0 = vmfault(pagetable, psz, va0, 0)) == 0) {
        परत -1;
      }
    }

    pte = walk(pagetable, va0, 0);
    // forbid copyout over read-only user text pages.
    जर ((*pte & PTE_W) == 0)
      परत -1;

    n = PGSIZE - (dstva - va0);
    जर (n > len)
      n = len;
    स_हटाओ((शून्य *)(pa0 + (dstva - va0)), src, n);

    len -= n;
    src += n;
    dstva = va0 + PGSIZE;
  }
  परत 0;
}

// Copy from user to kernel.
// Copy len bytes to dst from virtual address srcva in a given page table.
// Return 0 on success, -1 on error.
पूर्णांक
copyin(pagetable_t pagetable, uint64 psz, अक्षर *dst, uint64 srcva, uint64 len)
{
  uint64 n, va0, pa0;

  पर्यंत (len > 0) {
    va0 = PGROUNDDOWN(srcva);
    pa0 = walkaddr(pagetable, va0);
    जर (pa0 == 0) {
      जर ((pa0 = vmfault(pagetable, psz, va0, 1)) == 0) {
        परत -1;
      }
    }
    n = PGSIZE - (srcva - va0);
    जर (n > len)
      n = len;
    स_हटाओ(dst, (शून्य *)(pa0 + (srcva - va0)), n);

    len -= n;
    dst += n;
    srcva = va0 + PGSIZE;
  }
  परत 0;
}

// Copy a null-terminated string from user to kernel.
// Copy bytes to dst from virtual address srcva in a given page table,
// until a '\0', or max.
// Return 0 on success, -1 on error.
पूर्णांक
copyinstr(pagetable_t pagetable, uint64 psz, अक्षर *dst, uint64 srcva,
          uint64 max)
{
  uint64 n, va0, pa0;
  पूर्णांक got_null = 0;

  पर्यंत (got_null == 0 && max > 0) {
    va0 = PGROUNDDOWN(srcva);
    pa0 = walkaddr(pagetable, va0);
    जर (pa0 == 0) {
      जर ((pa0 = vmfault(pagetable, psz, va0, 1)) == 0) {
        परत -1;
      }
    }
    n = PGSIZE - (srcva - va0);
    जर (n > max)
      n = max;

    अक्षर *p = (अक्षर *)(pa0 + (srcva - va0));
    पर्यंत (n > 0) {
      जर (*p == '\0') {
        *dst = '\0';
        got_null = 1;
        थांबा;
      } अन्यथा {
        *dst = *p;
      }
      --n;
      --max;
      p++;
      dst++;
    }

    srcva = va0 + PGSIZE;
  }
  जर (got_null) {
    परत 0;
  } अन्यथा {
    परत -1;
  }
}

// allocate and map user memory if process is referencing a page
// that was lazily allocated in sys_sbrk().
// returns 0 if va is invalid or already mapped, or if
// out of physical memory, and physical address if successful.
uint64
vmfault(pagetable_t pagetable, uint64 psz, uint64 va, पूर्णांक पठ)
{
  uint64 mem;

  जर (va >= psz)
    परत 0;
  va = PGROUNDDOWN(va);
  जर (ismapped(pagetable, va)) {
    परत 0;
  }
  mem = (uint64)kalloc();
  जर (mem == 0)
    परत 0;
  स_रखो((शून्य *)mem, 0, PGSIZE);
  जर (mappages(pagetable, va, PGSIZE, mem, PTE_W | PTE_U | PTE_R) != 0) {
    kfree((शून्य *)mem);
    परत 0;
  }
  परत mem;
}

पूर्णांक
ismapped(pagetable_t pagetable, uint64 va)
{
  pte_t *pte = walk(pagetable, va, 0);
  जर (pte == 0) {
    परत 0;
  }
  जर (*pte & PTE_V) {
    परत 1;
  }
  परत 0;
}
