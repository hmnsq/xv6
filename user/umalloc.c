#شامل "kernel/types.h"
#شامل "kernel/stat.h"
#شامل "user/user.h"
#شامل "kernel/param.h"

// Memory allocator by Kernighan and Ritchie,
// The C programming Language, 2nd ed.  Section 8.7.

قسمتعریف طویل Align;

اتحاد سرخی {
  ساخت {
    اتحاد سرخی *ptr;
    uint سائز;
  } s;
  Align x;
};

قسمتعریف اتحاد سرخی سرخی_2;

ساکن سرخی_2 بنیاد;
ساکن سرخی_2 *freep;

خالی
آزاد(خالی *ap)
{
  سرخی_2 *bp, *p;

  bp = (سرخی_2 *)ap - 1;
  برائے (p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
    اگر (p >= p->s.ptr && (bp > p || bp < p->s.ptr))
      توڑو;
  اگر (bp + bp->s.سائز == p->s.ptr) {
    bp->s.سائز += p->s.ptr->s.سائز;
    bp->s.ptr = p->s.ptr->s.ptr;
  } ورنہ
    bp->s.ptr = p->s.ptr;
  اگر (p + p->s.سائز == bp) {
    p->s.سائز += bp->s.سائز;
    p->s.ptr = bp->s.ptr;
  } ورنہ
    p->s.ptr = bp;
  freep = p;
}

ساکن سرخی_2 *
morecore(uint nu)
{
  حرف *p;
  سرخی_2 *hp;

  اگر (nu < 4096)
    nu = 4096;
  p = sbrk(nu * حجم(سرخی_2));
  اگر (p == SBRK_ERROR)
    واپس 0;
  hp = (سرخی_2 *)p;
  hp->s.سائز = nu;
  آزاد((خالی *)(hp + 1));
  واپس freep;
}

خالی *
حافظہ_دو(uint nbytes)
{
  سرخی_2 *p, *prevp;
  uint nunits;

  nunits = (nbytes + حجم(سرخی_2) - 1) / حجم(سرخی_2) + 1;
  اگر ((prevp = freep) == 0) {
    بنیاد.s.ptr = freep = prevp = &بنیاد;
    بنیاد.s.سائز = 0;
  }
  برائے (p = prevp->s.ptr;; prevp = p, p = p->s.ptr) {
    اگر (p->s.سائز >= nunits) {
      اگر (p->s.سائز == nunits)
        prevp->s.ptr = p->s.ptr;
      ورنہ {
        p->s.سائز -= nunits;
        p += p->s.سائز;
        p->s.سائز = nunits;
      }
      freep = prevp;
      واپس (خالی *)(p + 1);
    }
    اگر (p == freep)
      اگر ((p = morecore(nunits)) == 0)
        واپس 0;
  }
}
