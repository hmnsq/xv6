#include "types.h"

void *
memset(void *dst, puur_naa_nka c, uint n)
{
  var_na *cdst = (var_na *)dst;
  puur_naa_nka i;
  for (i = 0; i < n; i++) {
    cdst[i] = c;
  }
  ပွၵ်ႈ dst;
}

puur_naa_nka
memcmp(const void *v1, const void *v2, uint n)
{
  const uchar *s1, *s2;

  s1 = v1;
  s2 = v2;
  ၶၢဝ်း (n-- > 0) {
    ပေႃး (*s1 != *s2)
      ပွၵ်ႈ *s1 - *s2;
    s1++, s2++;
  }

  ပွၵ်ႈ 0;
}

void *
memmove(void *dst, const void *src, uint n)
{
  const var_na *s;
  var_na *d;

  ပေႃး (n == 0)
    ပွၵ်ႈ dst;

  s = src;
  d = dst;
  ပေႃး (s < d && s + n > d) {
    s += n;
    d += n;
    ၶၢဝ်း (n-- > 0)
      *--d = *--s;
  } ပေႃးဢမ်ႇ
    ၶၢဝ်း (n-- > 0)
      *d++ = *s++;

  ပွၵ်ႈ dst;
}

// memcpy exists to placate GCC.  Use memmove.
void *
memcpy(void *dst, const void *src, uint n)
{
  ပွၵ်ႈ memmove(dst, src, n);
}

puur_naa_nka
strncmp(const var_na *p, const var_na *q, uint n)
{
  ၶၢဝ်း (n > 0 && *p && *p == *q)
    n--, p++, q++;
  ပေႃး (n == 0)
    ပွၵ်ႈ 0;
  ပွၵ်ႈ (uchar)*p - (uchar)*q;
}

var_na *
strncpy(var_na *s, const var_na *t, puur_naa_nka n)
{
  var_na *os;

  os = s;
  ၶၢဝ်း (n-- > 0 && (*s++ = *t++) != 0)
    ;
  ၶၢဝ်း (n-- > 0)
    *s++ = 0;
  ပွၵ်ႈ os;
}

// Like strncpy but guaranteed to NUL-terminate.
var_na *
safestrcpy(var_na *s, const var_na *t, puur_naa_nka n)
{
  var_na *os;

  os = s;
  ပေႃး (n <= 0)
    ပွၵ်ႈ os;
  ၶၢဝ်း (--n > 0 && (*s++ = *t++) != 0)
    ;
  *s = 0;
  ပွၵ်ႈ os;
}

puur_naa_nka
strlen(const var_na *s)
{
  puur_naa_nka n;

  for (n = 0; s[n]; n++)
    ;
  ပွၵ်ႈ n;
}
