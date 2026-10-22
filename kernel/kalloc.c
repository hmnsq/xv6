// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#ଅନ୍ତର୍ଭାଵ "types.h"
#ଅନ୍ତର୍ଭାଵ "param.h"
#ଅନ୍ତର୍ଭାଵ "memlayout.h"
#ଅନ୍ତର୍ଭାଵ "spinlock.h"
#ଅନ୍ତର୍ଭାଵ "riscv.h"
#ଅନ୍ତର୍ଭାଵ "defs.h"

ଶୂନ୍ୟ freerange(ଶୂନ୍ୟ *pa_start, ଶୂନ୍ୟ *pa_end);

ବାହ୍ୟ ଅକ୍ଷର end[]; // first address after kernel.
                   // defined by kernel.ld.

ସଂରଚନା run {
  ସଂରଚନା run *next;
};

ସଂରଚନା {
  ସଂରଚନା spinlock lock;
  ସଂରଚନା run *freelist;
} kmem;

ଶୂନ୍ୟ
kinit()
{
  initlock(&kmem.lock, "kmem");
  freerange(end, (ଶୂନ୍ୟ *)PHYSTOP);
}

ଶୂନ୍ୟ
freerange(ଶୂନ୍ୟ *pa_start, ଶୂନ୍ୟ *pa_end)
{
  ଅକ୍ଷର *p;
  p = (ଅକ୍ଷର *)PGROUNDUP((uint64)pa_start);
  ପାଇଁ (; p + PGSIZE <= (ଅକ୍ଷର *)pa_end; p += PGSIZE)
    kfree(p);
}

// Free the page of physical memory pointed at by pa,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
ଶୂନ୍ୟ
kfree(ଶୂନ୍ୟ *pa)
{
  ସଂରଚନା run *r;

  ଯଦି (((uint64)pa % PGSIZE) != 0 || (ଅକ୍ଷର *)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);

  r = (ସଂରଚନା run *)pa;

  acquire(&kmem.lock);
  r->next = kmem.freelist;
  kmem.freelist = r;
  release(&kmem.lock);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
ଶୂନ୍ୟ *
kalloc(ଶୂନ୍ୟ)
{
  ସଂରଚନା run *r;

  acquire(&kmem.lock);
  r = kmem.freelist;
  ଯଦି (r)
    kmem.freelist = r->next;
  release(&kmem.lock);

  ଯଦି (r)
    memset((ଅକ୍ଷର *)r, 5, PGSIZE); // fill with junk
  ଫେରନ୍ତୁ (ଶୂନ୍ୟ *)r;
}
