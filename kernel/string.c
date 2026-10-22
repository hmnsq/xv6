#include "types.h"

void *
memset(void *dst, puur_naa_nka c, uint n)
{
  var_na *cdst = (var_na *)dst;
  puur_naa_nka i;
  for (i = 0; i < n; i++) {
    cdst[i] = c;
  }
  ᨀᨘᨒ dst;
}

puur_naa_nka
memcmp(const void *v1, const void *v2, uint n)
{
  const uchar *s1, *s2;

  s1 = v1;
  s2 = v2;
  ᨀᨛ (n-- > 0) {
    ᨀᨘ (*s1 != *s2)
      ᨀᨘᨒ *s1 - *s2;
    s1++, s2++;
  }

  ᨀᨘᨒ 0;
}

void *
memmove(void *dst, const void *src, uint n)
{
  const var_na *s;
  var_na *d;

  ᨀᨘ (n == 0)
    ᨀᨘᨒ dst;

  s = src;
  d = dst;
  ᨀᨘ (s < d && s + n > d) {
    s += n;
    d += n;
    ᨀᨛ (n-- > 0)
      *--d = *--s;
  } ᨀᨘᨁ
    ᨀᨛ (n-- > 0)
      *d++ = *s++;

  ᨀᨘᨒ dst;
}

// memcpy exists to placate GCC.  Use memmove.
void *
memcpy(void *dst, const void *src, uint n)
{
  ᨀᨘᨒ memmove(dst, src, n);
}

puur_naa_nka
strncmp(const var_na *p, const var_na *q, uint n)
{
  ᨀᨛ (n > 0 && *p && *p == *q)
    n--, p++, q++;
  ᨀᨘ (n == 0)
    ᨀᨘᨒ 0;
  ᨀᨘᨒ (uchar)*p - (uchar)*q;
}

var_na *
strncpy(var_na *s, const var_na *t, puur_naa_nka n)
{
  var_na *os;

  os = s;
  ᨀᨛ (n-- > 0 && (*s++ = *t++) != 0)
    ;
  ᨀᨛ (n-- > 0)
    *s++ = 0;
  ᨀᨘᨒ os;
}

// Like strncpy but guaranteed to NUL-terminate.
var_na *
safestrcpy(var_na *s, const var_na *t, puur_naa_nka n)
{
  var_na *os;

  os = s;
  ᨀᨘ (n <= 0)
    ᨀᨘᨒ os;
  ᨀᨛ (--n > 0 && (*s++ = *t++) != 0)
    ;
  *s = 0;
  ᨀᨘᨒ os;
}

puur_naa_nka
strlen(const var_na *s)
{
  puur_naa_nka n;

  for (n = 0; s[n]; n++)
    ;
  ᨀᨘᨒ n;
}
