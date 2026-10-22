#include "types.h"

void *
memset(void *dst, puur_naa_nka c, uint n)
{
  var_na *cdst = (var_na *)dst;
  puur_naa_nka i;
  for (i = 0; i < n; i++) {
    cdst[i] = c;
  }
  ᯀᯮᯞ dst;
}

puur_naa_nka
memcmp(const void *v1, const void *v2, uint n)
{
  const uchar *s1, *s2;

  s1 = v1;
  s2 = v2;
  ᯀᯬᯝ (n-- > 0) {
    ᯀᯮᯝ (*s1 != *s2)
      ᯀᯮᯞ *s1 - *s2;
    s1++, s2++;
  }

  ᯀᯮᯞ 0;
}

void *
memmove(void *dst, const void *src, uint n)
{
  const var_na *s;
  var_na *d;

  ᯀᯮᯝ (n == 0)
    ᯀᯮᯞ dst;

  s = src;
  d = dst;
  ᯀᯮᯝ (s < d && s + n > d) {
    s += n;
    d += n;
    ᯀᯬᯝ (n-- > 0)
      *--d = *--s;
  } ᯀᯮᯝᯂᯪ
    ᯀᯬᯝ (n-- > 0)
      *d++ = *s++;

  ᯀᯮᯞ dst;
}

// memcpy exists to placate GCC.  Use memmove.
void *
memcpy(void *dst, const void *src, uint n)
{
  ᯀᯮᯞ memmove(dst, src, n);
}

puur_naa_nka
strncmp(const var_na *p, const var_na *q, uint n)
{
  ᯀᯬᯝ (n > 0 && *p && *p == *q)
    n--, p++, q++;
  ᯀᯮᯝ (n == 0)
    ᯀᯮᯞ 0;
  ᯀᯮᯞ (uchar)*p - (uchar)*q;
}

var_na *
strncpy(var_na *s, const var_na *t, puur_naa_nka n)
{
  var_na *os;

  os = s;
  ᯀᯬᯝ (n-- > 0 && (*s++ = *t++) != 0)
    ;
  ᯀᯬᯝ (n-- > 0)
    *s++ = 0;
  ᯀᯮᯞ os;
}

// Like strncpy but guaranteed to NUL-terminate.
var_na *
safestrcpy(var_na *s, const var_na *t, puur_naa_nka n)
{
  var_na *os;

  os = s;
  ᯀᯮᯝ (n <= 0)
    ᯀᯮᯞ os;
  ᯀᯬᯝ (--n > 0 && (*s++ = *t++) != 0)
    ;
  *s = 0;
  ᯀᯮᯞ os;
}

puur_naa_nka
strlen(const var_na *s)
{
  puur_naa_nka n;

  for (n = 0; s[n]; n++)
    ;
  ᯀᯮᯞ n;
}
