#અન્તર્ભાવ "kernel/types.h"
#અન્તર્ભાવ "kernel/stat.h"
#અન્તર્ભાવ "user/user.h"
#અન્તર્ભાવ "kernel/param.h"

// Memory allocator by Kernighan and Ritchie,
// The C programming Language, 2nd ed.  Section 8.7.

પ્રકારવ્યાખ્યા long Align;

સંઘ header {
  સંરચના {
    સંઘ header *ptr;
    uint size;
  } s;
  Align x;
};

પ્રકારવ્યાખ્યા સંઘ header Header;

સ્થિર Header base;
સ્થિર Header *freep;

શૂન્ય
મુક્ત(શૂન્ય *ap)
{
  Header *bp, *p;

  bp = (Header *)ap - 1;
  માટે (p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
    જો (p >= p->s.ptr && (bp > p || bp < p->s.ptr))
      તોડો;
  જો (bp + bp->s.size == p->s.ptr) {
    bp->s.size += p->s.ptr->s.size;
    bp->s.ptr = p->s.ptr->s.ptr;
  } નહીંતર
    bp->s.ptr = p->s.ptr;
  જો (p + p->s.size == bp) {
    p->s.size += bp->s.size;
    p->s.ptr = bp->s.ptr;
  } નહીંતર
    p->s.ptr = bp;
  freep = p;
}

સ્થિર Header *
morecore(uint nu)
{
  અક્ષર *p;
  Header *hp;

  જો (nu < 4096)
    nu = 4096;
  p = sbrk(nu * કદ(Header));
  જો (p == SBRK_ERROR)
    પાછા 0;
  hp = (Header *)p;
  hp->s.size = nu;
  મુક્ત((શૂન્ય *)(hp + 1));
  પાછા freep;
}

શૂન્ય *
malloc(uint nbytes)
{
  Header *p, *prevp;
  uint nunits;

  nunits = (nbytes + કદ(Header) - 1) / કદ(Header) + 1;
  જો ((prevp = freep) == 0) {
    base.s.ptr = freep = prevp = &base;
    base.s.size = 0;
  }
  માટે (p = prevp->s.ptr;; prevp = p, p = p->s.ptr) {
    જો (p->s.size >= nunits) {
      જો (p->s.size == nunits)
        prevp->s.ptr = p->s.ptr;
      નહીંતર {
        p->s.size -= nunits;
        p += p->s.size;
        p->s.size = nunits;
      }
      freep = prevp;
      પાછા (શૂન્ય *)(p + 1);
    }
    જો (p == freep)
      જો ((p = morecore(nunits)) == 0)
        પાછા 0;
  }
}
