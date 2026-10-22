#அந்தர்பாவ "param.h"
#அந்தர்பாவ "types.h"
#அந்தர்பாவ "memlayout.h"
#அந்தர்பாவ "elf.h"
#அந்தர்பாவ "riscv.h"
#அந்தர்பாவ "defs.h"
#அந்தர்பாவ "spinlock.h"
#அந்தர்பாவ "proc.h"
#அந்தர்பாவ "fs.h"

/*
 * the kernel's page table.
 */
pagetable_t kernel_pagetable;

வெளி எழுத்து etext[]; // kernel.ld sets this to end of kernel code.

வெளி எழுத்து trampoline[]; // trampoline.S

// Make a direct-map page table for the kernel.
pagetable_t
kvmmake(வெறுமை)
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

  திரும்பு kpgtbl;
}

// add a mapping to the kernel page table.
// only used when booting.
// does not flush TLB or enable paging.
வெறுமை
kvmmap(pagetable_t kpgtbl, uint64 va, uint64 pa, uint64 sz, முழு perm)
{
  எனில் (mappages(kpgtbl, va, sz, pa, perm) != 0)
    panic("kvmmap");
}

// Initialize the kernel_pagetable, shared by all CPUs.
வெறுமை
kvminit(வெறுமை)
{
  kernel_pagetable = kvmmake();
}

// Switch the current CPU's h/w page table register to
// the kernel's page table, and enable paging.
வெறுமை
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
walk(pagetable_t pagetable, uint64 va, முழு alloc)
{
  எனில் (va >= MAXVA)
    panic("walk");

  க்கு (முழு level = 2; level > 0; level--) {
    pte_t *pte = &pagetable[PX(level, va)];
    எனில் (*pte & PTE_V) {
      pagetable = (pagetable_t)PTE2PA(*pte);
    } இல்லையெனில் {
      எனில் (!alloc || (pagetable = (pde_t *)kalloc()) == 0)
        திரும்பு 0;
      memset(pagetable, 0, PGSIZE);
      *pte = PA2PTE(pagetable) | PTE_V;
    }
  }
  திரும்பு &pagetable[PX(0, va)];
}

// Look up a virtual address, return the physical address,
// or 0 if not mapped.
// Can only be used to look up user pages.
uint64
walkaddr(pagetable_t pagetable, uint64 va)
{
  pte_t *pte;
  uint64 pa;

  எனில் (va >= MAXVA)
    திரும்பு 0;

  pte = walk(pagetable, va, 0);
  எனில் (pte == 0)
    திரும்பு 0;
  எனில் ((*pte & PTE_V) == 0)
    திரும்பு 0;
  எனில் ((*pte & PTE_U) == 0)
    திரும்பு 0;
  pa = PTE2PA(*pte);
  திரும்பு pa;
}

// Create PTEs for virtual addresses starting at va that refer to
// physical addresses starting at pa.
// va and size MUST be page-aligned.
// Returns 0 on success, -1 if walk() couldn't
// allocate a needed page-table page.
முழு
mappages(pagetable_t pagetable, uint64 va, uint64 size, uint64 pa, முழு perm)
{
  uint64 a, last;
  pte_t *pte;

  எனில் ((va % PGSIZE) != 0)
    panic("mappages: va not aligned");

  எனில் ((size % PGSIZE) != 0)
    panic("mappages: size not aligned");

  எனில் (size == 0)
    panic("mappages: size");

  a = va;
  last = va + size - PGSIZE;
  க்கு (;;) {
    எனில் ((pte = walk(pagetable, a, 1)) == 0)
      திரும்பு -1;
    எனில் (*pte & PTE_V)
      panic("mappages: remap");
    *pte = PA2PTE(pa) | perm | PTE_V;
    எனில் (a == last)
      நிறுத்து;
    a += PGSIZE;
    pa += PGSIZE;
  }
  திரும்பு 0;
}

// create an empty user page table.
// returns 0 if out of memory.
pagetable_t
uvmcreate()
{
  pagetable_t pagetable;
  pagetable = (pagetable_t)kalloc();
  எனில் (pagetable == 0)
    திரும்பு 0;
  memset(pagetable, 0, PGSIZE);
  திரும்பு pagetable;
}

// Remove npages of mappings starting from va. va must be
// page-aligned. It's OK if the mappings don't exist.
// Optionally free the physical memory.
வெறுமை
uvmunmap(pagetable_t pagetable, uint64 va, uint64 npages, முழு do_free)
{
  uint64 a;
  pte_t *pte;

  எனில் ((va % PGSIZE) != 0)
    panic("uvmunmap: not aligned");

  க்கு (a = va; a < va + npages * PGSIZE; a += PGSIZE) {
    எனில் ((pte = walk(pagetable, a, 0)) == 0) // leaf page table entry allocated?
      தொடர்;
    எனில் ((*pte & PTE_V) == 0) // has physical page been allocated?
      தொடர்;
    எனில் (do_free) {
      uint64 pa = PTE2PA(*pte);
      kfree((வெறுமை *)pa);
    }
    *pte = 0;
  }
}

