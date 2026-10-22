#include "types.h"

void *
memset(void *dst, ចំនួនគត់ c, uint n)
{
  var_na *cdst = (var_na *)dst;
  ចំនួនគត់ i;
  សម្រាប់ (i = 0; i < n; i++) {
    cdst[i] = c;
  }
  ត្រឡប់ dst;
}

ចំនួនគត់
memcmp(const void *v1, const void *v2, uint n)
{
  const uchar *s1, *s2;

  s1 = v1;
  s2 = v2;
  ខណៈពេល (n-- > 0) {
    ប្រសិនបើ (*s1 != *s2)
      ត្រឡប់ *s1 - *s2;
    s1++, s2++;
  }

  ត្រឡប់ 0;
}

void *
memmove(void *dst, const void *src, uint n)
{
  const var_na *s;
  var_na *d;

  ប្រសិនបើ (n == 0)
    ត្រឡប់ dst;

  s = src;
  d = dst;
  ប្រសិនបើ (s < d && s + n > d) {
    s += n;
    d += n;
    ខណៈពេល (n-- > 0)
      *--d = *--s;
  } បើមិនដូច្នេះ
    ខណៈពេល (n-- > 0)
      *d++ = *s++;

  ត្រឡប់ dst;
}

// memcpy exists to placate GCC.  Use memmove.
void *
memcpy(void *dst, const void *src, uint n)
{
  ត្រឡប់ memmove(dst, src, n);
}

ចំនួនគត់
strncmp(const var_na *p, const var_na *q, uint n)
{
  ខណៈពេល (n > 0 && *p && *p == *q)
    n--, p++, q++;
  ប្រសិនបើ (n == 0)
    ត្រឡប់ 0;
  ត្រឡប់ (uchar)*p - (uchar)*q;
}

var_na *
strncpy(var_na *s, const var_na *t, ចំនួនគត់ n)
{
  var_na *os;

  os = s;
  ខណៈពេល (n-- > 0 && (*s++ = *t++) != 0)
    ;
  ខណៈពេល (n-- > 0)
    *s++ = 0;
  ត្រឡប់ os;
}

// Like strncpy but guaranteed to NUL-terminate.
var_na *
safestrcpy(var_na *s, const var_na *t, ចំនួនគត់ n)
{
  var_na *os;

  os = s;
  ប្រសិនបើ (n <= 0)
    ត្រឡប់ os;
  ខណៈពេល (--n > 0 && (*s++ = *t++) != 0)
    ;
  *s = 0;
  ត្រឡប់ os;
}

ចំនួនគត់
strlen(const var_na *s)
{
  ចំនួនគត់ n;

  សម្រាប់ (n = 0; s[n]; n++)
    ;
  ត្រឡប់ n;
}
