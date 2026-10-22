// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#ضمن "types.h"
#ضمن "param.h"
#ضمن "memlayout.h"
#ضمن "spinlock.h"
#ضمن "riscv.h"
#ضمن "defs.h"

فراغ freerange(فراغ *pa_شروع, فراغ *pa_انتها);

خارجي حرف انتها[]; // first address after kernel.
                   // defined by kernel.ld.

بنية پیاپی {
  بنية پیاپی *بعدی;
};

بنية {
  بنية spinlock قفل;
  بنية پیاپی *freelist;
} kmem;

فراغ
kinit()
{
  initlock(&kmem.قفل, "kmem");
  freerange(انتها, (فراغ *)PHYSTOP);
}

فراغ
freerange(فراغ *pa_شروع, فراغ *pa_انتها)
{
  حرف *p;
  p = (حرف *)PGROUNDUP((uint64)pa_شروع);
  لكل (; p + PGSIZE <= (حرف *)pa_انتها; p += PGSIZE)
    kfree(p);
}

// Free the page of physical memory pointed at by pa,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
فراغ
kfree(فراغ *pa)
{
  بنية پیاپی *r;

  إذا (((uint64)pa % PGSIZE) != 0 || (حرف *)pa < انتها || (uint64)pa >= PHYSTOP)
    panic("kfree");

  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);

  r = (بنية پیاپی *)pa;

  acquire(&kmem.قفل);
  r->بعدی = kmem.freelist;
  kmem.freelist = r;
  release(&kmem.قفل);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
فراغ *
kalloc(فراغ)
{
  بنية پیاپی *r;

  acquire(&kmem.قفل);
  r = kmem.freelist;
  إذا (r)
    kmem.freelist = r->بعدی;
  release(&kmem.قفل);

  إذا (r)
    memset((حرف *)r, 5, PGSIZE); // fill with junk
  أرجع (فراغ *)r;
}
