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
  为 (p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
    如果 (p >= p->s.ptr && (bp > p || bp < p->s.ptr))
      break;
  如果 (bp + bp->s.size == p->s.ptr) {
    bp->s.size += p->s.ptr->s.size;
    bp->s.ptr = p->s.ptr->s.ptr;
  } 否则
    bp->s.ptr = p->s.ptr;
  如果 (p + p->s.size == bp) {
    p->s.size += bp->s.size;
    p->s.ptr = bp->s.ptr;
  } 否则
    p->s.ptr = bp;
  freep = p;
}

static Header *
morecore(uint nu)
{
  char *p;
  Header *hp;

  如果 (nu < 4096)
    nu = 4096;
  p = sbrk(nu * sizeof(Header));
  如果 (p == SBRK_ERROR)
    返回 0;
  hp = (Header *)p;
  hp->s.size = nu;
  free((void *)(hp + 1));
  返回 freep;
}

void *
malloc(uint nbytes)
{
  Header *p, *prevp;
  uint nunits;

  nunits = (nbytes + sizeof(Header) - 1) / sizeof(Header) + 1;
  如果 ((prevp = freep) == 0) {
    base.s.ptr = freep = prevp = &base;
    base.s.size = 0;
  }
  为 (p = prevp->s.ptr;; prevp = p, p = p->s.ptr) {
    如果 (p->s.size >= nunits) {
      如果 (p->s.size == nunits)
        prevp->s.ptr = p->s.ptr;
      否则 {
        p->s.size -= nunits;
        p += p->s.size;
        p->s.size = nunits;
      }
      freep = prevp;
      返回 (void *)(p + 1);
    }
    如果 (p == freep)
      如果 ((p = morecore(nunits)) == 0)
        返回 0;
  }
}
