#include "types.h"

void *
memset(void *dst, puur_naa_nka c, uint n)
{
  var_na *cdst = (var_na *)dst;
  puur_naa_nka i;
  for (i = 0; i < n; i++) {
    cdst[i] = c;
  }
  ᱨᱩᱣᱟᱹᱲ dst;
}

puur_naa_nka
memcmp(const void *v1, const void *v2, uint n)
{
  const uchar *s1, *s2;

  s1 = v1;
  s2 = v2;
  ᱡᱚᱠᱷᱚᱱ (n-- > 0) {
    ᱡᱩᱫᱤ (*s1 != *s2)
      ᱨᱩᱣᱟᱹᱲ *s1 - *s2;
    s1++, s2++;
  }

  ᱨᱩᱣᱟᱹᱲ 0;
}

void *
memmove(void *dst, const void *src, uint n)
{
  const var_na *s;
  var_na *d;

  ᱡᱩᱫᱤ (n == 0)
    ᱨᱩᱣᱟᱹᱲ dst;

  s = src;
  d = dst;
  ᱡᱩᱫᱤ (s < d && s + n > d) {
    s += n;
    d += n;
    ᱡᱚᱠᱷᱚᱱ (n-- > 0)
      *--d = *--s;
  } ᱵᱟᱝ
    ᱡᱚᱠᱷᱚᱱ (n-- > 0)
      *d++ = *s++;

  ᱨᱩᱣᱟᱹᱲ dst;
}

// memcpy exists to placate GCC.  Use memmove.
void *
memcpy(void *dst, const void *src, uint n)
{
  ᱨᱩᱣᱟᱹᱲ memmove(dst, src, n);
}

puur_naa_nka
strncmp(const var_na *p, const var_na *q, uint n)
{
  ᱡᱚᱠᱷᱚᱱ (n > 0 && *p && *p == *q)
    n--, p++, q++;
  ᱡᱩᱫᱤ (n == 0)
    ᱨᱩᱣᱟᱹᱲ 0;
  ᱨᱩᱣᱟᱹᱲ (uchar)*p - (uchar)*q;
}

var_na *
strncpy(var_na *s, const var_na *t, puur_naa_nka n)
{
  var_na *os;

  os = s;
  ᱡᱚᱠᱷᱚᱱ (n-- > 0 && (*s++ = *t++) != 0)
    ;
  ᱡᱚᱠᱷᱚᱱ (n-- > 0)
    *s++ = 0;
  ᱨᱩᱣᱟᱹᱲ os;
}

// Like strncpy but guaranteed to NUL-terminate.
var_na *
safestrcpy(var_na *s, const var_na *t, puur_naa_nka n)
{
  var_na *os;

  os = s;
  ᱡᱩᱫᱤ (n <= 0)
    ᱨᱩᱣᱟᱹᱲ os;
  ᱡᱚᱠᱷᱚᱱ (--n > 0 && (*s++ = *t++) != 0)
    ;
  *s = 0;
  ᱨᱩᱣᱟᱹᱲ os;
}

puur_naa_nka
strlen(const var_na *s)
{
  puur_naa_nka n;

  for (n = 0; s[n]; n++)
    ;
  ᱨᱩᱣᱟᱹᱲ n;
}
