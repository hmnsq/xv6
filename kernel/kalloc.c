// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#અન્તર્ભાવ "types.h"
#અન્તર્ભાવ "param.h"
#અન્તર્ભાવ "memlayout.h"
#અન્તર્ભાવ "spinlock.h"
#અન્તર્ભાવ "riscv.h"
#અન્તર્ભાવ "defs.h"

શૂન્ય freerange(શૂન્ય *pa_start, શૂન્ય *pa_end);

બાહ્ય અક્ષર end[]; // first address after kernel.
                   // defined by kernel.ld.

સંરચના run {
  સંરચના run *next;
};

સંરચના {
  સંરચના spinlock lock;
  સંરચના run *freelist;
} kmem;

શૂન્ય
kinit()
{
  initlock(&kmem.lock, "kmem");
  freerange(end, (શૂન્ય *)PHYSTOP);
}

શૂન્ય
freerange(શૂન્ય *pa_start, શૂન્ય *pa_end)
{
  અક્ષર *p;
  p = (અક્ષર *)PGROUNDUP((uint64)pa_start);
  માટે (; p + PGSIZE <= (અક્ષર *)pa_end; p += PGSIZE)
    kfree(p);
}

// Free the page of physical memory pointed at by pa,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
શૂન્ય
kfree(શૂન્ય *pa)
{
  સંરચના run *r;

  જો (((uint64)pa % PGSIZE) != 0 || (અક્ષર *)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);

  r = (સંરચના run *)pa;

  acquire(&kmem.lock);
  r->next = kmem.freelist;
  kmem.freelist = r;
  release(&kmem.lock);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
શૂન્ય *
kalloc(શૂન્ય)
{
  સંરચના run *r;

  acquire(&kmem.lock);
  r = kmem.freelist;
  જો (r)
    kmem.freelist = r->next;
  release(&kmem.lock);

  જો (r)
    memset((અક્ષર *)r, 5, PGSIZE); // fill with junk
  પાછા (શૂન્ય *)r;
}
