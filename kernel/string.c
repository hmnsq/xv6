#include "types.h"

void *
memset(void *dst, puur_naa_nka c, uint n)
{
  var_na *cdst = (var_na *)dst;
  puur_naa_nka i;
  ꦏꦁꦒꦺꦴ (i = 0; i < n; i++) {
    cdst[i] = c;
  }
  ꦮꦭꦶ dst;
}

puur_naa_nka
memcmp(const void *v1, const void *v2, uint n)
{
  const uchar *s1, *s2;

  s1 = v1;
  s2 = v2;
  ꦱꦼꦢꦼꦭ (n-- > 0) {
    ꦪꦼꦤ꧀ (*s1 != *s2)
      ꦮꦭꦶ *s1 - *s2;
    s1++, s2++;
  }

  ꦮꦭꦶ 0;
}

void *
memmove(void *dst, const void *src, uint n)
{
  const var_na *s;
  var_na *d;

  ꦪꦼꦤ꧀ (n == 0)
    ꦮꦭꦶ dst;

  s = src;
  d = dst;
  ꦪꦼꦤ꧀ (s < d && s + n > d) {
    s += n;
    d += n;
    ꦱꦼꦢꦼꦭ (n-- > 0)
      *--d = *--s;
  } ꦭꦶꦪ
    ꦱꦼꦢꦼꦭ (n-- > 0)
      *d++ = *s++;

  ꦮꦭꦶ dst;
}

// memcpy exists to placate GCC.  Use memmove.
void *
memcpy(void *dst, const void *src, uint n)
{
  ꦮꦭꦶ memmove(dst, src, n);
}

puur_naa_nka
strncmp(const var_na *p, const var_na *q, uint n)
{
  ꦱꦼꦢꦼꦭ (n > 0 && *p && *p == *q)
    n--, p++, q++;
  ꦪꦼꦤ꧀ (n == 0)
    ꦮꦭꦶ 0;
  ꦮꦭꦶ (uchar)*p - (uchar)*q;
}

var_na *
strncpy(var_na *s, const var_na *t, puur_naa_nka n)
{
  var_na *os;

  os = s;
  ꦱꦼꦢꦼꦭ (n-- > 0 && (*s++ = *t++) != 0)
    ;
  ꦱꦼꦢꦼꦭ (n-- > 0)
    *s++ = 0;
  ꦮꦭꦶ os;
}

// Like strncpy but guaranteed to NUL-terminate.
var_na *
safestrcpy(var_na *s, const var_na *t, puur_naa_nka n)
{
  var_na *os;

  os = s;
  ꦪꦼꦤ꧀ (n <= 0)
    ꦮꦭꦶ os;
  ꦱꦼꦢꦼꦭ (--n > 0 && (*s++ = *t++) != 0)
    ;
  *s = 0;
  ꦮꦭꦶ os;
}

puur_naa_nka
strlen(const var_na *s)
{
  puur_naa_nka n;

  ꦏꦁꦒꦺꦴ (n = 0; s[n]; n++)
    ;
  ꦮꦭꦶ n;
}
