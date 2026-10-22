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

बाह्य अक्षर end[]; // first address after kernel.
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
  अक्षर *p;
  p = (अक्षर *)PGROUNDUP((uint64)pa_start);
  साठी (; p + PGSIZE <= (अक्षर *)pa_end; p += PGSIZE)
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

  जर (((uint64)pa % PGSIZE) != 0 || (अक्षर *)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  // Fill with junk to catch dangling refs.
  स_रखो(pa, 1, PGSIZE);

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
  जर (r)
    kmem.freelist = r->next;
  release(&kmem.lock);

  जर (r)
    स_रखो((अक्षर *)r, 5, PGSIZE); // fill with junk
  परत (शून्य *)r;
}
