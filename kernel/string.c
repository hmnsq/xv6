#include "types.h"

void *
memset(void *dst, 整数 c, uint n)
{
  char *cdst = (char *)dst;
  整数 i;
  为 (i = 0; i < n; i++) {
    cdst[i] = c;
  }
  返回 dst;
}

整数
memcmp(const void *v1, const void *v2, uint n)
{
  const uchar *s1, *s2;

  s1 = v1;
  s2 = v2;
  当 (n-- > 0) {
    如果 (*s1 != *s2)
      返回 *s1 - *s2;
    s1++, s2++;
  }

  返回 0;
}

void *
memmove(void *dst, const void *src, uint n)
{
  const char *s;
  char *d;

  如果 (n == 0)
    返回 dst;

  s = src;
  d = dst;
  如果 (s < d && s + n > d) {
    s += n;
    d += n;
    当 (n-- > 0)
      *--d = *--s;
  } 否则
    当 (n-- > 0)
      *d++ = *s++;

  返回 dst;
}

// memcpy exists to placate GCC.  Use memmove.
void *
memcpy(void *dst, const void *src, uint n)
{
  返回 memmove(dst, src, n);
}

整数
strncmp(const char *p, const char *q, uint n)
{
  当 (n > 0 && *p && *p == *q)
    n--, p++, q++;
  如果 (n == 0)
    返回 0;
  返回 (uchar)*p - (uchar)*q;
}

char *
strncpy(char *s, const char *t, 整数 n)
{
  char *os;

  os = s;
  当 (n-- > 0 && (*s++ = *t++) != 0)
    ;
  当 (n-- > 0)
    *s++ = 0;
  返回 os;
}

// Like strncpy but guaranteed to NUL-terminate.
char *
safestrcpy(char *s, const char *t, 整数 n)
{
  char *os;

  os = s;
  如果 (n <= 0)
    返回 os;
  当 (--n > 0 && (*s++ = *t++) != 0)
    ;
  *s = 0;
  返回 os;
}

整数
strlen(const char *s)
{
  整数 n;

  为 (n = 0; s[n]; n++)
    ;
  返回 n;
}
