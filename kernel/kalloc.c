// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#ਅਨ੍ਤਰ੍ਭਾਵ "types.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "param.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "memlayout.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "spinlock.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "riscv.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "defs.h"

ਖਾਲੀ freerange(ਖਾਲੀ *pa_ਸ਼ੁਰੂ, ਖਾਲੀ *pa_ਸਿਰਾ);

ਬਾਹਰੀ ਅੱਖਰ ਸਿਰਾ[]; // first address after kernel.
                   // defined by kernel.ld.

ਬਣਤਰ ਲੜੀਵਾਰ {
  ਬਣਤਰ ਲੜੀਵਾਰ *ਅਗਲਾ;
};

ਬਣਤਰ {
  ਬਣਤਰ spinlock ਤਾਲਾ;
  ਬਣਤਰ ਲੜੀਵਾਰ *freelist;
} kmem;

ਖਾਲੀ
kinit()
{
  initlock(&kmem.ਤਾਲਾ, "kmem");
  freerange(ਸਿਰਾ, (ਖਾਲੀ *)PHYSTOP);
}

ਖਾਲੀ
freerange(ਖਾਲੀ *pa_ਸ਼ੁਰੂ, ਖਾਲੀ *pa_ਸਿਰਾ)
{
  ਅੱਖਰ *p;
  p = (ਅੱਖਰ *)PGROUNDUP((uint64)pa_ਸ਼ੁਰੂ);
  ਲਈ (; p + PGSIZE <= (ਅੱਖਰ *)pa_ਸਿਰਾ; p += PGSIZE)
    kfree(p);
}

// Free the page of physical memory pointed at by pa,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
ਖਾਲੀ
kfree(ਖਾਲੀ *pa)
{
  ਬਣਤਰ ਲੜੀਵਾਰ *r;

  ਜੇ (((uint64)pa % PGSIZE) != 0 || (ਅੱਖਰ *)pa < ਸਿਰਾ || (uint64)pa >= PHYSTOP)
    panic("kfree");

  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);

  r = (ਬਣਤਰ ਲੜੀਵਾਰ *)pa;

  acquire(&kmem.ਤਾਲਾ);
  r->ਅਗਲਾ = kmem.freelist;
  kmem.freelist = r;
  release(&kmem.ਤਾਲਾ);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
ਖਾਲੀ *
kalloc(ਖਾਲੀ)
{
  ਬਣਤਰ ਲੜੀਵਾਰ *r;

  acquire(&kmem.ਤਾਲਾ);
  r = kmem.freelist;
  ਜੇ (r)
    kmem.freelist = r->ਅਗਲਾ;
  release(&kmem.ਤਾਲਾ);

  ਜੇ (r)
    memset((ਅੱਖਰ *)r, 5, PGSIZE); // fill with junk
  ਵਾਪਸ (ਖਾਲੀ *)r;
}
