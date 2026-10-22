#ضمن "kernel/types.h"
#ضمن "kernel/stat.h"
#ضمن "user/user.h"
#ضمن "kernel/param.h"

// Memory allocator by Kernighan and Ritchie,
// The C programming Language, 2nd ed.  Section 8.7.

تعريفنوع long Align;

اتحاد header {
  بنية {
    اتحاد header *ptr;
    uint size;
  } s;
  Align x;
};

تعريفنوع اتحاد header Header;

ساكن Header base;
ساكن Header *freep;

فراغ
حرر(فراغ *ap)
{
  Header *bp, *p;

  bp = (Header *)ap - 1;
  لكل (p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
    𐤀𐤌 (p >= p->s.ptr && (bp > p || bp < p->s.ptr))
      اكسر;
  𐤀𐤌 (bp + bp->s.size == p->s.ptr) {
    bp->s.size += p->s.ptr->s.size;
    bp->s.ptr = p->s.ptr->s.ptr;
  } وإلا
    bp->s.ptr = p->s.ptr;
  𐤀𐤌 (p + p->s.size == bp) {
    p->s.size += bp->s.size;
    p->s.ptr = bp->s.ptr;
  } وإلا
    p->s.ptr = bp;
  freep = p;
}

ساكن Header *
morecore(uint nu)
{
  حرف *p;
  Header *hp;

  𐤀𐤌 (nu < 4096)
    nu = 4096;
  p = sbrk(nu * حجم(Header));
  𐤀𐤌 (p == SBRK_ERROR)
    أرجع 0;
  hp = (Header *)p;
  hp->s.size = nu;
  حرر((فراغ *)(hp + 1));
  أرجع freep;
}

فراغ *
malloc(uint nbytes)
{
  Header *p, *prevp;
  uint nunits;

  nunits = (nbytes + حجم(Header) - 1) / حجم(Header) + 1;
  𐤀𐤌 ((prevp = freep) == 0) {
    base.s.ptr = freep = prevp = &base;
    base.s.size = 0;
  }
  لكل (p = prevp->s.ptr;; prevp = p, p = p->s.ptr) {
    𐤀𐤌 (p->s.size >= nunits) {
      𐤀𐤌 (p->s.size == nunits)
        prevp->s.ptr = p->s.ptr;
      وإلا {
        p->s.size -= nunits;
        p += p->s.size;
        p->s.size = nunits;
      }
      freep = prevp;
      أرجع (فراغ *)(p + 1);
    }
    𐤀𐤌 (p == freep)
      𐤀𐤌 ((p = morecore(nunits)) == 0)
        أرجع 0;
  }
}
