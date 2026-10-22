// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#شامل "types.h"
#شامل "param.h"
#شامل "memlayout.h"
#شامل "spinlock.h"
#شامل "riscv.h"
#شامل "defs.h"

تهی freerange(تهی *pa_start, تهی *pa_end);

بیرونی نویسه end[]; // first address after kernel.
                   // defined by kernel.ld.

ساختار run {
  ساختار run *next;
};

ساختار {
  ساختار spinlock lock;
  ساختار run *freelist;
} kmem;

تهی
kinit()
{
  initlock(&kmem.lock, "kmem");
  freerange(end, (تهی *)PHYSTOP);
}

تهی
freerange(تهی *pa_start, تهی *pa_end)
{
  نویسه *p;
  p = (نویسه *)PGROUNDUP((uint64)pa_start);
  برای (; p + PGSIZE <= (نویسه *)pa_end; p += PGSIZE)
    kfree(p);
}

// Free the page of physical memory pointed at by pa,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
تهی
kfree(تهی *pa)
{
  ساختار run *r;

  اگر (((uint64)pa % PGSIZE) != 0 || (نویسه *)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);

  r = (ساختار run *)pa;

  acquire(&kmem.lock);
  r->next = kmem.freelist;
  kmem.freelist = r;
  release(&kmem.lock);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
تهی *
kalloc(تهی)
{
  ساختار run *r;

  acquire(&kmem.lock);
  r = kmem.freelist;
  اگر (r)
    kmem.freelist = r->next;
  release(&kmem.lock);

  اگر (r)
    memset((نویسه *)r, 5, PGSIZE); // fill with junk
  بازگشت (تهی *)r;
}
