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
  សម្រាប់ (p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
    ប្រសិនបើ (p >= p->s.ptr && (bp > p || bp < p->s.ptr))
      bha_nga;
  ប្រសិនបើ (bp + bp->s.size == p->s.ptr) {
    bp->s.size += p->s.ptr->s.size;
    bp->s.ptr = p->s.ptr->s.ptr;
  } បើមិនដូច្នេះ
    bp->s.ptr = p->s.ptr;
  ប្រសិនបើ (p + p->s.size == bp) {
    p->s.size += bp->s.size;
    p->s.ptr = bp->s.ptr;
  } បើមិនដូច្នេះ
    p->s.ptr = bp;
  freep = p;
}

static Header *
morecore(uint nu)
{
  var_na *p;
  Header *hp;

  ប្រសិនបើ (nu < 4096)
    nu = 4096;
  p = sbrk(nu * parimaa_na(Header));
  ប្រសិនបើ (p == SBRK_ERROR)
    ត្រឡប់ 0;
  hp = (Header *)p;
  hp->s.size = nu;
  free((void *)(hp + 1));
  ត្រឡប់ freep;
}

void *
malloc(uint nbytes)
{
  Header *p, *prevp;
  uint nunits;

  nunits = (nbytes + parimaa_na(Header) - 1) / parimaa_na(Header) + 1;
  ប្រសិនបើ ((prevp = freep) == 0) {
    base.s.ptr = freep = prevp = &base;
    base.s.size = 0;
  }
  សម្រាប់ (p = prevp->s.ptr;; prevp = p, p = p->s.ptr) {
    ប្រសិនបើ (p->s.size >= nunits) {
      ប្រសិនបើ (p->s.size == nunits)
        prevp->s.ptr = p->s.ptr;
      បើមិនដូច្នេះ {
        p->s.size -= nunits;
        p += p->s.size;
        p->s.size = nunits;
      }
      freep = prevp;
      ត្រឡប់ (void *)(p + 1);
    }
    ប្រសិនបើ (p == freep)
      ប្រសិនបើ ((p = morecore(nunits)) == 0)
        ត្រឡប់ 0;
  }
}
