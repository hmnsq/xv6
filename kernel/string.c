#include "types.h"

void *
memset(void *dst, số_nguyên c, uint n)
{
  char *cdst = (char *)dst;
  số_nguyên i;
  cho (i = 0; i < n; i++) {
    cdst[i] = c;
  }
  trả_về dst;
}

số_nguyên
memcmp(const void *v1, const void *v2, uint n)
{
  const uchar *s1, *s2;

  s1 = v1;
  s2 = v2;
  trong_khi (n-- > 0) {
    nếu (*s1 != *s2)
      trả_về *s1 - *s2;
    s1++, s2++;
  }

  trả_về 0;
}

void *
memmove(void *dst, const void *src, uint n)
{
  const char *s;
  char *d;

  nếu (n == 0)
    trả_về dst;

  s = src;
  d = dst;
  nếu (s < d && s + n > d) {
    s += n;
    d += n;
    trong_khi (n-- > 0)
      *--d = *--s;
  } không_thì
    trong_khi (n-- > 0)
      *d++ = *s++;

  trả_về dst;
}

// memcpy exists to placate GCC.  Use memmove.
void *
memcpy(void *dst, const void *src, uint n)
{
  trả_về memmove(dst, src, n);
}

số_nguyên
strncmp(const char *p, const char *q, uint n)
{
  trong_khi (n > 0 && *p && *p == *q)
    n--, p++, q++;
  nếu (n == 0)
    trả_về 0;
  trả_về (uchar)*p - (uchar)*q;
}

char *
strncpy(char *s, const char *t, số_nguyên n)
{
  char *os;

  os = s;
  trong_khi (n-- > 0 && (*s++ = *t++) != 0)
    ;
  trong_khi (n-- > 0)
    *s++ = 0;
  trả_về os;
}

// Like strncpy but guaranteed to NUL-terminate.
char *
safestrcpy(char *s, const char *t, số_nguyên n)
{
  char *os;

  os = s;
  nếu (n <= 0)
    trả_về os;
  trong_khi (--n > 0 && (*s++ = *t++) != 0)
    ;
  *s = 0;
  trả_về os;
}

số_nguyên
strlen(const char *s)
{
  số_nguyên n;

  cho (n = 0; s[n]; n++)
    ;
  trả_về n;
}
