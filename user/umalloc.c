#अन्तर्भाव "kernel/types.h"
#अन्तर्भाव "kernel/stat.h"
#अन्तर्भाव "user/user.h"
#अन्तर्भाव "kernel/param.h"

// Memory allocator by Kernighan and Ritchie,
// The C programming Language, 2nd ed.  Section 8.7.

प्रकारव्याख्या दीर्घ Align;

संघ header {
  संरचना {
    संघ header *ptr;
    uint size;
  } s;
  Align x;
};

प्रकारव्याख्या संघ header Header;

स्थिर Header base;
स्थिर Header *freep;

शून्य
मुक्त(शून्य *ap)
{
  Header *bp, *p;

  bp = (Header *)ap - 1;
  साठी (p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
    जर (p >= p->s.ptr && (bp > p || bp < p->s.ptr))
      थांबा;
  जर (bp + bp->s.size == p->s.ptr) {
    bp->s.size += p->s.ptr->s.size;
    bp->s.ptr = p->s.ptr->s.ptr;
  } अन्यथा
    bp->s.ptr = p->s.ptr;
  जर (p + p->s.size == bp) {
    p->s.size += bp->s.size;
    p->s.ptr = bp->s.ptr;
  } अन्यथा
    p->s.ptr = bp;
  freep = p;
}

स्थिर Header *
morecore(uint nu)
{
  अक्षर *p;
  Header *hp;

  जर (nu < 4096)
    nu = 4096;
  p = sbrk(nu * आकार(Header));
  जर (p == SBRK_ERROR)
    परत 0;
  hp = (Header *)p;
  hp->s.size = nu;
  मुक्त((शून्य *)(hp + 1));
  परत freep;
}

शून्य *
दो_स्मृति(uint nbytes)
{
  Header *p, *prevp;
  uint nunits;

  nunits = (nbytes + आकार(Header) - 1) / आकार(Header) + 1;
  जर ((prevp = freep) == 0) {
    base.s.ptr = freep = prevp = &base;
    base.s.size = 0;
  }
  साठी (p = prevp->s.ptr;; prevp = p, p = p->s.ptr) {
    जर (p->s.size >= nunits) {
      जर (p->s.size == nunits)
        prevp->s.ptr = p->s.ptr;
      अन्यथा {
        p->s.size -= nunits;
        p += p->s.size;
        p->s.size = nunits;
      }
      freep = prevp;
      परत (शून्य *)(p + 1);
    }
    जर (p == freep)
      जर ((p = morecore(nunits)) == 0)
        परत 0;
  }
}
