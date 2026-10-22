#include "types.h"

void *
memset(void *dst, puur_naa_nka c, uint n)
{
  var_na *cdst = (var_na *)dst;
  puur_naa_nka i;
  for (i = 0; i < n; i++) {
    cdst[i] = c;
  }
  ᤀᤡᤴ dst;
}

puur_naa_nka
memcmp(const void *v1, const void *v2, uint n)
{
  const uchar *s1, *s2;

  s1 = v1;
  s2 = v2;
  ᤀᤠ᤺ (n-- > 0) {
    ᤀᤡ᤺ (*s1 != *s2)
      ᤀᤡᤴ *s1 - *s2;
    s1++, s2++;
  }

  ᤀᤡᤴ 0;
}

void *
memmove(void *dst, const void *src, uint n)
{
  const var_na *s;
  var_na *d;

  ᤀᤡ᤺ (n == 0)
    ᤀᤡᤴ dst;

  s = src;
  d = dst;
  ᤀᤡ᤺ (s < d && s + n > d) {
    s += n;
    d += n;
    ᤀᤠ᤺ (n-- > 0)
      *--d = *--s;
  } ᤀᤡ᤺ᤆᤡ
    ᤀᤠ᤺ (n-- > 0)
      *d++ = *s++;

  ᤀᤡᤴ dst;
}

// memcpy exists to placate GCC.  Use memmove.
void *
memcpy(void *dst, const void *src, uint n)
{
  ᤀᤡᤴ memmove(dst, src, n);
}

puur_naa_nka
strncmp(const var_na *p, const var_na *q, uint n)
{
  ᤀᤠ᤺ (n > 0 && *p && *p == *q)
    n--, p++, q++;
  ᤀᤡ᤺ (n == 0)
    ᤀᤡᤴ 0;
  ᤀᤡᤴ (uchar)*p - (uchar)*q;
}

var_na *
strncpy(var_na *s, const var_na *t, puur_naa_nka n)
{
  var_na *os;

  os = s;
  ᤀᤠ᤺ (n-- > 0 && (*s++ = *t++) != 0)
    ;
  ᤀᤠ᤺ (n-- > 0)
    *s++ = 0;
  ᤀᤡᤴ os;
}

// Like strncpy but guaranteed to NUL-terminate.
var_na *
safestrcpy(var_na *s, const var_na *t, puur_naa_nka n)
{
  var_na *os;

  os = s;
  ᤀᤡ᤺ (n <= 0)
    ᤀᤡᤴ os;
  ᤀᤠ᤺ (--n > 0 && (*s++ = *t++) != 0)
    ;
  *s = 0;
  ᤀᤡᤴ os;
}

puur_naa_nka
strlen(const var_na *s)
{
  puur_naa_nka n;

  for (n = 0; s[n]; n++)
    ;
  ᤀᤡᤴ n;
}
