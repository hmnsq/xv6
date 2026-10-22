#include "types.h"

void *
memset(void *dst, จำนวนเต็ม c, uint n)
{
  var_na *cdst = (var_na *)dst;
  จำนวนเต็ม i;
  สำหรับ (i = 0; i < n; i++) {
    cdst[i] = c;
  }
  คืน dst;
}

จำนวนเต็ม
memcmp(const void *v1, const void *v2, uint n)
{
  const uchar *s1, *s2;

  s1 = v1;
  s2 = v2;
  ขณะ (n-- > 0) {
    ถ้า (*s1 != *s2)
      คืน *s1 - *s2;
    s1++, s2++;
  }

  คืน 0;
}

void *
memmove(void *dst, const void *src, uint n)
{
  const var_na *s;
  var_na *d;

  ถ้า (n == 0)
    คืน dst;

  s = src;
  d = dst;
  ถ้า (s < d && s + n > d) {
    s += n;
    d += n;
    ขณะ (n-- > 0)
      *--d = *--s;
  } มิฉะนั้น
    ขณะ (n-- > 0)
      *d++ = *s++;

  คืน dst;
}

// memcpy exists to placate GCC.  Use memmove.
void *
memcpy(void *dst, const void *src, uint n)
{
  คืน memmove(dst, src, n);
}

จำนวนเต็ม
strncmp(const var_na *p, const var_na *q, uint n)
{
  ขณะ (n > 0 && *p && *p == *q)
    n--, p++, q++;
  ถ้า (n == 0)
    คืน 0;
  คืน (uchar)*p - (uchar)*q;
}

var_na *
strncpy(var_na *s, const var_na *t, จำนวนเต็ม n)
{
  var_na *os;

  os = s;
  ขณะ (n-- > 0 && (*s++ = *t++) != 0)
    ;
  ขณะ (n-- > 0)
    *s++ = 0;
  คืน os;
}

// Like strncpy but guaranteed to NUL-terminate.
var_na *
safestrcpy(var_na *s, const var_na *t, จำนวนเต็ม n)
{
  var_na *os;

  os = s;
  ถ้า (n <= 0)
    คืน os;
  ขณะ (--n > 0 && (*s++ = *t++) != 0)
    ;
  *s = 0;
  คืน os;
}

จำนวนเต็ม
strlen(const var_na *s)
{
  จำนวนเต็ม n;

  สำหรับ (n = 0; s[n]; n++)
    ;
  คืน n;
}
