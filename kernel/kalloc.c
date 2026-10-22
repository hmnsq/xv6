// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#ಅನ್ತರ್ಭಾವ "types.h"
#ಅನ್ತರ್ಭಾವ "param.h"
#ಅನ್ತರ್ಭಾವ "memlayout.h"
#ಅನ್ತರ್ಭಾವ "spinlock.h"
#ಅನ್ತರ್ಭಾವ "riscv.h"
#ಅನ್ತರ್ಭಾವ "defs.h"

ಶೂನ್ಯ freerange(ಶೂನ್ಯ *pa_start, ಶೂನ್ಯ *pa_end);

ಬಾಹ್ಯ ವರ್ಣ end[]; // first address after kernel.
                   // defined by kernel.ld.

ಸಂರಚನಾ run {
  ಸಂರಚನಾ run *next;
};

ಸಂರಚನಾ {
  ಸಂರಚನಾ spinlock lock;
  ಸಂರಚನಾ run *freelist;
} kmem;

ಶೂನ್ಯ
kinit()
{
  initlock(&kmem.lock, "kmem");
  freerange(end, (ಶೂನ್ಯ *)PHYSTOP);
}

ಶೂನ್ಯ
freerange(ಶೂನ್ಯ *pa_start, ಶೂನ್ಯ *pa_end)
{
  ವರ್ಣ *p;
  p = (ವರ್ಣ *)PGROUNDUP((uint64)pa_start);
  ಪ್ರತಿ (; p + PGSIZE <= (ವರ್ಣ *)pa_end; p += PGSIZE)
    kfree(p);
}

// Free the page of physical memory pointed at by pa,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
ಶೂನ್ಯ
kfree(ಶೂನ್ಯ *pa)
{
  ಸಂರಚನಾ run *r;

  ಯದಿ (((uint64)pa % PGSIZE) != 0 || (ವರ್ಣ *)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);

  r = (ಸಂರಚನಾ run *)pa;

  acquire(&kmem.lock);
  r->next = kmem.freelist;
  kmem.freelist = r;
  release(&kmem.lock);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
ಶೂನ್ಯ *
kalloc(ಶೂನ್ಯ)
{
  ಸಂರಚನಾ run *r;

  acquire(&kmem.lock);
  r = kmem.freelist;
  ಯದಿ (r)
    kmem.freelist = r->next;
  release(&kmem.lock);

  ಯದಿ (r)
    memset((ವರ್ಣ *)r, 5, PGSIZE); // fill with junk
  ಪ್ರತ್ಯಾಗಮ (ಶೂನ್ಯ *)r;
}
