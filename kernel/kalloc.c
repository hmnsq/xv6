// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#ضمن "types.h"
#ضمن "param.h"
#ضمن "memlayout.h"
#ضمن "spinlock.h"
#ضمن "riscv.h"
#ضمن "defs.h"

فراغ freerange(فراغ *pa_شروعات, فراغ *pa_پڇاڙي);

خارجي حرف پڇاڙي[]; // first address after kernel.
                   // defined by kernel.ld.

بنية لاڳيتو {
  بنية لاڳيتو *ايندڙ;
};

بنية {
  بنية spinlock تالو;
  بنية لاڳيتو *freelist;
} kmem;

فراغ
kinit()
{
  initlock(&kmem.تالو, "kmem");
  freerange(پڇاڙي, (فراغ *)PHYSTOP);
}

فراغ
freerange(فراغ *pa_شروعات, فراغ *pa_پڇاڙي)
{
  حرف *p;
  p = (حرف *)PGROUNDUP((uint64)pa_شروعات);
  لكل (; p + PGSIZE <= (حرف *)pa_پڇاڙي; p += PGSIZE)
    kfree(p);
}

// Free the page of physical memory pointed at by pa,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
فراغ
kfree(فراغ *pa)
{
  بنية لاڳيتو *r;

  إذا (((uint64)pa % PGSIZE) != 0 || (حرف *)pa < پڇاڙي || (uint64)pa >= PHYSTOP)
    panic("kfree");

  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);

  r = (بنية لاڳيتو *)pa;

  acquire(&kmem.تالو);
  r->ايندڙ = kmem.freelist;
  kmem.freelist = r;
  release(&kmem.تالو);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
فراغ *
kalloc(فراغ)
{
  بنية لاڳيتو *r;

  acquire(&kmem.تالو);
  r = kmem.freelist;
  إذا (r)
    kmem.freelist = r->ايندڙ;
  release(&kmem.تالو);

  إذا (r)
    memset((حرف *)r, 5, PGSIZE); // fill with junk
  أرجع (فراغ *)r;
}
