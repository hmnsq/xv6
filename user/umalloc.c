#समावेश "kernel/types.h"
#समावेश "kernel/stat.h"
#समावेश "user/user.h"
#समावेश "kernel/param.h"

// Memory allocator by Kernighan and Ritchie,
// The C programming Language, 2nd ed.  Section 8.7.

प्रकार दीर्घ Align;

जोड़ header {
  काष्ठा {
    जोड़ header *ptr;
    uint आकार;
  } s;
  Align x;
};

प्रकार जोड़ header Header;

अटल Header base;
अटल Header *freep;

व्योम
मुक्त(व्योम *ap)
{
  Header *bp, *p;

  bp = (Header *)ap - 1;
  क्रम (p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
    अगर (p >= p->s.ptr && (bp > p || bp < p->s.ptr))
      अवरोध;
  अगर (bp + bp->s.आकार == p->s.ptr) {
    bp->s.आकार += p->s.ptr->s.आकार;
    bp->s.ptr = p->s.ptr->s.ptr;
  } अन्यथा
    bp->s.ptr = p->s.ptr;
  अगर (p + p->s.आकार == bp) {
    p->s.आकार += bp->s.आकार;
    p->s.ptr = bp->s.ptr;
  } अन्यथा
    p->s.ptr = bp;
  freep = p;
}

अटल Header *
morecore(uint nu)
{
  अक्षर *p;
  Header *hp;

  अगर (nu < 4096)
    nu = 4096;
  p = sbrk(nu * माप(Header));
  अगर (p == SBRK_ERROR)
    वापस 0;
  hp = (Header *)p;
  hp->s.आकार = nu;
  मुक्त((व्योम *)(hp + 1));
  वापस freep;
}

व्योम *
दो_स्मृति(uint nbytes)
{
  Header *p, *prevp;
  uint nunits;

  nunits = (nbytes + माप(Header) - 1) / माप(Header) + 1;
  अगर ((prevp = freep) == 0) {
    base.s.ptr = freep = prevp = &base;
    base.s.आकार = 0;
  }
  क्रम (p = prevp->s.ptr;; prevp = p, p = p->s.ptr) {
    अगर (p->s.आकार >= nunits) {
      अगर (p->s.आकार == nunits)
        prevp->s.ptr = p->s.ptr;
      अन्यथा {
        p->s.आकार -= nunits;
        p += p->s.आकार;
        p->s.आकार = nunits;
      }
      freep = prevp;
      वापस (व्योम *)(p + 1);
    }
    अगर (p == freep)
      अगर ((p = morecore(nunits)) == 0)
        वापस 0;
  }
}
