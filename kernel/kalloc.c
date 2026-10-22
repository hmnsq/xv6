// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#ضمن "types.h"
#ضمن "param.h"
#ضمن "memlayout.h"
#ضمن "spinlock.h"
#ضمن "riscv.h"
#ضمن "defs.h"

فراغ freerange(فراغ *pa_شروع, فراغ *pa_سرا);

خارجي حرف سرا[]; // first address after kernel.
                   // defined by kernel.ld.

بنية لگاتار {
  بنية لگاتار *اگلا;
};

بنية {
  بنية spinlock جندرا;
  بنية لگاتار *freelist;
} kmem;

فراغ
kinit()
{
  initlock(&kmem.جندرا, "kmem");
  freerange(سرا, (فراغ *)PHYSTOP);
}

فراغ
freerange(فراغ *pa_شروع, فراغ *pa_سرا)
{
  حرف *p;
  p = (حرف *)PGROUNDUP((uint64)pa_شروع);
  لكل (; p + PGSIZE <= (حرف *)pa_سرا; p += PGSIZE)
    kfree(p);
}

// Free the page of physical memory pointed at by pa,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
فراغ
kfree(فراغ *pa)
{
  بنية لگاتار *r;

  إذا (((uint64)pa % PGSIZE) != 0 || (حرف *)pa < سرا || (uint64)pa >= PHYSTOP)
    panic("kfree");

  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);

  r = (بنية لگاتار *)pa;

  acquire(&kmem.جندرا);
  r->اگلا = kmem.freelist;
  kmem.freelist = r;
  release(&kmem.جندرا);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
فراغ *
kalloc(فراغ)
{
  بنية لگاتار *r;

  acquire(&kmem.جندرا);
  r = kmem.freelist;
  إذا (r)
    kmem.freelist = r->اگلا;
  release(&kmem.جندرا);

  إذا (r)
    memset((حرف *)r, 5, PGSIZE); // fill with junk
  أرجع (فراغ *)r;
}
