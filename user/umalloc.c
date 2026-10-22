#ضمن "kernel/types.h"
#ضمن "kernel/stat.h"
#ضمن "user/user.h"
#ضمن "kernel/param.h"

// Memory allocator by Kernighan and Ritchie,
// The C programming Language, 2nd ed.  Section 8.7.

تعريفنوع اوږد Align;

اتحاد سرلیک {
  بنية {
    اتحاد سرلیک *ptr;
    uint اندازه;
  } s;
  Align x;
};

تعريفنوع اتحاد سرلیک سرلیک_2;

ساكن سرلیک_2 بنسټ;
ساكن سرلیک_2 *freep;

فراغ
حرر(فراغ *ap)
{
  سرلیک_2 *bp, *p;

  bp = (سرلیک_2 *)ap - 1;
  لكل (p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
    إذا (p >= p->s.ptr && (bp > p || bp < p->s.ptr))
      اكسر;
  إذا (bp + bp->s.اندازه == p->s.ptr) {
    bp->s.اندازه += p->s.ptr->s.اندازه;
    bp->s.ptr = p->s.ptr->s.ptr;
  } وإلا
    bp->s.ptr = p->s.ptr;
  إذا (p + p->s.اندازه == bp) {
    p->s.اندازه += bp->s.اندازه;
    p->s.ptr = bp->s.ptr;
  } وإلا
    p->s.ptr = bp;
  freep = p;
}

ساكن سرلیک_2 *
morecore(uint nu)
{
  حرف *p;
  سرلیک_2 *hp;

  إذا (nu < 4096)
    nu = 4096;
  p = sbrk(nu * حجم(سرلیک_2));
  إذا (p == SBRK_ERROR)
    أرجع 0;
  hp = (سرلیک_2 *)p;
  hp->s.اندازه = nu;
  حرر((فراغ *)(hp + 1));
  أرجع freep;
}

فراغ *
حافظه_ورکړه(uint nbytes)
{
  سرلیک_2 *p, *prevp;
  uint nunits;

  nunits = (nbytes + حجم(سرلیک_2) - 1) / حجم(سرلیک_2) + 1;
  إذا ((prevp = freep) == 0) {
    بنسټ.s.ptr = freep = prevp = &بنسټ;
    بنسټ.s.اندازه = 0;
  }
  لكل (p = prevp->s.ptr;; prevp = p, p = p->s.ptr) {
    إذا (p->s.اندازه >= nunits) {
      إذا (p->s.اندازه == nunits)
        prevp->s.ptr = p->s.ptr;
      وإلا {
        p->s.اندازه -= nunits;
        p += p->s.اندازه;
        p->s.اندازه = nunits;
      }
      freep = prevp;
      أرجع (فراغ *)(p + 1);
    }
    إذا (p == freep)
      إذا ((p = morecore(nunits)) == 0)
        أرجع 0;
  }
}
