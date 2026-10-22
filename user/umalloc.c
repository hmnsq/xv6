#ಅನ್ತರ್ಭಾವ "kernel/types.h"
#ಅನ್ತರ್ಭಾವ "kernel/stat.h"
#ಅನ್ತರ್ಭಾವ "user/user.h"
#ಅನ್ತರ್ಭಾವ "kernel/param.h"

// Memory allocator by Kernighan and Ritchie,
// The C programming Language, 2nd ed.  Section 8.7.

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ದೀರ್ಘ Align;

ಒಕ್ಕೂಟ ಶೀರ್ಷಿಕೆ {
  ರಚನೆ {
    ಒಕ್ಕೂಟ ಶೀರ್ಷಿಕೆ *ptr;
    uint ಗಾತ್ರ_2;
  } s;
  Align x;
};

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಒಕ್ಕೂಟ ಶೀರ್ಷಿಕೆ ಶೀರ್ಷಿಕೆ_2;

ಸ್ಥಿರ ಶೀರ್ಷಿಕೆ_2 ತಳ;
ಸ್ಥಿರ ಶೀರ್ಷಿಕೆ_2 *freep;

ಶೂನ್ಯ
ಬಿಡುಗಡೆ(ಶೂನ್ಯ *ap)
{
  ಶೀರ್ಷಿಕೆ_2 *bp, *p;

  bp = (ಶೀರ್ಷಿಕೆ_2 *)ap - 1;
  ಗಾಗಿ (p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
    ಆದರೆ (p >= p->s.ptr && (bp > p || bp < p->s.ptr))
      ನಿಲ್ಲಿಸು;
  ಆದರೆ (bp + bp->s.ಗಾತ್ರ_2 == p->s.ptr) {
    bp->s.ಗಾತ್ರ_2 += p->s.ptr->s.ಗಾತ್ರ_2;
    bp->s.ptr = p->s.ptr->s.ptr;
  } ಇಲ್ಲದಿದ್ದರೆ
    bp->s.ptr = p->s.ptr;
  ಆದರೆ (p + p->s.ಗಾತ್ರ_2 == bp) {
    p->s.ಗಾತ್ರ_2 += bp->s.ಗಾತ್ರ_2;
    p->s.ptr = bp->s.ptr;
  } ಇಲ್ಲದಿದ್ದರೆ
    p->s.ptr = bp;
  freep = p;
}

ಸ್ಥಿರ ಶೀರ್ಷಿಕೆ_2 *
morecore(uint nu)
{
  ಅಕ್ಷರ *p;
  ಶೀರ್ಷಿಕೆ_2 *hp;

  ಆದರೆ (nu < 4096)
    nu = 4096;
  p = sbrk(nu * ಗಾತ್ರ(ಶೀರ್ಷಿಕೆ_2));
  ಆದರೆ (p == SBRK_ERROR)
    ಹಿಂತಿರುಗಿ 0;
  hp = (ಶೀರ್ಷಿಕೆ_2 *)p;
  hp->s.ಗಾತ್ರ_2 = nu;
  ಬಿಡುಗಡೆ((ಶೂನ್ಯ *)(hp + 1));
  ಹಿಂತಿರುಗಿ freep;
}

ಶೂನ್ಯ *
ಸ್ಮರಣೆ_ಕೊಡು(uint nbytes)
{
  ಶೀರ್ಷಿಕೆ_2 *p, *prevp;
  uint nunits;

  nunits = (nbytes + ಗಾತ್ರ(ಶೀರ್ಷಿಕೆ_2) - 1) / ಗಾತ್ರ(ಶೀರ್ಷಿಕೆ_2) + 1;
  ಆದರೆ ((prevp = freep) == 0) {
    ತಳ.s.ptr = freep = prevp = &ತಳ;
    ತಳ.s.ಗಾತ್ರ_2 = 0;
  }
  ಗಾಗಿ (p = prevp->s.ptr;; prevp = p, p = p->s.ptr) {
    ಆದರೆ (p->s.ಗಾತ್ರ_2 >= nunits) {
      ಆದರೆ (p->s.ಗಾತ್ರ_2 == nunits)
        prevp->s.ptr = p->s.ptr;
      ಇಲ್ಲದಿದ್ದರೆ {
        p->s.ಗಾತ್ರ_2 -= nunits;
        p += p->s.ಗಾತ್ರ_2;
        p->s.ಗಾತ್ರ_2 = nunits;
      }
      freep = prevp;
      ಹಿಂತಿರುಗಿ (ಶೂನ್ಯ *)(p + 1);
    }
    ಆದರೆ (p == freep)
      ಆದರೆ ((p = morecore(nunits)) == 0)
        ಹಿಂತಿರುಗಿ 0;
  }
}
