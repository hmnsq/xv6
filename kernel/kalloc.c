// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#अन्तर्भाव "types.h"
#अन्तर्भाव "param.h"
#अन्तर्भाव "memlayout.h"
#अन्तर्भाव "spinlock.h"
#अन्तर्भाव "riscv.h"
#अन्तर्भाव "defs.h"

शून्य freerange(शून्य *pa_start, शून्य *pa_end);

बाह्य वर्ण end[]; // first address after kernel.
                   // defined by kernel.ld.

संरचना run {
  संरचना run *next;
};

संरचना {
  संरचना spinlock lock;
  संरचना run *freelist;
} kmem;

शून्य
kinit()
{
  initlock(&kmem.lock, "kmem");
  freerange(end, (शून्य *)PHYSTOP);
}

शून्य
freerange(शून्य *pa_start, शून्य *pa_end)
{
  वर्ण *p;
  p = (वर्ण *)PGROUNDUP((uint64)pa_start);
  प्रति (; p + PGSIZE <= (वर्ण *)pa_end; p += PGSIZE)
    kfree(p);
}

// Free the page of physical memory pointed at by pa,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
शून्य
kfree(शून्य *pa)
{
  संरचना run *r;

  जइ (((uint64)pa % PGSIZE) != 0 || (वर्ण *)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);

  r = (संरचना run *)pa;

  acquire(&kmem.lock);
  r->next = kmem.freelist;
  kmem.freelist = r;
  release(&kmem.lock);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
शून्य *
kalloc(शून्य)
{
  संरचना run *r;

  acquire(&kmem.lock);
  r = kmem.freelist;
  जइ (r)
    kmem.freelist = r->next;
  release(&kmem.lock);

  जइ (r)
    memset((वर्ण *)r, 5, PGSIZE); // fill with junk
  प्रत्यागम (शून्य *)r;
}
