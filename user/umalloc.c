#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

// Memory allocator by Kernighan and Ritchie,
// The C programming Language, 2nd ed.  Section 8.7.

typedef long Align;

sa_ngha header {
  sa_nracanaa {
    sa_ngha header *ptr;
    uint size;
  } s;
  Align x;
};

typedef sa_ngha header Header;

static Header base;
static Header *freep;

void
free(void *ap)
{
  Header *bp, *p;

  bp = (Header *)ap - 1;
  for (p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
    ᬬᬾᬦ᭄ (p >= p->s.ptr && (bp > p || bp < p->s.ptr))
      bha_nga;
  ᬬᬾᬦ᭄ (bp + bp->s.size == p->s.ptr) {
    bp->s.size += p->s.ptr->s.size;
    bp->s.ptr = p->s.ptr->s.ptr;
  } ᬮᬶᬬ
    bp->s.ptr = p->s.ptr;
  ᬬᬾᬦ᭄ (p + p->s.size == bp) {
    p->s.size += bp->s.size;
    p->s.ptr = bp->s.ptr;
  } ᬮᬶᬬ
    p->s.ptr = bp;
  freep = p;
}

static Header *
morecore(uint nu)
{
  var_na *p;
  Header *hp;

  ᬬᬾᬦ᭄ (nu < 4096)
    nu = 4096;
  p = sbrk(nu * parimaa_na(Header));
  ᬬᬾᬦ᭄ (p == SBRK_ERROR)
    ᬯᬮᬶ 0;
  hp = (Header *)p;
  hp->s.size = nu;
  free((void *)(hp + 1));
  ᬯᬮᬶ freep;
}

void *
malloc(uint nbytes)
{
  Header *p, *prevp;
  uint nunits;

  nunits = (nbytes + parimaa_na(Header) - 1) / parimaa_na(Header) + 1;
  ᬬᬾᬦ᭄ ((prevp = freep) == 0) {
    base.s.ptr = freep = prevp = &base;
    base.s.size = 0;
  }
  for (p = prevp->s.ptr;; prevp = p, p = p->s.ptr) {
    ᬬᬾᬦ᭄ (p->s.size >= nunits) {
      ᬬᬾᬦ᭄ (p->s.size == nunits)
        prevp->s.ptr = p->s.ptr;
      ᬮᬶᬬ {
        p->s.size -= nunits;
        p += p->s.size;
        p->s.size = nunits;
      }
      freep = prevp;
      ᬯᬮᬶ (void *)(p + 1);
    }
    ᬬᬾᬦ᭄ (p == freep)
      ᬬᬾᬦ᭄ ((p = morecore(nunits)) == 0)
        ᬯᬮᬶ 0;
  }
}
