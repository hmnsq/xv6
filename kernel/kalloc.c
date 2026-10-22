// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#ضمن "types.h"
#ضمن "param.h"
#ضمن "memlayout.h"
#ضمن "spinlock.h"
#ضمن "riscv.h"
#ضمن "defs.h"

فراغ freerange(فراغ *pa_پیل, فراغ *pa_پای_ټکی);

خارجي حرف پای_ټکی[]; // first address after kernel.
                   // defined by kernel.ld.

بنية پرلپسې {
  بنية پرلپسې *بل;
};

بنية {
  بنية spinlock قلف;
  بنية پرلپسې *freelist;
} kmem;

فراغ
kinit()
{
  initlock(&kmem.قلف, "kmem");
  freerange(پای_ټکی, (فراغ *)PHYSTOP);
}

فراغ
freerange(فراغ *pa_پیل, فراغ *pa_پای_ټکی)
{
  حرف *p;
  p = (حرف *)PGROUNDUP((uint64)pa_پیل);
  لكل (; p + PGSIZE <= (حرف *)pa_پای_ټکی; p += PGSIZE)
    kfree(p);
}

// Free the page of physical memory pointed at by pa,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
فراغ
kfree(فراغ *pa)
{
  بنية پرلپسې *r;

  إذا (((uint64)pa % PGSIZE) != 0 || (حرف *)pa < پای_ټکی || (uint64)pa >= PHYSTOP)
    panic("kfree");

  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);

  r = (بنية پرلپسې *)pa;

  acquire(&kmem.قلف);
  r->بل = kmem.freelist;
  kmem.freelist = r;
  release(&kmem.قلف);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
فراغ *
kalloc(فراغ)
{
  بنية پرلپسې *r;

  acquire(&kmem.قلف);
  r = kmem.freelist;
  إذا (r)
    kmem.freelist = r->بل;
  release(&kmem.قلف);

  إذا (r)
    memset((حرف *)r, 5, PGSIZE); // fill with junk
  أرجع (فراغ *)r;
}
