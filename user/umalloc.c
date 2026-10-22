#అన్తర్భావ "kernel/types.h"
#అన్తర్భావ "kernel/stat.h"
#అన్తర్భావ "user/user.h"
#అన్తర్భావ "kernel/param.h"

// Memory allocator by Kernighan and Ritchie,
// The C programming Language, 2nd ed.  Section 8.7.

రకనిర్వచనం దీర్ఘ Align;

సంఘం శీర్షిక {
  నిర్మాణం {
    సంఘం శీర్షిక *ptr;
    uint కొలత;
  } s;
  Align x;
};

రకనిర్వచనం సంఘం శీర్షిక శీర్షిక_2;

స్థిర శీర్షిక_2 పునాది;
స్థిర శీర్షిక_2 *freep;

శూన్య
విడుదల(శూన్య *ap)
{
  శీర్షిక_2 *bp, *p;

  bp = (శీర్షిక_2 *)ap - 1;
  కొరకు (p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
    ఒకవేళ (p >= p->s.ptr && (bp > p || bp < p->s.ptr))
      ఆపు;
  ఒకవేళ (bp + bp->s.కొలత == p->s.ptr) {
    bp->s.కొలత += p->s.ptr->s.కొలత;
    bp->s.ptr = p->s.ptr->s.ptr;
  } లేకపోతే
    bp->s.ptr = p->s.ptr;
  ఒకవేళ (p + p->s.కొలత == bp) {
    p->s.కొలత += bp->s.కొలత;
    p->s.ptr = bp->s.ptr;
  } లేకపోతే
    p->s.ptr = bp;
  freep = p;
}

స్థిర శీర్షిక_2 *
morecore(uint nu)
{
  అక్షరం *p;
  శీర్షిక_2 *hp;

  ఒకవేళ (nu < 4096)
    nu = 4096;
  p = sbrk(nu * పరిమాణం(శీర్షిక_2));
  ఒకవేళ (p == SBRK_ERROR)
    తిరిగి 0;
  hp = (శీర్షిక_2 *)p;
  hp->s.కొలత = nu;
  విడుదల((శూన్య *)(hp + 1));
  తిరిగి freep;
}

శూన్య *
జ్ఞాపకం_ఇవ్వు(uint nbytes)
{
  శీర్షిక_2 *p, *prevp;
  uint nunits;

  nunits = (nbytes + పరిమాణం(శీర్షిక_2) - 1) / పరిమాణం(శీర్షిక_2) + 1;
  ఒకవేళ ((prevp = freep) == 0) {
    పునాది.s.ptr = freep = prevp = &పునాది;
    పునాది.s.కొలత = 0;
  }
  కొరకు (p = prevp->s.ptr;; prevp = p, p = p->s.ptr) {
    ఒకవేళ (p->s.కొలత >= nunits) {
      ఒకవేళ (p->s.కొలత == nunits)
        prevp->s.ptr = p->s.ptr;
      లేకపోతే {
        p->s.కొలత -= nunits;
        p += p->s.కొలత;
        p->s.కొలత = nunits;
      }
      freep = prevp;
      తిరిగి (శూన్య *)(p + 1);
    }
    ఒకవేళ (p == freep)
      ఒకవేళ ((p = morecore(nunits)) == 0)
        తిరిగి 0;
  }
}
