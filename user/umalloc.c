#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/types.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/stat.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "user/user.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/param.h"

// Memory allocator by Kernighan and Ritchie,
// The C programming Language, 2nd ed.  Section 8.7.

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਲੰਮਾ Align;

ਸੰਘ ਸਿਰਲੇਖ {
  ਬਣਤਰ {
    ਸੰਘ ਸਿਰਲੇਖ *ptr;
    uint ਮਾਪ;
  } s;
  Align x;
};

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਸੰਘ ਸਿਰਲੇਖ ਸਿਰਲੇਖ_2;

ਸਥਿਰ ਸਿਰਲੇਖ_2 ਅਧਾਰ;
ਸਥਿਰ ਸਿਰਲੇਖ_2 *freep;

ਖਾਲੀ
ਮੁਕਤ(ਖਾਲੀ *ap)
{
  ਸਿਰਲੇਖ_2 *bp, *p;

  bp = (ਸਿਰਲੇਖ_2 *)ap - 1;
  ਲਈ (p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
    ਜੇ (p >= p->s.ptr && (bp > p || bp < p->s.ptr))
      ਰੋਕੋ;
  ਜੇ (bp + bp->s.ਮਾਪ == p->s.ptr) {
    bp->s.ਮਾਪ += p->s.ptr->s.ਮਾਪ;
    bp->s.ptr = p->s.ptr->s.ptr;
  } ਨਹੀਂਤਾਂ
    bp->s.ptr = p->s.ptr;
  ਜੇ (p + p->s.ਮਾਪ == bp) {
    p->s.ਮਾਪ += bp->s.ਮਾਪ;
    p->s.ptr = bp->s.ptr;
  } ਨਹੀਂਤਾਂ
    p->s.ptr = bp;
  freep = p;
}

ਸਥਿਰ ਸਿਰਲੇਖ_2 *
morecore(uint nu)
{
  ਅੱਖਰ *p;
  ਸਿਰਲੇਖ_2 *hp;

  ਜੇ (nu < 4096)
    nu = 4096;
  p = sbrk(nu * ਆਕਾਰ(ਸਿਰਲੇਖ_2));
  ਜੇ (p == SBRK_ERROR)
    ਵਾਪਸ 0;
  hp = (ਸਿਰਲੇਖ_2 *)p;
  hp->s.ਮਾਪ = nu;
  ਮੁਕਤ((ਖਾਲੀ *)(hp + 1));
  ਵਾਪਸ freep;
}

ਖਾਲੀ *
ਸਮ੍ਰਿਤੀ_ਦਿਓ(uint nbytes)
{
  ਸਿਰਲੇਖ_2 *p, *prevp;
  uint nunits;

  nunits = (nbytes + ਆਕਾਰ(ਸਿਰਲੇਖ_2) - 1) / ਆਕਾਰ(ਸਿਰਲੇਖ_2) + 1;
  ਜੇ ((prevp = freep) == 0) {
    ਅਧਾਰ.s.ptr = freep = prevp = &ਅਧਾਰ;
    ਅਧਾਰ.s.ਮਾਪ = 0;
  }
  ਲਈ (p = prevp->s.ptr;; prevp = p, p = p->s.ptr) {
    ਜੇ (p->s.ਮਾਪ >= nunits) {
      ਜੇ (p->s.ਮਾਪ == nunits)
        prevp->s.ptr = p->s.ptr;
      ਨਹੀਂਤਾਂ {
        p->s.ਮਾਪ -= nunits;
        p += p->s.ਮਾਪ;
        p->s.ਮਾਪ = nunits;
      }
      freep = prevp;
      ਵਾਪਸ (ਖਾਲੀ *)(p + 1);
    }
    ਜੇ (p == freep)
      ਜੇ ((p = morecore(nunits)) == 0)
        ਵਾਪਸ 0;
  }
}
