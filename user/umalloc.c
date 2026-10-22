#شامل "kernel/types.h"
#شامل "kernel/stat.h"
#شامل "user/user.h"
#شامل "kernel/param.h"

// Memory allocator by Kernighan and Ritchie,
// The C programming Language, 2nd ed.  Section 8.7.

تعریفنوع long Align;

اجتماع header {
  ساختار {
    اجتماع header *ptr;
    uint size;
  } s;
  Align x;
};

تعریفنوع اجتماع header Header;

ایستا Header base;
ایستا Header *freep;

تهی
آزاد(تهی *ap)
{
  Header *bp, *p;

  bp = (Header *)ap - 1;
  برای (p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
    اگر (p >= p->s.ptr && (bp > p || bp < p->s.ptr))
      بشکن;
  اگر (bp + bp->s.size == p->s.ptr) {
    bp->s.size += p->s.ptr->s.size;
    bp->s.ptr = p->s.ptr->s.ptr;
  } وگرنه
    bp->s.ptr = p->s.ptr;
  اگر (p + p->s.size == bp) {
    p->s.size += bp->s.size;
    p->s.ptr = bp->s.ptr;
  } وگرنه
    p->s.ptr = bp;
  freep = p;
}

ایستا Header *
morecore(uint nu)
{
  نویسه *p;
  Header *hp;

  اگر (nu < 4096)
    nu = 4096;
  p = sbrk(nu * اندازه(Header));
  اگر (p == SBRK_ERROR)
    بازگشت 0;
  hp = (Header *)p;
  hp->s.size = nu;
  آزاد((تهی *)(hp + 1));
  بازگشت freep;
}

تهی *
malloc(uint nbytes)
{
  Header *p, *prevp;
  uint nunits;

  nunits = (nbytes + اندازه(Header) - 1) / اندازه(Header) + 1;
  اگر ((prevp = freep) == 0) {
    base.s.ptr = freep = prevp = &base;
    base.s.size = 0;
  }
  برای (p = prevp->s.ptr;; prevp = p, p = p->s.ptr) {
    اگر (p->s.size >= nunits) {
      اگر (p->s.size == nunits)
        prevp->s.ptr = p->s.ptr;
      وگرنه {
        p->s.size -= nunits;
        p += p->s.size;
        p->s.size = nunits;
      }
      freep = prevp;
      بازگشت (تهی *)(p + 1);
    }
    اگر (p == freep)
      اگر ((p = morecore(nunits)) == 0)
        بازگشت 0;
  }
}
