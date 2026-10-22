#ضمن "kernel/types.h"
#ضمن "kernel/stat.h"
#ضمن "user/user.h"
#ضمن "kernel/param.h"

// Memory allocator by Kernighan and Ritchie,
// The C programming Language, 2nd ed.  Section 8.7.

تعريفنوع ڊگهو Align;

اتحاد سرو {
  بنية {
    اتحاد سرو *ptr;
    uint سائيز;
  } s;
  Align x;
};

تعريفنوع اتحاد سرو سرو_2;

ساكن سرو_2 بنياد;
ساكن سرو_2 *freep;

فراغ
حرر(فراغ *ap)
{
  سرو_2 *bp, *p;

  bp = (سرو_2 *)ap - 1;
  لكل (p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
    إذا (p >= p->s.ptr && (bp > p || bp < p->s.ptr))
      اكسر;
  إذا (bp + bp->s.سائيز == p->s.ptr) {
    bp->s.سائيز += p->s.ptr->s.سائيز;
    bp->s.ptr = p->s.ptr->s.ptr;
  } وإلا
    bp->s.ptr = p->s.ptr;
  إذا (p + p->s.سائيز == bp) {
    p->s.سائيز += bp->s.سائيز;
    p->s.ptr = bp->s.ptr;
  } وإلا
    p->s.ptr = bp;
  freep = p;
}

ساكن سرو_2 *
morecore(uint nu)
{
  حرف *p;
  سرو_2 *hp;

  إذا (nu < 4096)
    nu = 4096;
  p = sbrk(nu * حجم(سرو_2));
  إذا (p == SBRK_ERROR)
    أرجع 0;
  hp = (سرو_2 *)p;
  hp->s.سائيز = nu;
  حرر((فراغ *)(hp + 1));
  أرجع freep;
}

فراغ *
يادگيري_ڏيو(uint nbytes)
{
  سرو_2 *p, *prevp;
  uint nunits;

  nunits = (nbytes + حجم(سرو_2) - 1) / حجم(سرو_2) + 1;
  إذا ((prevp = freep) == 0) {
    بنياد.s.ptr = freep = prevp = &بنياد;
    بنياد.s.سائيز = 0;
  }
  لكل (p = prevp->s.ptr;; prevp = p, p = p->s.ptr) {
    إذا (p->s.سائيز >= nunits) {
      إذا (p->s.سائيز == nunits)
        prevp->s.ptr = p->s.ptr;
      وإلا {
        p->s.سائيز -= nunits;
        p += p->s.سائيز;
        p->s.سائيز = nunits;
      }
      freep = prevp;
      أرجع (فراغ *)(p + 1);
    }
    إذا (p == freep)
      إذا ((p = morecore(nunits)) == 0)
        أرجع 0;
  }
}
