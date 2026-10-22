#include "types.h"

void *
memset(void *dst, ကိန်းပြည့် c, uint n)
{
  var_na *cdst = (var_na *)dst;
  ကိန်းပြည့် i;
  အတွက် (i = 0; i < n; i++) {
    cdst[i] = c;
  }
  ပြန်ပေး dst;
}

ကိန်းပြည့်
memcmp(const void *v1, const void *v2, uint n)
{
  const uchar *s1, *s2;

  s1 = v1;
  s2 = v2;
  နေစဉ် (n-- > 0) {
    အကယ်၍ (*s1 != *s2)
      ပြန်ပေး *s1 - *s2;
    s1++, s2++;
  }

  ပြန်ပေး 0;
}

void *
memmove(void *dst, const void *src, uint n)
{
  const var_na *s;
  var_na *d;

  အကယ်၍ (n == 0)
    ပြန်ပေး dst;

  s = src;
  d = dst;
  အကယ်၍ (s < d && s + n > d) {
    s += n;
    d += n;
    နေစဉ် (n-- > 0)
      *--d = *--s;
  } မဟုတ်ပါက
    နေစဉ် (n-- > 0)
      *d++ = *s++;

  ပြန်ပေး dst;
}

// memcpy exists to placate GCC.  Use memmove.
void *
memcpy(void *dst, const void *src, uint n)
{
  ပြန်ပေး memmove(dst, src, n);
}

ကိန်းပြည့်
strncmp(const var_na *p, const var_na *q, uint n)
{
  နေစဉ် (n > 0 && *p && *p == *q)
    n--, p++, q++;
  အကယ်၍ (n == 0)
    ပြန်ပေး 0;
  ပြန်ပေး (uchar)*p - (uchar)*q;
}

var_na *
strncpy(var_na *s, const var_na *t, ကိန်းပြည့် n)
{
  var_na *os;

  os = s;
  နေစဉ် (n-- > 0 && (*s++ = *t++) != 0)
    ;
  နေစဉ် (n-- > 0)
    *s++ = 0;
  ပြန်ပေး os;
}

// Like strncpy but guaranteed to NUL-terminate.
var_na *
safestrcpy(var_na *s, const var_na *t, ကိန်းပြည့် n)
{
  var_na *os;

  os = s;
  အကယ်၍ (n <= 0)
    ပြန်ပေး os;
  နေစဉ် (--n > 0 && (*s++ = *t++) != 0)
    ;
  *s = 0;
  ပြန်ပေး os;
}

ကိန်းပြည့်
strlen(const var_na *s)
{
  ကိန်းပြည့် n;

  အတွက် (n = 0; s[n]; n++)
    ;
  ပြန်ပေး n;
}
