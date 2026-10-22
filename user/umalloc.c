#அந்தர்பாவ "kernel/types.h"
#அந்தர்பாவ "kernel/stat.h"
#அந்தர்பாவ "user/user.h"
#அந்தர்பாவ "kernel/param.h"

// Memory allocator by Kernighan and Ritchie,
// The C programming Language, 2nd ed.  Section 8.7.

வகைவரை long Align;

ஒன்றிப்பு header {
  கட்டமைப்பு {
    ஒன்றிப்பு header *ptr;
    uint size;
  } s;
  Align x;
};

வகைவரை ஒன்றிப்பு header Header;

நிலையான Header base;
நிலையான Header *freep;

வெறுமை
விடுவி(வெறுமை *ap)
{
  Header *bp, *p;

  bp = (Header *)ap - 1;
  க்கு (p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
    எனில் (p >= p->s.ptr && (bp > p || bp < p->s.ptr))
      நிறுத்து;
  எனில் (bp + bp->s.size == p->s.ptr) {
    bp->s.size += p->s.ptr->s.size;
    bp->s.ptr = p->s.ptr->s.ptr;
  } இல்லையெனில்
    bp->s.ptr = p->s.ptr;
  எனில் (p + p->s.size == bp) {
    p->s.size += bp->s.size;
    p->s.ptr = bp->s.ptr;
  } இல்லையெனில்
    p->s.ptr = bp;
  freep = p;
}

நிலையான Header *
morecore(uint nu)
{
  எழுத்து *p;
  Header *hp;

  எனில் (nu < 4096)
    nu = 4096;
  p = sbrk(nu * அளவு(Header));
  எனில் (p == SBRK_ERROR)
    திரும்பு 0;
  hp = (Header *)p;
  hp->s.size = nu;
  விடுவி((வெறுமை *)(hp + 1));
  திரும்பு freep;
}

வெறுமை *
malloc(uint nbytes)
{
  Header *p, *prevp;
  uint nunits;

  nunits = (nbytes + அளவு(Header) - 1) / அளவு(Header) + 1;
  எனில் ((prevp = freep) == 0) {
    base.s.ptr = freep = prevp = &base;
    base.s.size = 0;
  }
  க்கு (p = prevp->s.ptr;; prevp = p, p = p->s.ptr) {
    எனில் (p->s.size >= nunits) {
      எனில் (p->s.size == nunits)
        prevp->s.ptr = p->s.ptr;
      இல்லையெனில் {
        p->s.size -= nunits;
        p += p->s.size;
        p->s.size = nunits;
      }
      freep = prevp;
      திரும்பு (வெறுமை *)(p + 1);
    }
    எனில் (p == freep)
      எனில் ((p = morecore(nunits)) == 0)
        திரும்பு 0;
  }
}
