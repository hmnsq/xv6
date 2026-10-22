// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#ಅನ್ತರ್ಭಾವ "types.h"
#ಅನ್ತರ್ಭಾವ "param.h"
#ಅನ್ತರ್ಭಾವ "memlayout.h"
#ಅನ್ತರ್ಭಾವ "spinlock.h"
#ಅನ್ತರ್ಭಾವ "riscv.h"
#ಅನ್ತರ್ಭಾವ "defs.h"

ಶೂನ್ಯ freerange(ಶೂನ್ಯ *pa_ಆರಂಭ, ಶೂನ್ಯ *pa_ಕೊನೆ);

ಬಾಹ್ಯ ಅಕ್ಷರ ಕೊನೆ[]; // first address after kernel.
                   // defined by kernel.ld.

ರಚನೆ ಸತತ {
  ರಚನೆ ಸತತ *ಮುಂದಿನ;
};

ರಚನೆ {
  ರಚನೆ spinlock ಬೀಗ;
  ರಚನೆ ಸತತ *freelist;
} kmem;

ಶೂನ್ಯ
kinit()
{
  initlock(&kmem.ಬೀಗ, "kmem");
  freerange(ಕೊನೆ, (ಶೂನ್ಯ *)PHYSTOP);
}

ಶೂನ್ಯ
freerange(ಶೂನ್ಯ *pa_ಆರಂಭ, ಶೂನ್ಯ *pa_ಕೊನೆ)
{
  ಅಕ್ಷರ *p;
  p = (ಅಕ್ಷರ *)PGROUNDUP((uint64)pa_ಆರಂಭ);
  ಗಾಗಿ (; p + PGSIZE <= (ಅಕ್ಷರ *)pa_ಕೊನೆ; p += PGSIZE)
    kfree(p);
}

// Free the page of physical memory pointed at by pa,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
ಶೂನ್ಯ
kfree(ಶೂನ್ಯ *pa)
{
  ರಚನೆ ಸತತ *r;

  ಆದರೆ (((uint64)pa % PGSIZE) != 0 || (ಅಕ್ಷರ *)pa < ಕೊನೆ || (uint64)pa >= PHYSTOP)
    panic("kfree");

  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);

  r = (ರಚನೆ ಸತತ *)pa;

  acquire(&kmem.ಬೀಗ);
  r->ಮುಂದಿನ = kmem.freelist;
  kmem.freelist = r;
  release(&kmem.ಬೀಗ);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
ಶೂನ್ಯ *
kalloc(ಶೂನ್ಯ)
{
  ರಚನೆ ಸತತ *r;

  acquire(&kmem.ಬೀಗ);
  r = kmem.freelist;
  ಆದರೆ (r)
    kmem.freelist = r->ಮುಂದಿನ;
  release(&kmem.ಬೀಗ);

  ಆದರೆ (r)
    memset((ಅಕ್ಷರ *)r, 5, PGSIZE); // fill with junk
  ಹಿಂತಿರುಗಿ (ಶೂನ್ಯ *)r;
}
