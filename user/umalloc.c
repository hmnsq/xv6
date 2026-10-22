#ଅନ୍ତର୍ଭାଵ "kernel/types.h"
#ଅନ୍ତର୍ଭାଵ "kernel/stat.h"
#ଅନ୍ତର୍ଭାଵ "user/user.h"
#ଅନ୍ତର୍ଭାଵ "kernel/param.h"

// Memory allocator by Kernighan and Ritchie,
// The C programming Language, 2nd ed.  Section 8.7.

ପ୍ରକାରବ୍ୟାଖ୍ୟା long Align;

ସଂଘ header {
  ସଂରଚନା {
    ସଂଘ header *ptr;
    uint size;
  } s;
  Align x;
};

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂଘ header Header;

ସ୍ଥିର Header base;
ସ୍ଥିର Header *freep;

ଶୂନ୍ୟ
ମୁକ୍ତ(ଶୂନ୍ୟ *ap)
{
  Header *bp, *p;

  bp = (Header *)ap - 1;
  ପାଇଁ (p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
    ଯଦି (p >= p->s.ptr && (bp > p || bp < p->s.ptr))
      ଭାଙ୍ଗ;
  ଯଦି (bp + bp->s.size == p->s.ptr) {
    bp->s.size += p->s.ptr->s.size;
    bp->s.ptr = p->s.ptr->s.ptr;
  } ଅନ୍ୟଥା
    bp->s.ptr = p->s.ptr;
  ଯଦି (p + p->s.size == bp) {
    p->s.size += bp->s.size;
    p->s.ptr = bp->s.ptr;
  } ଅନ୍ୟଥା
    p->s.ptr = bp;
  freep = p;
}

ସ୍ଥିର Header *
morecore(uint nu)
{
  ଅକ୍ଷର *p;
  Header *hp;

  ଯଦି (nu < 4096)
    nu = 4096;
  p = sbrk(nu * ଆକାର(Header));
  ଯଦି (p == SBRK_ERROR)
    ଫେରନ୍ତୁ 0;
  hp = (Header *)p;
  hp->s.size = nu;
  ମୁକ୍ତ((ଶୂନ୍ୟ *)(hp + 1));
  ଫେରନ୍ତୁ freep;
}

ଶୂନ୍ୟ *
malloc(uint nbytes)
{
  Header *p, *prevp;
  uint nunits;

  nunits = (nbytes + ଆକାର(Header) - 1) / ଆକାର(Header) + 1;
  ଯଦି ((prevp = freep) == 0) {
    base.s.ptr = freep = prevp = &base;
    base.s.size = 0;
  }
  ପାଇଁ (p = prevp->s.ptr;; prevp = p, p = p->s.ptr) {
    ଯଦି (p->s.size >= nunits) {
      ଯଦି (p->s.size == nunits)
        prevp->s.ptr = p->s.ptr;
      ଅନ୍ୟଥା {
        p->s.size -= nunits;
        p += p->s.size;
        p->s.size = nunits;
      }
      freep = prevp;
      ଫେରନ୍ତୁ (ଶୂନ୍ୟ *)(p + 1);
    }
    ଯଦି (p == freep)
      ଯଦି ((p = morecore(nunits)) == 0)
        ଫେରନ୍ତୁ 0;
  }
}
