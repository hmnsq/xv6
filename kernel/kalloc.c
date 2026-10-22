// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#অন্তর্ভাব "types.h"
#অন্তর্ভাব "param.h"
#অন্তর্ভাব "memlayout.h"
#অন্তর্ভাব "spinlock.h"
#অন্তর্ভাব "riscv.h"
#অন্তর্ভাব "defs.h"

শূন্য freerange(শূন্য *pa_start, শূন্য *pa_end);

বাহ্যিক অক্ষর end[]; // first address after kernel.
                   // defined by kernel.ld.

কাঠামো run {
  কাঠামো run *পরবর্তী;
};

কাঠামো {
  কাঠামো spinlock lock;
  কাঠামো run *freelist;
} kmem;

শূন্য
kinit()
{
  initlock(&kmem.lock, "kmem");
  freerange(end, (শূন্য *)PHYSTOP);
}

শূন্য
freerange(শূন্য *pa_start, শূন্য *pa_end)
{
  অক্ষর *p;
  p = (অক্ষর *)PGROUNDUP((uint64)pa_start);
  জন্য (; p + PGSIZE <= (অক্ষর *)pa_end; p += PGSIZE)
    kfree(p);
}

// Free the page of physical memory pointed at by pa,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
শূন্য
kfree(শূন্য *pa)
{
  কাঠামো run *r;

  যদি (((uint64)pa % PGSIZE) != 0 || (অক্ষর *)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);

  r = (কাঠামো run *)pa;

  acquire(&kmem.lock);
  r->পরবর্তী = kmem.freelist;
  kmem.freelist = r;
  release(&kmem.lock);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
শূন্য *
kalloc(শূন্য)
{
  কাঠামো run *r;

  acquire(&kmem.lock);
  r = kmem.freelist;
  যদি (r)
    kmem.freelist = r->পরবর্তী;
  release(&kmem.lock);

  যদি (r)
    memset((অক্ষর *)r, 5, PGSIZE); // fill with junk
  ফেরত (শূন্য *)r;
}
