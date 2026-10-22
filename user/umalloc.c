#अन्तर्भाव "kernel/types.h"
#अन्तर्भाव "kernel/stat.h"
#अन्तर्भाव "user/user.h"
#अन्तर्भाव "kernel/param.h"

// Memory allocator by Kernighan and Ritchie,
// The C programming Language, 2nd ed.  Section 8.7.

प्रकारनिर्देश दीर्घ Align;

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
  लागि (p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
    यदि (p >= p->s.ptr && (bp > p || bp < p->s.ptr))
      रोक;
  यदि (bp + bp->s.size == p->s.ptr) {
    bp->s.size += p->s.ptr->s.size;
    bp->s.ptr = p->s.ptr->s.ptr;
  } अन्यथा
    bp->s.ptr = p->s.ptr;
  यदि (p + p->s.size == bp) {
    p->s.size += bp->s.size;
    p->s.ptr = bp->s.ptr;
  } अन्यथा
    p->s.ptr = bp;
  freep = p;
}

स्थिर Header *
morecore(uint nu)
{
  वर्ण *p;
  Header *hp;

  यदि (nu < 4096)
    nu = 4096;
  p = sbrk(nu * परिमाण(Header));
  यदि (p == SBRK_ERROR)
    फर्काऊ 0;
  hp = (Header *)p;
  hp->s.size = nu;
  मुक्त((शून्य *)(hp + 1));
  फर्काऊ freep;
}

शून्य *
दो_स्मृति(uint nbytes)
{
  Header *p, *prevp;
  uint nunits;

  nunits = (nbytes + परिमाण(Header) - 1) / परिमाण(Header) + 1;
  यदि ((prevp = freep) == 0) {
    base.s.ptr = freep = prevp = &base;
    base.s.size = 0;
  }
  लागि (p = prevp->s.ptr;; prevp = p, p = p->s.ptr) {
    यदि (p->s.size >= nunits) {
      यदि (p->s.size == nunits)
        prevp->s.ptr = p->s.ptr;
      अन्यथा {
        p->s.size -= nunits;
        p += p->s.size;
        p->s.size = nunits;
      }
      freep = prevp;
      फर्काऊ (शून्य *)(p + 1);
    }
    यदि (p == freep)
      यदि ((p = morecore(nunits)) == 0)
        फर्काऊ 0;
  }
}
