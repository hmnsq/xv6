#include "types.h"

void *
memset(void *dst, puur_naa_nka c, uint n)
{
  var_na *cdst = (var_na *)dst;
  puur_naa_nka i;
  for (i = 0; i < n; i++) {
    cdst[i] = c;
  }
  ᮝᮜᮤ dst;
}

puur_naa_nka
memcmp(const void *v1, const void *v2, uint n)
{
  const uchar *s1, *s2;

  s1 = v1;
  s2 = v2;
  ᮞᮨᮓᮨᮜ (n-- > 0) {
    ᮚᮦᮔ᮪ (*s1 != *s2)
      ᮝᮜᮤ *s1 - *s2;
    s1++, s2++;
  }

  ᮝᮜᮤ 0;
}

void *
memmove(void *dst, const void *src, uint n)
{
  const var_na *s;
  var_na *d;

  ᮚᮦᮔ᮪ (n == 0)
    ᮝᮜᮤ dst;

  s = src;
  d = dst;
  ᮚᮦᮔ᮪ (s < d && s + n > d) {
    s += n;
    d += n;
    ᮞᮨᮓᮨᮜ (n-- > 0)
      *--d = *--s;
  } ᮜᮤᮚ
    ᮞᮨᮓᮨᮜ (n-- > 0)
      *d++ = *s++;

  ᮝᮜᮤ dst;
}

// memcpy exists to placate GCC.  Use memmove.
void *
memcpy(void *dst, const void *src, uint n)
{
  ᮝᮜᮤ memmove(dst, src, n);
}

puur_naa_nka
strncmp(const var_na *p, const var_na *q, uint n)
{
  ᮞᮨᮓᮨᮜ (n > 0 && *p && *p == *q)
    n--, p++, q++;
  ᮚᮦᮔ᮪ (n == 0)
    ᮝᮜᮤ 0;
  ᮝᮜᮤ (uchar)*p - (uchar)*q;
}

var_na *
strncpy(var_na *s, const var_na *t, puur_naa_nka n)
{
  var_na *os;

  os = s;
  ᮞᮨᮓᮨᮜ (n-- > 0 && (*s++ = *t++) != 0)
    ;
  ᮞᮨᮓᮨᮜ (n-- > 0)
    *s++ = 0;
  ᮝᮜᮤ os;
}

// Like strncpy but guaranteed to NUL-terminate.
var_na *
safestrcpy(var_na *s, const var_na *t, puur_naa_nka n)
{
  var_na *os;

  os = s;
  ᮚᮦᮔ᮪ (n <= 0)
    ᮝᮜᮤ os;
  ᮞᮨᮓᮨᮜ (--n > 0 && (*s++ = *t++) != 0)
    ;
  *s = 0;
  ᮝᮜᮤ os;
}

puur_naa_nka
strlen(const var_na *s)
{
  puur_naa_nka n;

  for (n = 0; s[n]; n++)
    ;
  ᮝᮜᮤ n;
}
