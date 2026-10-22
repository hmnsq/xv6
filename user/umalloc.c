#അന്തര്ഭാവ "kernel/types.h"
#അന്തര്ഭാവ "kernel/stat.h"
#അന്തര്ഭാവ "user/user.h"
#അന്തര്ഭാവ "kernel/param.h"

// Memory allocator by Kernighan and Ritchie,
// The C programming Language, 2nd ed.  Section 8.7.

തരനിർവചനം long Align;

സംഘം header {
  ഘടന {
    സംഘം header *ptr;
    uint size;
  } s;
  Align x;
};

തരനിർവചനം സംഘം header Header;

സ്ഥിതം Header base;
സ്ഥിതം Header *freep;

ശൂന്യം
സ്വതന്ത്രം(ശൂന്യം *ap)
{
  Header *bp, *p;

  bp = (Header *)ap - 1;
  വേണ്ടി (p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
    എങ്കിൽ (p >= p->s.ptr && (bp > p || bp < p->s.ptr))
      നിർത്തുക;
  എങ്കിൽ (bp + bp->s.size == p->s.ptr) {
    bp->s.size += p->s.ptr->s.size;
    bp->s.ptr = p->s.ptr->s.ptr;
  } അല്ലെങ്കിൽ
    bp->s.ptr = p->s.ptr;
  എങ്കിൽ (p + p->s.size == bp) {
    p->s.size += bp->s.size;
    p->s.ptr = bp->s.ptr;
  } അല്ലെങ്കിൽ
    p->s.ptr = bp;
  freep = p;
}

സ്ഥിതം Header *
morecore(uint nu)
{
  അക്ഷരം *p;
  Header *hp;

  എങ്കിൽ (nu < 4096)
    nu = 4096;
  p = sbrk(nu * വലുപ്പം(Header));
  എങ്കിൽ (p == SBRK_ERROR)
    തിരികെ 0;
  hp = (Header *)p;
  hp->s.size = nu;
  സ്വതന്ത്രം((ശൂന്യം *)(hp + 1));
  തിരികെ freep;
}

ശൂന്യം *
malloc(uint nbytes)
{
  Header *p, *prevp;
  uint nunits;

  nunits = (nbytes + വലുപ്പം(Header) - 1) / വലുപ്പം(Header) + 1;
  എങ്കിൽ ((prevp = freep) == 0) {
    base.s.ptr = freep = prevp = &base;
    base.s.size = 0;
  }
  വേണ്ടി (p = prevp->s.ptr;; prevp = p, p = p->s.ptr) {
    എങ്കിൽ (p->s.size >= nunits) {
      എങ്കിൽ (p->s.size == nunits)
        prevp->s.ptr = p->s.ptr;
      അല്ലെങ്കിൽ {
        p->s.size -= nunits;
        p += p->s.size;
        p->s.size = nunits;
      }
      freep = prevp;
      തിരികെ (ശൂന്യം *)(p + 1);
    }
    എങ്കിൽ (p == freep)
      എങ്കിൽ ((p = morecore(nunits)) == 0)
        തിരികെ 0;
  }
}
