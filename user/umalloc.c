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
  དོན་ལུ (p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
    གལ་སྲིད (p >= p->s.ptr && (bp > p || bp < p->s.ptr))
      bha_nga;
  གལ་སྲིད (bp + bp->s.size == p->s.ptr) {
    bp->s.size += p->s.ptr->s.size;
    bp->s.ptr = p->s.ptr->s.ptr;
  } དེ་མིན
    bp->s.ptr = p->s.ptr;
  གལ་སྲིད (p + p->s.size == bp) {
    p->s.size += bp->s.size;
    p->s.ptr = bp->s.ptr;
  } དེ་མིན
    p->s.ptr = bp;
  freep = p;
}

static Header *
morecore(uint nu)
{
  var_na *p;
  Header *hp;

  གལ་སྲིད (nu < 4096)
    nu = 4096;
  p = sbrk(nu * parimaa_na(Header));
  གལ་སྲིད (p == SBRK_ERROR)
    ལོག 0;
  hp = (Header *)p;
  hp->s.size = nu;
  free((void *)(hp + 1));
  ལོག freep;
}

void *
malloc(uint nbytes)
{
  Header *p, *prevp;
  uint nunits;

  nunits = (nbytes + parimaa_na(Header) - 1) / parimaa_na(Header) + 1;
  གལ་སྲིད ((prevp = freep) == 0) {
    base.s.ptr = freep = prevp = &base;
    base.s.size = 0;
  }
  དོན་ལུ (p = prevp->s.ptr;; prevp = p, p = p->s.ptr) {
    གལ་སྲིད (p->s.size >= nunits) {
      གལ་སྲིད (p->s.size == nunits)
        prevp->s.ptr = p->s.ptr;
      དེ་མིན {
        p->s.size -= nunits;
        p += p->s.size;
        p->s.size = nunits;
      }
      freep = prevp;
      ལོག (void *)(p + 1);
    }
    གལ་སྲིད (p == freep)
      གལ་སྲིད ((p = morecore(nunits)) == 0)
        ལོག 0;
  }
}