// Allocate PTEs and physical memory to grow a process from oldsz to
// newsz, which need not be page aligned.  Returns new size or 0 on error.
uint64
uvmalloc(pagetable_t pagetable, uint64 oldsz, uint64 newsz, முழு xperm)
{
  எழுத்து *mem;
  uint64 a;

  எனில் (newsz < oldsz)
    திரும்பு oldsz;

  oldsz = PGROUNDUP(oldsz);
  க்கு (a = oldsz; a < newsz; a += PGSIZE) {
    mem = kalloc();
    எனில் (mem == 0) {
      uvmdealloc(pagetable, a, oldsz);
      திரும்பு 0;
    }
    memset(mem, 0, PGSIZE);
    எனில் (mappages(pagetable, a, PGSIZE, (uint64)mem, PTE_R | PTE_U | xperm) !=
        0) {
      kfree(mem);
      uvmdealloc(pagetable, a, oldsz);
      திரும்பு 0;
    }
  }
  திரும்பு newsz;
}

// Deallocate user pages to bring the process size from oldsz to
// newsz.  oldsz and newsz need not be page-aligned, nor does newsz
// need to be less than oldsz.  oldsz can be larger than the actual
// process size.  Returns the new process size.
uint64
uvmdealloc(pagetable_t pagetable, uint64 oldsz, uint64 newsz)
{
  எனில் (newsz >= oldsz)
    திரும்பு oldsz;

  எனில் (PGROUNDUP(newsz) < PGROUNDUP(oldsz)) {
    முழு npages = (PGROUNDUP(oldsz) - PGROUNDUP(newsz)) / PGSIZE;
    uvmunmap(pagetable, PGROUNDUP(newsz), npages, 1);
  }

  திரும்பு newsz;
}

// Recursively free page-table pages.
// All leaf mappings must already have been removed.
வெறுமை
freewalk(pagetable_t pagetable)
{
  // there are 2^9 = 512 PTEs in a page table.
  க்கு (முழு i = 0; i < 512; i++) {
    pte_t pte = pagetable[i];
    எனில் ((pte & PTE_V) && (pte & (PTE_R | PTE_W | PTE_X)) == 0) {
      // this PTE points to a lower-level page table.
      uint64 child = PTE2PA(pte);
      freewalk((pagetable_t)child);
      pagetable[i] = 0;
    } இல்லையெனில் எனில் (pte & PTE_V) {
      panic("freewalk: leaf");
    }
  }
  kfree((வெறுமை *)pagetable);
}

// Free user memory pages,
// then free page-table pages.
வெறுமை
uvmfree(pagetable_t pagetable, uint64 sz)
{
  எனில் (sz > 0)
    uvmunmap(pagetable, 0, PGROUNDUP(sz) / PGSIZE, 1);
  freewalk(pagetable);
}

// Given a parent process's page table, copy
// its memory into a child's page table.
// Copies both the page table and the
// physical memory.
// returns 0 on success, -1 on failure.
// frees any allocated pages on failure.
முழு
uvmcopy(pagetable_t old, pagetable_t new, uint64 sz)
{
  pte_t *pte;
  uint64 pa, i;
  uint flags;
  எழுத்து *mem;

  க்கு (i = 0; i < sz; i += PGSIZE) {
    எனில் ((pte = walk(old, i, 0)) == 0)
      தொடர்; // page table entry hasn't been allocated
    எனில் ((*pte & PTE_V) == 0)
      தொடர்; // physical page hasn't been allocated
    pa = PTE2PA(*pte);
    flags = PTE_FLAGS(*pte);
    எனில் ((mem = kalloc()) == 0)
      செல் err;
    memmove(mem, (எழுத்து *)pa, PGSIZE);
    எனில் (mappages(new, i, PGSIZE, (uint64)mem, flags) != 0) {
      kfree(mem);
      செல் err;
    }
  }
  திரும்பு 0;

err:
  uvmunmap(new, 0, i / PGSIZE, 1);
  திரும்பு -1;
}

// mark a PTE invalid for user access.
// used by exec for the user stack guard page.
வெறுமை
uvmclear(pagetable_t pagetable, uint64 va)
{
  pte_t *pte;

  pte = walk(pagetable, va, 0);
  எனில் (pte == 0)
    panic("uvmclear");
  *pte &= ~PTE_U;
}

