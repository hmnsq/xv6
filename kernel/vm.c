#include "param.h"
#include "types.h"
#include "memlayout.h"
#include "elf.h"
#include "riscv.h"
#include "defs.h"
#include "spinlock.h"
#include "proc.h"
#include "fs.h"

/*
 * the kernel's page table.
 */
pagetable_t kernel_pagetable;

extern char etext[]; // kernel.ld sets this to end of kernel code.

extern char trampoline[]; // trampoline.S

// Make a direct-map page table for the kernel.
pagetable_t
kvmmake(void)
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

  반환 kpgtbl;
}

// add a mapping to the kernel page table.
// only used when booting.
// does not flush TLB or enable paging.
void
kvmmap(pagetable_t kpgtbl, uint64 va, uint64 pa, uint64 sz, 정수 perm)
{
  만약 (mappages(kpgtbl, va, sz, pa, perm) != 0)
    panic("kvmmap");
}

// Initialize the kernel_pagetable, shared by all CPUs.
void
kvminit(void)
{
  kernel_pagetable = kvmmake();
}

// Switch the current CPU's h/w page table register to
// the kernel's page table, and enable paging.
void
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
walk(pagetable_t pagetable, uint64 va, 정수 alloc)
{
  만약 (va >= MAXVA)
    panic("walk");

  위해 (정수 level = 2; level > 0; level--) {
    pte_t *pte = &pagetable[PX(level, va)];
    만약 (*pte & PTE_V) {
      pagetable = (pagetable_t)PTE2PA(*pte);
    } 아니면 {
      만약 (!alloc || (pagetable = (pde_t *)kalloc()) == 0)
        반환 0;
      memset(pagetable, 0, PGSIZE);
      *pte = PA2PTE(pagetable) | PTE_V;
    }
  }
  반환 &pagetable[PX(0, va)];
}

// Look up a virtual address, return the physical address,
// or 0 if not mapped.
// Can only be used to look up user pages.
uint64
walkaddr(pagetable_t pagetable, uint64 va)
{
  pte_t *pte;
  uint64 pa;

  만약 (va >= MAXVA)
    반환 0;

  pte = walk(pagetable, va, 0);
  만약 (pte == 0)
    반환 0;
  만약 ((*pte & PTE_V) == 0)
    반환 0;
  만약 ((*pte & PTE_U) == 0)
    반환 0;
  pa = PTE2PA(*pte);
  반환 pa;
}

// Create PTEs for virtual addresses starting at va that refer to
// physical addresses starting at pa.
// va and size MUST be page-aligned.
// Returns 0 on success, -1 if walk() couldn't
// allocate a needed page-table page.
정수
mappages(pagetable_t pagetable, uint64 va, uint64 size, uint64 pa, 정수 perm)
{
  uint64 a, last;
  pte_t *pte;

  만약 ((va % PGSIZE) != 0)
    panic("mappages: va not aligned");

  만약 ((size % PGSIZE) != 0)
    panic("mappages: size not aligned");

  만약 (size == 0)
    panic("mappages: size");

  a = va;
  last = va + size - PGSIZE;
  위해 (;;) {
    만약 ((pte = walk(pagetable, a, 1)) == 0)
      반환 -1;
    만약 (*pte & PTE_V)
      panic("mappages: remap");
    *pte = PA2PTE(pa) | perm | PTE_V;
    만약 (a == last)
      break;
    a += PGSIZE;
    pa += PGSIZE;
  }
  반환 0;
}

// create an empty user page table.
// returns 0 if out of memory.
pagetable_t
uvmcreate()
{
  pagetable_t pagetable;
  pagetable = (pagetable_t)kalloc();
  만약 (pagetable == 0)
    반환 0;
  memset(pagetable, 0, PGSIZE);
  반환 pagetable;
}

// Remove npages of mappings starting from va. va must be
// page-aligned. It's OK if the mappings don't exist.
// Optionally free the physical memory.
void
uvmunmap(pagetable_t pagetable, uint64 va, uint64 npages, 정수 do_free)
{
  uint64 a;
  pte_t *pte;

  만약 ((va % PGSIZE) != 0)
    panic("uvmunmap: not aligned");

  위해 (a = va; a < va + npages * PGSIZE; a += PGSIZE) {
    만약 ((pte = walk(pagetable, a, 0)) == 0) // leaf page table entry allocated?
      continue;
    만약 ((*pte & PTE_V) == 0) // has physical page been allocated?
      continue;
    만약 (do_free) {
      uint64 pa = PTE2PA(*pte);
      kfree((void *)pa);
    }
    *pte = 0;
  }
}

