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
  עבור (p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
    אם (p >= p->s.ptr && (bp > p || bp < p->s.ptr))
      اكسر;
  אם (bp + bp->s.size == p->s.ptr) {
    bp->s.size += p->s.ptr->s.size;
    bp->s.ptr = p->s.ptr->s.ptr;
  } אחרת
    bp->s.ptr = p->s.ptr;
  אם (p + p->s.size == bp) {
    p->s.size += bp->s.size;
    p->s.ptr = bp->s.ptr;
  } אחרת
    p->s.ptr = bp;
  freep = p;
}

ساكن Header *
morecore(uint nu)
{
  حرف *p;
  Header *hp;

  אם (nu < 4096)
    nu = 4096;
  p = sbrk(nu * حجم(Header));
  אם (p == SBRK_ERROR)
    החזר 0;
  hp = (Header *)p;
  hp->s.size = nu;
  حرر((فراغ *)(hp + 1));
  החזר freep;
}

فراغ *
malloc(uint nbytes)
{
  Header *p, *prevp;
  uint nunits;

  nunits = (nbytes + حجم(Header) - 1) / حجم(Header) + 1;
  אם ((prevp = freep) == 0) {
    base.s.ptr = freep = prevp = &base;
    base.s.size = 0;
  }
  עבור (p = prevp->s.ptr;; prevp = p, p = p->s.ptr) {
    אם (p->s.size >= nunits) {
      אם (p->s.size == nunits)
        prevp->s.ptr = p->s.ptr;
      אחרת {
        p->s.size -= nunits;
        p += p->s.size;
        p->s.size = nunits;
      }
      freep = prevp;
      החזר (فراغ *)(p + 1);
    }
    אם (p == freep)
      אם ((p = morecore(nunits)) == 0)
        החזר 0;
  }
}
