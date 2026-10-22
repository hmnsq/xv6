// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#அந்தர்பாவ "types.h"
#அந்தர்பாவ "param.h"
#அந்தர்பாவ "memlayout.h"
#அந்தர்பாவ "spinlock.h"
#அந்தர்பாவ "riscv.h"
#அந்தர்பாவ "defs.h"

வெறுமை freerange(வெறுமை *pa_start, வெறுமை *pa_end);

வெளி எழுத்து end[]; // first address after kernel.
                   // defined by kernel.ld.

கட்டமைப்பு run {
  கட்டமைப்பு run *next;
};

கட்டமைப்பு {
  கட்டமைப்பு spinlock lock;
  கட்டமைப்பு run *freelist;
} kmem;

வெறுமை
kinit()
{
  initlock(&kmem.lock, "kmem");
  freerange(end, (வெறுமை *)PHYSTOP);
}

வெறுமை
freerange(வெறுமை *pa_start, வெறுமை *pa_end)
{
  எழுத்து *p;
  p = (எழுத்து *)PGROUNDUP((uint64)pa_start);
  க்கு (; p + PGSIZE <= (எழுத்து *)pa_end; p += PGSIZE)
    kfree(p);
}

// Free the page of physical memory pointed at by pa,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
வெறுமை
kfree(வெறுமை *pa)
{
  கட்டமைப்பு run *r;

  எனில் (((uint64)pa % PGSIZE) != 0 || (எழுத்து *)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);

  r = (கட்டமைப்பு run *)pa;

  acquire(&kmem.lock);
  r->next = kmem.freelist;
  kmem.freelist = r;
  release(&kmem.lock);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
வெறுமை *
kalloc(வெறுமை)
{
  கட்டமைப்பு run *r;

  acquire(&kmem.lock);
  r = kmem.freelist;
  எனில் (r)
    kmem.freelist = r->next;
  release(&kmem.lock);

  எனில் (r)
    memset((எழுத்து *)r, 5, PGSIZE); // fill with junk
  திரும்பு (வெறுமை *)r;
}
