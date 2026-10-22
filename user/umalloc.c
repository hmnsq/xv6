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
  для (p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
    если (p >= p->s.ptr && (bp > p || bp < p->s.ptr))
      break;
  если (bp + bp->s.size == p->s.ptr) {
    bp->s.size += p->s.ptr->s.size;
    bp->s.ptr = p->s.ptr->s.ptr;
  } иначе
    bp->s.ptr = p->s.ptr;
  если (p + p->s.size == bp) {
    p->s.size += bp->s.size;
    p->s.ptr = bp->s.ptr;
  } иначе
    p->s.ptr = bp;
  freep = p;
}

static Header *
morecore(uint nu)
{
  char *p;
  Header *hp;

  если (nu < 4096)
    nu = 4096;
  p = sbrk(nu * sizeof(Header));
  если (p == SBRK_ERROR)
    вернуть 0;
  hp = (Header *)p;
  hp->s.size = nu;
  free((void *)(hp + 1));
  вернуть freep;
}

void *
malloc(uint nbytes)
{
  Header *p, *prevp;
  uint nunits;

  nunits = (nbytes + sizeof(Header) - 1) / sizeof(Header) + 1;
  если ((prevp = freep) == 0) {
    base.s.ptr = freep = prevp = &base;
    base.s.size = 0;
  }
  для (p = prevp->s.ptr;; prevp = p, p = p->s.ptr) {
    если (p->s.size >= nunits) {
      если (p->s.size == nunits)
        prevp->s.ptr = p->s.ptr;
      иначе {
        p->s.size -= nunits;
        p += p->s.size;
        p->s.size = nunits;
      }
      freep = prevp;
      вернуть (void *)(p + 1);
    }
    если (p == freep)
      если ((p = morecore(nunits)) == 0)
        вернуть 0;
  }
}
