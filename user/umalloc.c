#अन्तर्भाव "kernel/types.h"
#अन्तर्भाव "kernel/stat.h"
#अन्तर्भाव "user/user.h"
#अन्तर्भाव "kernel/param.h"

// Memory allocator by Kernighan and Ritchie,
// The C programming Language, 2nd ed.  Section 8.7.

प्रकारनिर्देश दीर्घ Align;

सङ्घ शीर्षकम् {
  संरचना {
    सङ्घ शीर्षकम् *ptr;
    uint आकार;
  } s;
  Align x;
};

प्रकारनिर्देश सङ्घ शीर्षकम् शीर्षकम्_2;

स्थिर शीर्षकम्_2 आधारः;
स्थिर शीर्षकम्_2 *freep;

शून्य
मुक्त(शून्य *ap)
{
  शीर्षकम्_2 *bp, *p;

  bp = (शीर्षकम्_2 *)ap - 1;
  प्रति (p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
    यदि (p >= p->s.ptr && (bp > p || bp < p->s.ptr))
      भङ्ग;
  यदि (bp + bp->s.आकार == p->s.ptr) {
    bp->s.आकार += p->s.ptr->s.आकार;
    bp->s.ptr = p->s.ptr->s.ptr;
  } अन्यथा
    bp->s.ptr = p->s.ptr;
  यदि (p + p->s.आकार == bp) {
    p->s.आकार += bp->s.आकार;
    p->s.ptr = bp->s.ptr;
  } अन्यथा
    p->s.ptr = bp;
  freep = p;
}

स्थिर शीर्षकम्_2 *
morecore(uint nu)
{
  वर्ण *p;
  शीर्षकम्_2 *hp;

  यदि (nu < 4096)
    nu = 4096;
  p = sbrk(nu * परिमाण(शीर्षकम्_2));
  यदि (p == SBRK_ERROR)
    प्रत्यागम 0;
  hp = (शीर्षकम्_2 *)p;
  hp->s.आकार = nu;
  मुक्त((शून्य *)(hp + 1));
  प्रत्यागम freep;
}

शून्य *
स्मृतिं_देहि(uint nbytes)
{
  शीर्षकम्_2 *p, *prevp;
  uint nunits;

  nunits = (nbytes + परिमाण(शीर्षकम्_2) - 1) / परिमाण(शीर्षकम्_2) + 1;
  यदि ((prevp = freep) == 0) {
    आधारः.s.ptr = freep = prevp = &आधारः;
    आधारः.s.आकार = 0;
  }
  प्रति (p = prevp->s.ptr;; prevp = p, p = p->s.ptr) {
    यदि (p->s.आकार >= nunits) {
      यदि (p->s.आकार == nunits)
        prevp->s.ptr = p->s.ptr;
      अन्यथा {
        p->s.आकार -= nunits;
        p += p->s.आकार;
        p->s.आकार = nunits;
      }
      freep = prevp;
      प्रत्यागम (शून्य *)(p + 1);
    }
    यदि (p == freep)
      यदि ((p = morecore(nunits)) == 0)
        प्रत्यागम 0;
  }
}
