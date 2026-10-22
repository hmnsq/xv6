// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#ضمن "types.h"
#ضمن "param.h"
#ضمن "memlayout.h"
#ضمن "spinlock.h"
#ضمن "riscv.h"
#ضمن "defs.h"

فراغ freerange(فراغ *pa_start, فراغ *pa_end);

خارجي حرف end[]; // first address after kernel.
                   // defined by kernel.ld.

بنية run {
  بنية run *next;
};

بنية {
  بنية spinlock lock;
  بنية run *freelist;
} kmem;

فراغ
kinit()
{
  initlock(&kmem.lock, "kmem");
  freerange(end, (فراغ *)PHYSTOP);
}

فراغ
freerange(فراغ *pa_start, فراغ *pa_end)
{
  حرف *p;
  p = (حرف *)PGROUNDUP((uint64)pa_start);
  עבור (; p + PGSIZE <= (حرف *)pa_end; p += PGSIZE)
    kfree(p);
}

// Free the page of physical memory pointed at by pa,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
فراغ
kfree(فراغ *pa)
{
  بنية run *r;

  אם (((uint64)pa % PGSIZE) != 0 || (حرف *)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);

  r = (بنية run *)pa;

  acquire(&kmem.lock);
  r->next = kmem.freelist;
  kmem.freelist = r;
  release(&kmem.lock);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
فراغ *
kalloc(فراغ)
{
  بنية run *r;

  acquire(&kmem.lock);
  r = kmem.freelist;
  אם (r)
    kmem.freelist = r->next;
  release(&kmem.lock);

  אם (r)
    memset((حرف *)r, 5, PGSIZE); // fill with junk
  החזר (فراغ *)r;
}
