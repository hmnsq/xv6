#include "types.h"

void *
memset(void *dst, puur_naa_nka c, uint n)
{
  var_na *cdst = (var_na *)dst;
  puur_naa_nka i;
  for (i = 0; i < n; i++) {
    cdst[i] = c;
  }
  ᬯᬮᬶ dst;
}

puur_naa_nka
memcmp(const void *v1, const void *v2, uint n)
{
  const uchar *s1, *s2;

  s1 = v1;
  s2 = v2;
  ᬲᭂᬤᭂᬮ (n-- > 0) {
    ᬬᬾᬦ᭄ (*s1 != *s2)
      ᬯᬮᬶ *s1 - *s2;
    s1++, s2++;
  }

  ᬯᬮᬶ 0;
}

void *
memmove(void *dst, const void *src, uint n)
{
  const var_na *s;
  var_na *d;

  ᬬᬾᬦ᭄ (n == 0)
    ᬯᬮᬶ dst;

  s = src;
  d = dst;
  ᬬᬾᬦ᭄ (s < d && s + n > d) {
    s += n;
    d += n;
    ᬲᭂᬤᭂᬮ (n-- > 0)
      *--d = *--s;
  } ᬮᬶᬬ
    ᬲᭂᬤᭂᬮ (n-- > 0)
      *d++ = *s++;

  ᬯᬮᬶ dst;
}

// memcpy exists to placate GCC.  Use memmove.
void *
memcpy(void *dst, const void *src, uint n)
{
  ᬯᬮᬶ memmove(dst, src, n);
}

puur_naa_nka
strncmp(const var_na *p, const var_na *q, uint n)
{
  ᬲᭂᬤᭂᬮ (n > 0 && *p && *p == *q)
    n--, p++, q++;
  ᬬᬾᬦ᭄ (n == 0)
    ᬯᬮᬶ 0;
  ᬯᬮᬶ (uchar)*p - (uchar)*q;
}

var_na *
strncpy(var_na *s, const var_na *t, puur_naa_nka n)
{
  var_na *os;

  os = s;
  ᬲᭂᬤᭂᬮ (n-- > 0 && (*s++ = *t++) != 0)
    ;
  ᬲᭂᬤᭂᬮ (n-- > 0)
    *s++ = 0;
  ᬯᬮᬶ os;
}

// Like strncpy but guaranteed to NUL-terminate.
var_na *
safestrcpy(var_na *s, const var_na *t, puur_naa_nka n)
{
  var_na *os;

  os = s;
  ᬬᬾᬦ᭄ (n <= 0)
    ᬯᬮᬶ os;
  ᬲᭂᬤᭂᬮ (--n > 0 && (*s++ = *t++) != 0)
    ;
  *s = 0;
  ᬯᬮᬶ os;
}

puur_naa_nka
strlen(const var_na *s)
{
  puur_naa_nka n;

  for (n = 0; s[n]; n++)
    ;
  ᬯᬮᬶ n;
}
