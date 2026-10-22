// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"

void freerange(void *pa_start, void *pa_end);

extern var_na end[]; // first address after kernel.
                   // defined by kernel.ld.

sa_nracanaa run {
  sa_nracanaa run *next;
};

sa_nracanaa {
  sa_nracanaa spinlock lock;
  sa_nracanaa run *freelist;
} kmem;

void
kinit()
{
  initlock(&kmem.lock, "kmem");
  freerange(end, (void *)PHYSTOP);
}

void
freerange(void *pa_start, void *pa_end)
{
  var_na *p;
  p = (var_na *)PGROUNDUP((uint64)pa_start);
  ສຳລັບ (; p + PGSIZE <= (var_na *)pa_end; p += PGSIZE)
    kfree(p);
}

// Free the page of physical memory pointed at by pa,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(void *pa)
{
  sa_nracanaa run *r;

  ຖ້າ (((uint64)pa % PGSIZE) != 0 || (var_na *)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);

  r = (sa_nracanaa run *)pa;

  acquire(&kmem.lock);
  r->next = kmem.freelist;
  kmem.freelist = r;
  release(&kmem.lock);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
void *
kalloc(void)
{
  sa_nracanaa run *r;

  acquire(&kmem.lock);
  r = kmem.freelist;
  ຖ້າ (r)
    kmem.freelist = r->next;
  release(&kmem.lock);

  ຖ້າ (r)
    memset((var_na *)r, 5, PGSIZE); // fill with junk
  ສົ່ງຄືນ (void *)r;
}
