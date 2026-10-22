#ضمن "kernel/types.h"
#ضمن "kernel/stat.h"
#ضمن "user/user.h"
#ضمن "kernel/param.h"

// Memory allocator by Kernighan and Ritchie,
// The C programming Language, 2nd ed.  Section 8.7.

تعريفنوع لما Align;

اتحاد سرخی {
  بنية {
    اتحاد سرخی *ptr;
    uint ناپ;
  } s;
  Align x;
};

تعريفنوع اتحاد سرخی سرخی_2;

ساكن سرخی_2 ادھار;
ساكن سرخی_2 *freep;

فراغ
حرر(فراغ *ap)
{
  سرخی_2 *bp, *p;

  bp = (سرخی_2 *)ap - 1;
  لكل (p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
    إذا (p >= p->s.ptr && (bp > p || bp < p->s.ptr))
      اكسر;
  إذا (bp + bp->s.ناپ == p->s.ptr) {
    bp->s.ناپ += p->s.ptr->s.ناپ;
    bp->s.ptr = p->s.ptr->s.ptr;
  } وإلا
    bp->s.ptr = p->s.ptr;
  إذا (p + p->s.ناپ == bp) {
    p->s.ناپ += bp->s.ناپ;
    p->s.ptr = bp->s.ptr;
  } وإلا
    p->s.ptr = bp;
  freep = p;
}

ساكن سرخی_2 *
morecore(uint nu)
{
  حرف *p;
  سرخی_2 *hp;

  إذا (nu < 4096)
    nu = 4096;
  p = sbrk(nu * حجم(سرخی_2));
  إذا (p == SBRK_ERROR)
    أرجع 0;
  hp = (سرخی_2 *)p;
  hp->s.ناپ = nu;
  حرر((فراغ *)(hp + 1));
  أرجع freep;
}

فراغ *
حافظہ_دیو(uint nbytes)
{
  سرخی_2 *p, *prevp;
  uint nunits;

  nunits = (nbytes + حجم(سرخی_2) - 1) / حجم(سرخی_2) + 1;
  إذا ((prevp = freep) == 0) {
    ادھار.s.ptr = freep = prevp = &ادھار;
    ادھار.s.ناپ = 0;
  }
  لكل (p = prevp->s.ptr;; prevp = p, p = p->s.ptr) {
    إذا (p->s.ناپ >= nunits) {
      إذا (p->s.ناپ == nunits)
        prevp->s.ptr = p->s.ptr;
      وإلا {
        p->s.ناپ -= nunits;
        p += p->s.ناپ;
        p->s.ناپ = nunits;
      }
      freep = prevp;
      أرجع (فراغ *)(p + 1);
    }
    إذا (p == freep)
      إذا ((p = morecore(nunits)) == 0)
        أرجع 0;
  }
}
