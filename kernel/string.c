#include "types.h"

void *
memset(void *dst, ཧྲིལ c, uint n)
{
  var_na *cdst = (var_na *)dst;
  ཧྲིལ i;
  ཆེད་དུ (i = 0; i < n; i++) {
    cdst[i] = c;
  }
  ཕྱིར dst;
}

ཧྲིལ
memcmp(const void *v1, const void *v2, uint n)
{
  const uchar *s1, *s2;

  s1 = v1;
  s2 = v2;
  སྐབས (n-- > 0) {
    གལ་ཏེ (*s1 != *s2)
      ཕྱིར *s1 - *s2;
    s1++, s2++;
  }

  ཕྱིར 0;
}

void *
memmove(void *dst, const void *src, uint n)
{
  const var_na *s;
  var_na *d;

  གལ་ཏེ (n == 0)
    ཕྱིར dst;

  s = src;
  d = dst;
  གལ་ཏེ (s < d && s + n > d) {
    s += n;
    d += n;
    སྐབས (n-- > 0)
      *--d = *--s;
  } དེ་མིན
    སྐབས (n-- > 0)
      *d++ = *s++;

  ཕྱིར dst;
}

// memcpy exists to placate GCC.  Use memmove.
void *
memcpy(void *dst, const void *src, uint n)
{
  ཕྱིར memmove(dst, src, n);
}

ཧྲིལ
strncmp(const var_na *p, const var_na *q, uint n)
{
  སྐབས (n > 0 && *p && *p == *q)
    n--, p++, q++;
  གལ་ཏེ (n == 0)
    ཕྱིར 0;
  ཕྱིར (uchar)*p - (uchar)*q;
}

var_na *
strncpy(var_na *s, const var_na *t, ཧྲིལ n)
{
  var_na *os;

  os = s;
  སྐབས (n-- > 0 && (*s++ = *t++) != 0)
    ;
  སྐབས (n-- > 0)
    *s++ = 0;
  ཕྱིར os;
}

// Like strncpy but guaranteed to NUL-terminate.
var_na *
safestrcpy(var_na *s, const var_na *t, ཧྲིལ n)
{
  var_na *os;

  os = s;
  གལ་ཏེ (n <= 0)
    ཕྱིར os;
  སྐབས (--n > 0 && (*s++ = *t++) != 0)
    ;
  *s = 0;
  ཕྱིར os;
}

ཧྲིལ
strlen(const var_na *s)
{
  ཧྲིལ n;

  ཆེད་དུ (n = 0; s[n]; n++)
    ;
  ཕྱིར n;
}
