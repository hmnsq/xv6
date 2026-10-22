#ಅನ್ತರ್ಭಾವ "kernel/types.h"
#ಅನ್ತರ್ಭಾವ "kernel/stat.h"
#ಅನ್ತರ್ಭಾವ "user/user.h"
#ಅನ್ತರ್ಭಾವ "kernel/param.h"

// Memory allocator by Kernighan and Ritchie,
// The C programming Language, 2nd ed.  Section 8.7.

ಪ್ರಕಾರನಿರ್ದೇಶ long Align;

ಸಙ್ಘ header {
  ಸಂರಚನಾ {
    ಸಙ್ಘ header *ptr;
    uint size;
  } s;
  Align x;
};

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಙ್ಘ header Header;

ಸ್ಥಿರ Header base;
ಸ್ಥಿರ Header *freep;

ಶೂನ್ಯ
ಮುಕ್ತ(ಶೂನ್ಯ *ap)
{
  Header *bp, *p;

  bp = (Header *)ap - 1;
  ಪ್ರತಿ (p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
    ಯದಿ (p >= p->s.ptr && (bp > p || bp < p->s.ptr))
      ಭಙ್ಗ;
  ಯದಿ (bp + bp->s.size == p->s.ptr) {
    bp->s.size += p->s.ptr->s.size;
    bp->s.ptr = p->s.ptr->s.ptr;
  } ಅನ್ಯಥಾ
    bp->s.ptr = p->s.ptr;
  ಯದಿ (p + p->s.size == bp) {
    p->s.size += bp->s.size;
    p->s.ptr = bp->s.ptr;
  } ಅನ್ಯಥಾ
    p->s.ptr = bp;
  freep = p;
}

ಸ್ಥಿರ Header *
morecore(uint nu)
{
  ವರ್ಣ *p;
  Header *hp;

  ಯದಿ (nu < 4096)
    nu = 4096;
  p = sbrk(nu * ಪರಿಮಾಣ(Header));
  ಯದಿ (p == SBRK_ERROR)
    ಪ್ರತ್ಯಾಗಮ 0;
  hp = (Header *)p;
  hp->s.size = nu;
  ಮುಕ್ತ((ಶೂನ್ಯ *)(hp + 1));
  ಪ್ರತ್ಯಾಗಮ freep;
}

ಶೂನ್ಯ *
malloc(uint nbytes)
{
  Header *p, *prevp;
  uint nunits;

  nunits = (nbytes + ಪರಿಮಾಣ(Header) - 1) / ಪರಿಮಾಣ(Header) + 1;
  ಯದಿ ((prevp = freep) == 0) {
    base.s.ptr = freep = prevp = &base;
    base.s.size = 0;
  }
  ಪ್ರತಿ (p = prevp->s.ptr;; prevp = p, p = p->s.ptr) {
    ಯದಿ (p->s.size >= nunits) {
      ಯದಿ (p->s.size == nunits)
        prevp->s.ptr = p->s.ptr;
      ಅನ್ಯಥಾ {
        p->s.size -= nunits;
        p += p->s.size;
        p->s.size = nunits;
      }
      freep = prevp;
      ಪ್ರತ್ಯಾಗಮ (ಶೂನ್ಯ *)(p + 1);
    }
    ಯದಿ (p == freep)
      ಯದಿ ((p = morecore(nunits)) == 0)
        ಪ್ರತ್ಯಾಗಮ 0;
  }
}
