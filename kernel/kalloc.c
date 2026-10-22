// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#अन्तर्भाव "types.h"
#अन्तर्भाव "param.h"
#अन्तर्भाव "memlayout.h"
#अन्तर्भाव "spinlock.h"
#अन्तर्भाव "riscv.h"
#अन्तर्भाव "defs.h"

शून्य freerange(शून्य *pa_start, शून्य *pa_अन्तः);

बाह्य वर्ण अन्तः_2[]; // first address after kernel.
                   // defined by kernel.ld.

संरचना सततम् {
  संरचना सततम् *अग्र;
};

संरचना {
  संरचना spinlock तालकम्;
  संरचना सततम् *freelist;
} kmem;

शून्य
kinit()
{
  initlock(&kmem.तालकम्, "kmem");
  freerange(अन्तः_2, (शून्य *)PHYSTOP);
}

शून्य
freerange(शून्य *pa_start, शून्य *pa_अन्तः)
{
  वर्ण *p;
  p = (वर्ण *)PGROUNDUP((uint64)pa_start);
  प्रति (; p + PGSIZE <= (वर्ण *)pa_अन्तः; p += PGSIZE)
    kfree(p);
}

// Free the page of physical memory pointed at by pa,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
शून्य
kfree(शून्य *pa)
{
  संरचना सततम् *r;

  यदि (((uint64)pa % PGSIZE) != 0 || (वर्ण *)pa < अन्तः_2 || (uint64)pa >= PHYSTOP)
    panic("kfree");

  // Fill with junk to catch dangling refs.
  स्मृतिं_पूरय(pa, 1, PGSIZE);

  r = (संरचना सततम् *)pa;

  acquire(&kmem.तालकम्);
  r->अग्र = kmem.freelist;
  kmem.freelist = r;
  release(&kmem.तालकम्);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
शून्य *
kalloc(शून्य)
{
  संरचना सततम् *r;

  acquire(&kmem.तालकम्);
  r = kmem.freelist;
  यदि (r)
    kmem.freelist = r->अग्र;
  release(&kmem.तालकम्);

  यदि (r)
    स्मृतिं_पूरय((वर्ण *)r, 5, PGSIZE); // fill with junk
  प्रत्यागम (शून्य *)r;
}
