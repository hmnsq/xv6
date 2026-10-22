#include "types.h"

void *
memset(void *dst, ཧྲིལ c, uint n)
{
  var_na *cdst = (var_na *)dst;
  ཧྲིལ i;
  དོན་ལུ (i = 0; i < n; i++) {
    cdst[i] = c;
  }
  ལོག dst;
}

ཧྲིལ
memcmp(const void *v1, const void *v2, uint n)
{
  const uchar *s1, *s2;

  s1 = v1;
  s2 = v2;
  སྐབས (n-- > 0) {
    གལ་སྲིད (*s1 != *s2)
      ལོག *s1 - *s2;
    s1++, s2++;
  }

  ལོག 0;
}

void *
memmove(void *dst, const void *src, uint n)
{
  const var_na *s;
  var_na *d;

  གལ་སྲིད (n == 0)
    ལོག dst;

  s = src;
  d = dst;
  གལ་སྲིད (s < d && s + n > d) {
    s += n;
    d += n;
    སྐབས (n-- > 0)
      *--d = *--s;
  } དེ་མིན
    སྐབས (n-- > 0)
      *d++ = *s++;

  ལོག dst;
}

// memcpy exists to placate GCC.  Use memmove.
void *
memcpy(void *dst, const void *src, uint n)
{
  ལོག memmove(dst, src, n);
}

ཧྲིལ
strncmp(const var_na *p, const var_na *q, uint n)
{
  སྐབས (n > 0 && *p && *p == *q)
    n--, p++, q++;
  གལ་སྲིད (n == 0)
    ལོག 0;
  ལོག (uchar)*p - (uchar)*q;
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
  ལོག os;
}

// Like strncpy but guaranteed to NUL-terminate.
var_na *
safestrcpy(var_na *s, const var_na *t, ཧྲིལ n)
{
  var_na *os;

  os = s;
  གལ་སྲིད (n <= 0)
    ལོག os;
  སྐབས (--n > 0 && (*s++ = *t++) != 0)
    ;
  *s = 0;
  ལོག os;
}

ཧྲིལ
strlen(const var_na *s)
{
  ཧྲིལ n;

  དོན་ལུ (n = 0; s[n]; n++)
    ;
  ལོག n;
}
