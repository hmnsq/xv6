#অন্তর্ভাব "kernel/types.h"
#অন্তর্ভাব "kernel/stat.h"
#অন্তর্ভাব "user/user.h"
#অন্তর্ভাব "kernel/param.h"

// Memory allocator by Kernighan and Ritchie,
// The C programming Language, 2nd ed.  Section 8.7.

প্রকারলেখ long Align;

সংঘ header {
  কাঠামো {
    সংঘ header *ptr;
    uint আকার_2;
  } s;
  Align x;
};

প্রকারলেখ সংঘ header Header;

স্থির Header base;
স্থির Header *freep;

শূন্য
মুক্ত(শূন্য *ap)
{
  Header *bp, *p;

  bp = (Header *)ap - 1;
  জন্য (p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
    যদি (p >= p->s.ptr && (bp > p || bp < p->s.ptr))
      বিরতি;
  যদি (bp + bp->s.আকার_2 == p->s.ptr) {
    bp->s.আকার_2 += p->s.ptr->s.আকার_2;
    bp->s.ptr = p->s.ptr->s.ptr;
  } অন্যথা
    bp->s.ptr = p->s.ptr;
  যদি (p + p->s.আকার_2 == bp) {
    p->s.আকার_2 += bp->s.আকার_2;
    p->s.ptr = bp->s.ptr;
  } অন্যথা
    p->s.ptr = bp;
  freep = p;
}

স্থির Header *
morecore(uint nu)
{
  অক্ষর *p;
  Header *hp;

  যদি (nu < 4096)
    nu = 4096;
  p = sbrk(nu * আকার(Header));
  যদি (p == SBRK_ERROR)
    ফেরত 0;
  hp = (Header *)p;
  hp->s.আকার_2 = nu;
  মুক্ত((শূন্য *)(hp + 1));
  ফেরত freep;
}

শূন্য *
malloc(uint nbytes)
{
  Header *p, *prevp;
  uint nunits;

  nunits = (nbytes + আকার(Header) - 1) / আকার(Header) + 1;
  যদি ((prevp = freep) == 0) {
    base.s.ptr = freep = prevp = &base;
    base.s.আকার_2 = 0;
  }
  জন্য (p = prevp->s.ptr;; prevp = p, p = p->s.ptr) {
    যদি (p->s.আকার_2 >= nunits) {
      যদি (p->s.আকার_2 == nunits)
        prevp->s.ptr = p->s.ptr;
      অন্যথা {
        p->s.আকার_2 -= nunits;
        p += p->s.আকার_2;
        p->s.আকার_2 = nunits;
      }
      freep = prevp;
      ফেরত (শূন্য *)(p + 1);
    }
    যদি (p == freep)
      যদি ((p = morecore(nunits)) == 0)
        ফেরত 0;
  }
}
