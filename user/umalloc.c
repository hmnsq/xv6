#अन्तर्भाव "kernel/types.h"
#अन्तर्भाव "kernel/stat.h"
#अन्तर्भाव "user/user.h"
#अन्तर्भाव "kernel/param.h"

// Memory allocator by Kernighan and Ritchie,
// The C programming Language, 2nd ed.  Section 8.7.

प्रकारनिर्देश long Align;

सङ्घ header {
  संरचना {
    सङ्घ header *ptr;
    uint size;
  } s;
  Align x;
};

प्रकारनिर्देश सङ्घ header Header;

स्थिर Header base;
स्थिर Header *freep;

शून्य
मुक्त(शून्य *ap)
{
  Header *bp, *p;

  bp = (Header *)ap - 1;
  अत्थाय (p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
    सचे (p >= p->s.ptr && (bp > p || bp < p->s.ptr))
      भङ्ग;
  सचे (bp + bp->s.size == p->s.ptr) {
    bp->s.size += p->s.ptr->s.size;
    bp->s.ptr = p->s.ptr->s.ptr;
  } अञ्ञथा
    bp->s.ptr = p->s.ptr;
  सचे (p + p->s.size == bp) {
    p->s.size += bp->s.size;
    p->s.ptr = bp->s.ptr;
  } अञ्ञथा
    p->s.ptr = bp;
  freep = p;
}

स्थिर Header *
morecore(uint nu)
{
  वर्ण *p;
  Header *hp;

  सचे (nu < 4096)
    nu = 4096;
  p = sbrk(nu * परिमाण(Header));
  सचे (p == SBRK_ERROR)
    पटिनिवत्त 0;
  hp = (Header *)p;
  hp->s.size = nu;
  मुक्त((शून्य *)(hp + 1));
  पटिनिवत्त freep;
}

शून्य *
malloc(uint nbytes)
{
  Header *p, *prevp;
  uint nunits;

  nunits = (nbytes + परिमाण(Header) - 1) / परिमाण(Header) + 1;
  सचे ((prevp = freep) == 0) {
    base.s.ptr = freep = prevp = &base;
    base.s.size = 0;
  }
  अत्थाय (p = prevp->s.ptr;; prevp = p, p = p->s.ptr) {
    सचे (p->s.size >= nunits) {
      सचे (p->s.size == nunits)
        prevp->s.ptr = p->s.ptr;
      अञ्ञथा {
        p->s.size -= nunits;
        p += p->s.size;
        p->s.size = nunits;
      }
      freep = prevp;
      पटिनिवत्त (शून्य *)(p + 1);
    }
    सचे (p == freep)
      सचे ((p = morecore(nunits)) == 0)
        पटिनिवत्त 0;
  }
}
