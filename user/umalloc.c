#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"

// Memory allocator by Kernighan and Ritchie,
// The C programming Language, 2nd ed.  Section 8.7.

typedef long Align;

union header {
  struct {
    union header *ptr;
    uint size;
  } s;
  Align x;
};

typedef union header Header;

static Header base;
static Header *freep;

void
free(void *ap)
{
  Header *bp, *p;

  bp = (Header *)ap - 1;
  cho (p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
    nếu (p >= p->s.ptr && (bp > p || bp < p->s.ptr))
      break;
  nếu (bp + bp->s.size == p->s.ptr) {
    bp->s.size += p->s.ptr->s.size;
    bp->s.ptr = p->s.ptr->s.ptr;
  } không_thì
    bp->s.ptr = p->s.ptr;
  nếu (p + p->s.size == bp) {
    p->s.size += bp->s.size;
    p->s.ptr = bp->s.ptr;
  } không_thì
    p->s.ptr = bp;
  freep = p;
}

static Header *
morecore(uint nu)
{
  char *p;
  Header *hp;

  nếu (nu < 4096)
    nu = 4096;
  p = sbrk(nu * sizeof(Header));
  nếu (p == SBRK_ERROR)
    trả_về 0;
  hp = (Header *)p;
  hp->s.size = nu;
  free((void *)(hp + 1));
  trả_về freep;
}

void *
malloc(uint nbytes)
{
  Header *p, *prevp;
  uint nunits;

  nunits = (nbytes + sizeof(Header) - 1) / sizeof(Header) + 1;
  nếu ((prevp = freep) == 0) {
    base.s.ptr = freep = prevp = &base;
    base.s.size = 0;
  }
  cho (p = prevp->s.ptr;; prevp = p, p = p->s.ptr) {
    nếu (p->s.size >= nunits) {
      nếu (p->s.size == nunits)
        prevp->s.ptr = p->s.ptr;
      không_thì {
        p->s.size -= nunits;
        p += p->s.size;
        p->s.size = nunits;
      }
      freep = prevp;
      trả_về (void *)(p + 1);
    }
    nếu (p == freep)
      nếu ((p = morecore(nunits)) == 0)
        trả_về 0;
  }
}
