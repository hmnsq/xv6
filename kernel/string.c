#include "types.h"

void *
memset(void *dst, ຈຳນວນເຕັມ c, uint n)
{
  var_na *cdst = (var_na *)dst;
  ຈຳນວນເຕັມ i;
  ສຳລັບ (i = 0; i < n; i++) {
    cdst[i] = c;
  }
  ສົ່ງຄືນ dst;
}

ຈຳນວນເຕັມ
memcmp(const void *v1, const void *v2, uint n)
{
  const uchar *s1, *s2;

  s1 = v1;
  s2 = v2;
  ໃນຂະນະທີ່ (n-- > 0) {
    ຖ້າ (*s1 != *s2)
      ສົ່ງຄືນ *s1 - *s2;
    s1++, s2++;
  }

  ສົ່ງຄືນ 0;
}

void *
memmove(void *dst, const void *src, uint n)
{
  const var_na *s;
  var_na *d;

  ຖ້າ (n == 0)
    ສົ່ງຄືນ dst;

  s = src;
  d = dst;
  ຖ້າ (s < d && s + n > d) {
    s += n;
    d += n;
    ໃນຂະນະທີ່ (n-- > 0)
      *--d = *--s;
  } ບໍ່ດັ່ງນັ້ນ
    ໃນຂະນະທີ່ (n-- > 0)
      *d++ = *s++;

  ສົ່ງຄືນ dst;
}

// memcpy exists to placate GCC.  Use memmove.
void *
memcpy(void *dst, const void *src, uint n)
{
  ສົ່ງຄືນ memmove(dst, src, n);
}

ຈຳນວນເຕັມ
strncmp(const var_na *p, const var_na *q, uint n)
{
  ໃນຂະນະທີ່ (n > 0 && *p && *p == *q)
    n--, p++, q++;
  ຖ້າ (n == 0)
    ສົ່ງຄືນ 0;
  ສົ່ງຄືນ (uchar)*p - (uchar)*q;
}

var_na *
strncpy(var_na *s, const var_na *t, ຈຳນວນເຕັມ n)
{
  var_na *os;

  os = s;
  ໃນຂະນະທີ່ (n-- > 0 && (*s++ = *t++) != 0)
    ;
  ໃນຂະນະທີ່ (n-- > 0)
    *s++ = 0;
  ສົ່ງຄືນ os;
}

// Like strncpy but guaranteed to NUL-terminate.
var_na *
safestrcpy(var_na *s, const var_na *t, ຈຳນວນເຕັມ n)
{
  var_na *os;

  os = s;
  ຖ້າ (n <= 0)
    ສົ່ງຄືນ os;
  ໃນຂະນະທີ່ (--n > 0 && (*s++ = *t++) != 0)
    ;
  *s = 0;
  ສົ່ງຄືນ os;
}

ຈຳນວນເຕັມ
strlen(const var_na *s)
{
  ຈຳນວນເຕັມ n;

  ສຳລັບ (n = 0; s[n]; n++)
    ;
  ສົ່ງຄືນ n;
}
