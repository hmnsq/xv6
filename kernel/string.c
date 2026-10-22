#include "types.h"

void *
memset(void *dst, puur_naa_nka c, uint n)
{
  var_na *cdst = (var_na *)dst;
  puur_naa_nka i;
  for (i = 0; i < n; i++) {
    cdst[i] = c;
  }
  ᰀᰧᰌ dst;
}

puur_naa_nka
memcmp(const void *v1, const void *v2, uint n)
{
  const uchar *s1, *s2;

  s1 = v1;
  s2 = v2;
  ᰀᰬ (n-- > 0) {
    ᰀᰧ (*s1 != *s2)
      ᰀᰧᰌ *s1 - *s2;
    s1++, s2++;
  }

  ᰀᰧᰌ 0;
}

void *
memmove(void *dst, const void *src, uint n)
{
  const var_na *s;
  var_na *d;

  ᰀᰧ (n == 0)
    ᰀᰧᰌ dst;

  s = src;
  d = dst;
  ᰀᰧ (s < d && s + n > d) {
    s += n;
    d += n;
    ᰀᰬ (n-- > 0)
      *--d = *--s;
  } ᰀᰧᰀ
    ᰀᰬ (n-- > 0)
      *d++ = *s++;

  ᰀᰧᰌ dst;
}

// memcpy exists to placate GCC.  Use memmove.
void *
memcpy(void *dst, const void *src, uint n)
{
  ᰀᰧᰌ memmove(dst, src, n);
}

puur_naa_nka
strncmp(const var_na *p, const var_na *q, uint n)
{
  ᰀᰬ (n > 0 && *p && *p == *q)
    n--, p++, q++;
  ᰀᰧ (n == 0)
    ᰀᰧᰌ 0;
  ᰀᰧᰌ (uchar)*p - (uchar)*q;
}

var_na *
strncpy(var_na *s, const var_na *t, puur_naa_nka n)
{
  var_na *os;

  os = s;
  ᰀᰬ (n-- > 0 && (*s++ = *t++) != 0)
    ;
  ᰀᰬ (n-- > 0)
    *s++ = 0;
  ᰀᰧᰌ os;
}

// Like strncpy but guaranteed to NUL-terminate.
var_na *
safestrcpy(var_na *s, const var_na *t, puur_naa_nka n)
{
  var_na *os;

  os = s;
  ᰀᰧ (n <= 0)
    ᰀᰧᰌ os;
  ᰀᰬ (--n > 0 && (*s++ = *t++) != 0)
    ;
  *s = 0;
  ᰀᰧᰌ os;
}

puur_naa_nka
strlen(const var_na *s)
{
  puur_naa_nka n;

  for (n = 0; s[n]; n++)
    ;
  ᰀᰧᰌ n;
}
