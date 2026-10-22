// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#അന്തര്ഭാവ "types.h"
#അന്തര്ഭാവ "param.h"
#അന്തര്ഭാവ "memlayout.h"
#അന്തര്ഭാവ "spinlock.h"
#അന്തര്ഭാവ "riscv.h"
#അന്തര്ഭാവ "defs.h"

ശൂന്യം freerange(ശൂന്യം *pa_start, ശൂന്യം *pa_end);

ബാഹ്യ അക്ഷരം end[]; // first address after kernel.
                   // defined by kernel.ld.

ഘടന run {
  ഘടന run *next;
};

ഘടന {
  ഘടന spinlock lock;
  ഘടന run *freelist;
} kmem;

ശൂന്യം
kinit()
{
  initlock(&kmem.lock, "kmem");
  freerange(end, (ശൂന്യം *)PHYSTOP);
}

ശൂന്യം
freerange(ശൂന്യം *pa_start, ശൂന്യം *pa_end)
{
  അക്ഷരം *p;
  p = (അക്ഷരം *)PGROUNDUP((uint64)pa_start);
  വേണ്ടി (; p + PGSIZE <= (അക്ഷരം *)pa_end; p += PGSIZE)
    kfree(p);
}

// Free the page of physical memory pointed at by pa,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
ശൂന്യം
kfree(ശൂന്യം *pa)
{
  ഘടന run *r;

  എങ്കിൽ (((uint64)pa % PGSIZE) != 0 || (അക്ഷരം *)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);

  r = (ഘടന run *)pa;

  acquire(&kmem.lock);
  r->next = kmem.freelist;
  kmem.freelist = r;
  release(&kmem.lock);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
ശൂന്യം *
kalloc(ശൂന്യം)
{
  ഘടന run *r;

  acquire(&kmem.lock);
  r = kmem.freelist;
  എങ്കിൽ (r)
    kmem.freelist = r->next;
  release(&kmem.lock);

  എങ്കിൽ (r)
    memset((അക്ഷരം *)r, 5, PGSIZE); // fill with junk
  തിരികെ (ശൂന്യം *)r;
}
