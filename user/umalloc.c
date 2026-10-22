#অন্তর্ভাব "kernel/types.h"
#অন্তর্ভাব "kernel/stat.h"
#অন্তর্ভাব "user/user.h"
#অন্তর্ভাব "kernel/param.h"

// Memory allocator by Kernighan and Ritchie,
// The C programming Language, 2nd ed.  Section 8.7.

প্রকারনির্দেশ long Align;

সঙ্ঘ header {
  সংরচনা {
    সঙ্ঘ header *ptr;
    uint size;
  } s;
  Align x;
};

প্রকারনির্দেশ সঙ্ঘ header Header;

স্থির Header base;
স্থির Header *freep;

শূন্য
মুক্ত(শূন্য *ap)
{
  Header *bp, *p;

  bp = (Header *)ap - 1;
  প্রতি (p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
    যদি (p >= p->s.ptr && (bp > p || bp < p->s.ptr))
      ভঙ্গ;
  যদি (bp + bp->s.size == p->s.ptr) {
    bp->s.size += p->s.ptr->s.size;
    bp->s.ptr = p->s.ptr->s.ptr;
  } অন্যথা
    bp->s.ptr = p->s.ptr;
  যদি (p + p->s.size == bp) {
    p->s.size += bp->s.size;
    p->s.ptr = bp->s.ptr;
  } অন্যথা
    p->s.ptr = bp;
  freep = p;
}

স্থির Header *
morecore(uint nu)
{
  বর্ণ *p;
  Header *hp;

  যদি (nu < 4096)
    nu = 4096;
  p = sbrk(nu * পরিমাণ(Header));
  যদি (p == SBRK_ERROR)
    প্রত্যাগম 0;
  hp = (Header *)p;
  hp->s.size = nu;
  মুক্ত((শূন্য *)(hp + 1));
  প্রত্যাগম freep;
}

শূন্য *
malloc(uint nbytes)
{
  Header *p, *prevp;
  uint nunits;

  nunits = (nbytes + পরিমাণ(Header) - 1) / পরিমাণ(Header) + 1;
  যদি ((prevp = freep) == 0) {
    base.s.ptr = freep = prevp = &base;
    base.s.size = 0;
  }
  প্রতি (p = prevp->s.ptr;; prevp = p, p = p->s.ptr) {
    যদি (p->s.size >= nunits) {
      যদি (p->s.size == nunits)
        prevp->s.ptr = p->s.ptr;
      অন্যথা {
        p->s.size -= nunits;
        p += p->s.size;
        p->s.size = nunits;
      }
      freep = prevp;
      প্রত্যাগম (শূন্য *)(p + 1);
    }
    যদি (p == freep)
      যদি ((p = morecore(nunits)) == 0)
        প্রত্যাগম 0;
  }
}
