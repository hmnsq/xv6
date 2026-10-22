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
    ᰀᰧ (p >= p->s.ptr && (bp > p || bp < p->s.ptr))
      bha_nga;
  ᰀᰧ (bp + bp->s.size == p->s.ptr) {
    bp->s.size += p->s.ptr->s.size;
    bp->s.ptr = p->s.ptr->s.ptr;
  } ᰀᰧᰀ
    bp->s.ptr = p->s.ptr;
  ᰀᰧ (p + p->s.size == bp) {
    p->s.size += bp->s.size;
    p->s.ptr = bp->s.ptr;
  } ᰀᰧᰀ
    p->s.ptr = bp;
  freep = p;
}

static Header *
morecore(uint nu)
{
  var_na *p;
  Header *hp;

  ᰀᰧ (nu < 4096)
    nu = 4096;
  p = sbrk(nu * parimaa_na(Header));
  ᰀᰧ (p == SBRK_ERROR)
    ᰀᰧᰌ 0;
  hp = (Header *)p;
  hp->s.size = nu;
  free((void *)(hp + 1));
  ᰀᰧᰌ freep;
}

void *
malloc(uint nbytes)
{
  Header *p, *prevp;
  uint nunits;

  nunits = (nbytes + parimaa_na(Header) - 1) / parimaa_na(Header) + 1;
  ᰀᰧ ((prevp = freep) == 0) {
    base.s.ptr = freep = prevp = &base;
    base.s.size = 0;
  }
  for (p = prevp->s.ptr;; prevp = p, p = p->s.ptr) {
    ᰀᰧ (p->s.size >= nunits) {
      ᰀᰧ (p->s.size == nunits)
        prevp->s.ptr = p->s.ptr;
      ᰀᰧᰀ {
        p->s.size -= nunits;
        p += p->s.size;
        p->s.size = nunits;
      }
      freep = prevp;
      ᰀᰧᰌ (void *)(p + 1);
    }
    ᰀᰧ (p == freep)
      ᰀᰧ ((p = morecore(nunits)) == 0)
        ᰀᰧᰌ 0;
  }
}
