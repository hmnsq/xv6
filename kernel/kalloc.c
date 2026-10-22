// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#समावेश "types.h"
#समावेश "param.h"
#समावेश "memlayout.h"
#समावेश "spinlock.h"
#समावेश "riscv.h"
#समावेश "defs.h"

व्योम freerange(व्योम *pa_start, व्योम *pa_end);

बाह्य अक्षर end[]; // first address after kernel.
                   // defined by kernel.ld.

काष्ठा run {
  काष्ठा run *अग्र;
};

काष्ठा {
  काष्ठा spinlock lock;
  काष्ठा run *freelist;
} kmem;

व्योम
kinit()
{
  initlock(&kmem.lock, "kmem");
  freerange(end, (व्योम *)PHYSTOP);
}

व्योम
freerange(व्योम *pa_start, व्योम *pa_end)
{
  अक्षर *p;
  p = (अक्षर *)PGROUNDUP((uint64)pa_start);
  क्रम (; p + PGSIZE <= (अक्षर *)pa_end; p += PGSIZE)
    kfree(p);
}

// Free the page of physical memory pointed at by pa,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
व्योम
kfree(व्योम *pa)
{
  काष्ठा run *r;

  अगर (((uint64)pa % PGSIZE) != 0 || (अक्षर *)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  // Fill with junk to catch dangling refs.
  स_रखो(pa, 1, PGSIZE);

  r = (काष्ठा run *)pa;

  acquire(&kmem.lock);
  r->अग्र = kmem.freelist;
  kmem.freelist = r;
  release(&kmem.lock);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
व्योम *
kalloc(व्योम)
{
  काष्ठा run *r;

  acquire(&kmem.lock);
  r = kmem.freelist;
  अगर (r)
    kmem.freelist = r->अग्र;
  release(&kmem.lock);

  अगर (r)
    स_रखो((अक्षर *)r, 5, PGSIZE); // fill with junk
  वापस (व्योम *)r;
}