// Allocate PTEs and physical memory to grow a process from oldsz to
// newsz, which need not be page aligned.  Returns new size or 0 on error.
uint64
uvmalloc(pagetable_t pagetable, uint64 oldsz, uint64 newsz, 정수 xperm)
{
  char *mem;
  uint64 a;

  만약 (newsz < oldsz)
    반환 oldsz;

  oldsz = PGROUNDUP(oldsz);
  위해 (a = oldsz; a < newsz; a += PGSIZE) {
    mem = kalloc();
    만약 (mem == 0) {
      uvmdealloc(pagetable, a, oldsz);
      반환 0;
    }
    memset(mem, 0, PGSIZE);
    만약 (mappages(pagetable, a, PGSIZE, (uint64)mem, PTE_R | PTE_U | xperm) !=
        0) {
      kfree(mem);
      uvmdealloc(pagetable, a, oldsz);
      반환 0;
    }
  }
  반환 newsz;
}

// Deallocate user pages to bring the process size from oldsz to
// newsz.  oldsz and newsz need not be page-aligned, nor does newsz
// need to be less than oldsz.  oldsz can be larger than the actual
// process size.  Returns the new process size.
uint64
uvmdealloc(pagetable_t pagetable, uint64 oldsz, uint64 newsz)
{
  만약 (newsz >= oldsz)
    반환 oldsz;

  만약 (PGROUNDUP(newsz) < PGROUNDUP(oldsz)) {
    정수 npages = (PGROUNDUP(oldsz) - PGROUNDUP(newsz)) / PGSIZE;
    uvmunmap(pagetable, PGROUNDUP(newsz), npages, 1);
  }

  반환 newsz;
}

// Recursively free page-table pages.
// All leaf mappings must already have been removed.
void
freewalk(pagetable_t pagetable)
{
  // there are 2^9 = 512 PTEs in a page table.
  위해 (정수 i = 0; i < 512; i++) {
    pte_t pte = pagetable[i];
    만약 ((pte & PTE_V) && (pte & (PTE_R | PTE_W | PTE_X)) == 0) {
      // this PTE points to a lower-level page table.
      uint64 child = PTE2PA(pte);
      freewalk((pagetable_t)child);
      pagetable[i] = 0;
    } 아니면 만약 (pte & PTE_V) {
      panic("freewalk: leaf");
    }
  }
  kfree((void *)pagetable);
}

// Free user memory pages,
// then free page-table pages.
void
uvmfree(pagetable_t pagetable, uint64 sz)
{
  만약 (sz > 0)
    uvmunmap(pagetable, 0, PGROUNDUP(sz) / PGSIZE, 1);
  freewalk(pagetable);
}

// Given a parent process's page table, copy
// its memory into a child's page table.
// Copies both the page table and the
// physical memory.
// returns 0 on success, -1 on failure.
// frees any allocated pages on failure.
정수
uvmcopy(pagetable_t old, pagetable_t new, uint64 sz)
{
  pte_t *pte;
  uint64 pa, i;
  uint flags;
  char *mem;

  위해 (i = 0; i < sz; i += PGSIZE) {
    만약 ((pte = walk(old, i, 0)) == 0)
      continue; // page table entry hasn't been allocated
    만약 ((*pte & PTE_V) == 0)
      continue; // physical page hasn't been allocated
    pa = PTE2PA(*pte);
    flags = PTE_FLAGS(*pte);
    만약 ((mem = kalloc()) == 0)
      goto err;
    memmove(mem, (char *)pa, PGSIZE);
    만약 (mappages(new, i, PGSIZE, (uint64)mem, flags) != 0) {
      kfree(mem);
      goto err;
    }
  }
  반환 0;

err:
  uvmunmap(new, 0, i / PGSIZE, 1);
  반환 -1;
}

// mark a PTE invalid for user access.
// used by exec for the user stack guard page.
void
uvmclear(pagetable_t pagetable, uint64 va)
{
  pte_t *pte;

  pte = walk(pagetable, va, 0);
  만약 (pte == 0)
    panic("uvmclear");
  *pte &= ~PTE_U;
}

