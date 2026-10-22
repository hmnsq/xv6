// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#شامل "types.h"
#شامل "param.h"
#شامل "memlayout.h"
#شامل "spinlock.h"
#شامل "riscv.h"
#شامل "defs.h"

خالی freerange(خالی *pa_آغاز, خالی *pa_انتہا);

خارجی حرف انتہا[]; // first address after kernel.
                   // defined by kernel.ld.

ساخت تسلسل {
  ساخت تسلسل *اگلا;
};

ساخت {
  ساخت spinlock تالا;
  ساخت تسلسل *freelist;
} kmem;

خالی
kinit()
{
  initlock(&kmem.تالا, "kmem");
  freerange(انتہا, (خالی *)PHYSTOP);
}

خالی
freerange(خالی *pa_آغاز, خالی *pa_انتہا)
{
  حرف *p;
  p = (حرف *)PGROUNDUP((uint64)pa_آغاز);
  برائے (; p + PGSIZE <= (حرف *)pa_انتہا; p += PGSIZE)
    kfree(p);
}

// Free the page of physical memory pointed at by pa,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
خالی
kfree(خالی *pa)
{
  ساخت تسلسل *r;

  اگر (((uint64)pa % PGSIZE) != 0 || (حرف *)pa < انتہا || (uint64)pa >= PHYSTOP)
    panic("kfree");

  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);

  r = (ساخت تسلسل *)pa;

  acquire(&kmem.تالا);
  r->اگلا = kmem.freelist;
  kmem.freelist = r;
  release(&kmem.تالا);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
خالی *
kalloc(خالی)
{
  ساخت تسلسل *r;

  acquire(&kmem.تالا);
  r = kmem.freelist;
  اگر (r)
    kmem.freelist = r->اگلا;
  release(&kmem.تالا);

  اگر (r)
    memset((حرف *)r, 5, PGSIZE); // fill with junk
  واپس (خالی *)r;
}