// Copy from kernel to user.
// Copy len bytes from src to virtual address dstva in a given page table.
// Return 0 on success, -1 on error.
முழு
copyout(pagetable_t pagetable, uint64 psz, uint64 dstva, எழுத்து *src, uint64 len)
{
  uint64 n, va0, pa0;
  pte_t *pte;

  போது (len > 0) {
    va0 = PGROUNDDOWN(dstva);
    எனில் (va0 >= MAXVA)
      திரும்பு -1;

    pa0 = walkaddr(pagetable, va0);
    எனில் (pa0 == 0) {
      எனில் ((pa0 = vmfault(pagetable, psz, va0, 0)) == 0) {
        திரும்பு -1;
      }
    }

    pte = walk(pagetable, va0, 0);
    // forbid copyout over read-only user text pages.
    எனில் ((*pte & PTE_W) == 0)
      திரும்பு -1;

    n = PGSIZE - (dstva - va0);
    எனில் (n > len)
      n = len;
    memmove((வெறுமை *)(pa0 + (dstva - va0)), src, n);

    len -= n;
    src += n;
    dstva = va0 + PGSIZE;
  }
  திரும்பு 0;
}

// Copy from user to kernel.
// Copy len bytes to dst from virtual address srcva in a given page table.
// Return 0 on success, -1 on error.
முழு
copyin(pagetable_t pagetable, uint64 psz, எழுத்து *dst, uint64 srcva, uint64 len)
{
  uint64 n, va0, pa0;

  போது (len > 0) {
    va0 = PGROUNDDOWN(srcva);
    pa0 = walkaddr(pagetable, va0);
    எனில் (pa0 == 0) {
      எனில் ((pa0 = vmfault(pagetable, psz, va0, 1)) == 0) {
        திரும்பு -1;
      }
    }
    n = PGSIZE - (srcva - va0);
    எனில் (n > len)
      n = len;
    memmove(dst, (வெறுமை *)(pa0 + (srcva - va0)), n);

    len -= n;
    dst += n;
    srcva = va0 + PGSIZE;
  }
  திரும்பு 0;
}

// Copy a null-terminated string from user to kernel.
// Copy bytes to dst from virtual address srcva in a given page table,
// until a '\0', or max.
// Return 0 on success, -1 on error.
முழு
copyinstr(pagetable_t pagetable, uint64 psz, எழுத்து *dst, uint64 srcva,
          uint64 max)
{
  uint64 n, va0, pa0;
  முழு got_null = 0;

  போது (got_null == 0 && max > 0) {
    va0 = PGROUNDDOWN(srcva);
    pa0 = walkaddr(pagetable, va0);
    எனில் (pa0 == 0) {
      எனில் ((pa0 = vmfault(pagetable, psz, va0, 1)) == 0) {
        திரும்பு -1;
      }
    }
    n = PGSIZE - (srcva - va0);
    எனில் (n > max)
      n = max;

    எழுத்து *p = (எழுத்து *)(pa0 + (srcva - va0));
    போது (n > 0) {
      எனில் (*p == '\0') {
        *dst = '\0';
        got_null = 1;
        நிறுத்து;
      } இல்லையெனில் {
        *dst = *p;
      }
      --n;
      --max;
      p++;
      dst++;
    }

    srcva = va0 + PGSIZE;
  }
  எனில் (got_null) {
    திரும்பு 0;
  } இல்லையெனில் {
    திரும்பு -1;
  }
}

// allocate and map user memory if process is referencing a page
// that was lazily allocated in sys_sbrk().
// returns 0 if va is invalid or already mapped, or if
// out of physical memory, and physical address if successful.
uint64
vmfault(pagetable_t pagetable, uint64 psz, uint64 va, முழு பட)
{
  uint64 mem;

  எனில் (va >= psz)
    திரும்பு 0;
  va = PGROUNDDOWN(va);
  எனில் (ismapped(pagetable, va)) {
    திரும்பு 0;
  }
  mem = (uint64)kalloc();
  எனில் (mem == 0)
    திரும்பு 0;
  memset((வெறுமை *)mem, 0, PGSIZE);
  எனில் (mappages(pagetable, va, PGSIZE, mem, PTE_W | PTE_U | PTE_R) != 0) {
    kfree((வெறுமை *)mem);
    திரும்பு 0;
  }
  திரும்பு mem;
}

முழு
ismapped(pagetable_t pagetable, uint64 va)
{
  pte_t *pte = walk(pagetable, va, 0);
  எனில் (pte == 0) {
    திரும்பு 0;
  }
  எனில் (*pte & PTE_V) {
    திரும்பு 1;
  }
  திரும்பு 0;
}