// Copy from kernel to user.
// Copy len bytes from src to virtual address dstva in a given page table.
// Return 0 on success, -1 on error.
정수
copyout(pagetable_t pagetable, uint64 psz, uint64 dstva, char *src, uint64 len)
{
  uint64 n, va0, pa0;
  pte_t *pte;

  동안 (len > 0) {
    va0 = PGROUNDDOWN(dstva);
    만약 (va0 >= MAXVA)
      반환 -1;

    pa0 = walkaddr(pagetable, va0);
    만약 (pa0 == 0) {
      만약 ((pa0 = vmfault(pagetable, psz, va0, 0)) == 0) {
        반환 -1;
      }
    }

    pte = walk(pagetable, va0, 0);
    // forbid copyout over read-only user text pages.
    만약 ((*pte & PTE_W) == 0)
      반환 -1;

    n = PGSIZE - (dstva - va0);
    만약 (n > len)
      n = len;
    memmove((void *)(pa0 + (dstva - va0)), src, n);

    len -= n;
    src += n;
    dstva = va0 + PGSIZE;
  }
  반환 0;
}

// Copy from user to kernel.
// Copy len bytes to dst from virtual address srcva in a given page table.
// Return 0 on success, -1 on error.
정수
copyin(pagetable_t pagetable, uint64 psz, char *dst, uint64 srcva, uint64 len)
{
  uint64 n, va0, pa0;

  동안 (len > 0) {
    va0 = PGROUNDDOWN(srcva);
    pa0 = walkaddr(pagetable, va0);
    만약 (pa0 == 0) {
      만약 ((pa0 = vmfault(pagetable, psz, va0, 1)) == 0) {
        반환 -1;
      }
    }
    n = PGSIZE - (srcva - va0);
    만약 (n > len)
      n = len;
    memmove(dst, (void *)(pa0 + (srcva - va0)), n);

    len -= n;
    dst += n;
    srcva = va0 + PGSIZE;
  }
  반환 0;
}

// Copy a null-terminated string from user to kernel.
// Copy bytes to dst from virtual address srcva in a given page table,
// until a '\0', or max.
// Return 0 on success, -1 on error.
정수
copyinstr(pagetable_t pagetable, uint64 psz, char *dst, uint64 srcva,
          uint64 max)
{
  uint64 n, va0, pa0;
  정수 got_null = 0;

  동안 (got_null == 0 && max > 0) {
    va0 = PGROUNDDOWN(srcva);
    pa0 = walkaddr(pagetable, va0);
    만약 (pa0 == 0) {
      만약 ((pa0 = vmfault(pagetable, psz, va0, 1)) == 0) {
        반환 -1;
      }
    }
    n = PGSIZE - (srcva - va0);
    만약 (n > max)
      n = max;

    char *p = (char *)(pa0 + (srcva - va0));
    동안 (n > 0) {
      만약 (*p == '\0') {
        *dst = '\0';
        got_null = 1;
        break;
      } 아니면 {
        *dst = *p;
      }
      --n;
      --max;
      p++;
      dst++;
    }

    srcva = va0 + PGSIZE;
  }
  만약 (got_null) {
    반환 0;
  } 아니면 {
    반환 -1;
  }
}

// allocate and map user memory if process is referencing a page
// that was lazily allocated in sys_sbrk().
// returns 0 if va is invalid or already mapped, or if
// out of physical memory, and physical address if successful.
uint64
vmfault(pagetable_t pagetable, uint64 psz, uint64 va, 정수 read)
{
  uint64 mem;

  만약 (va >= psz)
    반환 0;
  va = PGROUNDDOWN(va);
  만약 (ismapped(pagetable, va)) {
    반환 0;
  }
  mem = (uint64)kalloc();
  만약 (mem == 0)
    반환 0;
  memset((void *)mem, 0, PGSIZE);
  만약 (mappages(pagetable, va, PGSIZE, mem, PTE_W | PTE_U | PTE_R) != 0) {
    kfree((void *)mem);
    반환 0;
  }
  반환 mem;
}

정수
ismapped(pagetable_t pagetable, uint64 va)
{
  pte_t *pte = walk(pagetable, va, 0);
  만약 (pte == 0) {
    반환 0;
  }
  만약 (*pte & PTE_V) {
    반환 1;
  }
  반환 0;
}
