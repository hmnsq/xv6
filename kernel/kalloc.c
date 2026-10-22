// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#అన్తర్భావ "types.h"
#అన్తర్భావ "param.h"
#అన్తర్భావ "memlayout.h"
#అన్తర్భావ "spinlock.h"
#అన్తర్భావ "riscv.h"
#అన్తర్భావ "defs.h"

శూన్య freerange(శూన్య *pa_ప్రారంభం, శూన్య *pa_చివర);

బాహ్య అక్షరం చివర[]; // first address after kernel.
                   // defined by kernel.ld.

నిర్మాణం నిరంతరం {
  నిర్మాణం నిరంతరం *తదుపరి;
};

నిర్మాణం {
  నిర్మాణం spinlock తాళం;
  నిర్మాణం నిరంతరం *freelist;
} kmem;

శూన్య
kinit()
{
  initlock(&kmem.తాళం, "kmem");
  freerange(చివర, (శూన్య *)PHYSTOP);
}

శూన్య
freerange(శూన్య *pa_ప్రారంభం, శూన్య *pa_చివర)
{
  అక్షరం *p;
  p = (అక్షరం *)PGROUNDUP((uint64)pa_ప్రారంభం);
  కొరకు (; p + PGSIZE <= (అక్షరం *)pa_చివర; p += PGSIZE)
    kfree(p);
}

// Free the page of physical memory pointed at by pa,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
శూన్య
kfree(శూన్య *pa)
{
  నిర్మాణం నిరంతరం *r;

  ఒకవేళ (((uint64)pa % PGSIZE) != 0 || (అక్షరం *)pa < చివర || (uint64)pa >= PHYSTOP)
    panic("kfree");

  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);

  r = (నిర్మాణం నిరంతరం *)pa;

  acquire(&kmem.తాళం);
  r->తదుపరి = kmem.freelist;
  kmem.freelist = r;
  release(&kmem.తాళం);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
శూన్య *
kalloc(శూన్య)
{
  నిర్మాణం నిరంతరం *r;

  acquire(&kmem.తాళం);
  r = kmem.freelist;
  ఒకవేళ (r)
    kmem.freelist = r->తదుపరి;
  release(&kmem.తాళం);

  ఒకవేళ (r)
    memset((అక్షరం *)r, 5, PGSIZE); // fill with junk
  తిరిగి (శూన్య *)r;
}
