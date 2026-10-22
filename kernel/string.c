#include "types.h"

void *
memset(void *dst, 정수 c, uint n)
{
  char *cdst = (char *)dst;
  정수 i;
  위해 (i = 0; i < n; i++) {
    cdst[i] = c;
  }
  반환 dst;
}

정수
memcmp(const void *v1, const void *v2, uint n)
{
  const uchar *s1, *s2;

  s1 = v1;
  s2 = v2;
  동안 (n-- > 0) {
    만약 (*s1 != *s2)
      반환 *s1 - *s2;
    s1++, s2++;
  }

  반환 0;
}

void *
memmove(void *dst, const void *src, uint n)
{
  const char *s;
  char *d;

  만약 (n == 0)
    반환 dst;

  s = src;
  d = dst;
  만약 (s < d && s + n > d) {
    s += n;
    d += n;
    동안 (n-- > 0)
      *--d = *--s;
  } 아니면
    동안 (n-- > 0)
      *d++ = *s++;

  반환 dst;
}

// memcpy exists to placate GCC.  Use memmove.
void *
memcpy(void *dst, const void *src, uint n)
{
  반환 memmove(dst, src, n);
}

정수
strncmp(const char *p, const char *q, uint n)
{
  동안 (n > 0 && *p && *p == *q)
    n--, p++, q++;
  만약 (n == 0)
    반환 0;
  반환 (uchar)*p - (uchar)*q;
}

char *
strncpy(char *s, const char *t, 정수 n)
{
  char *os;

  os = s;
  동안 (n-- > 0 && (*s++ = *t++) != 0)
    ;
  동안 (n-- > 0)
    *s++ = 0;
  반환 os;
}

// Like strncpy but guaranteed to NUL-terminate.
char *
safestrcpy(char *s, const char *t, 정수 n)
{
  char *os;

  os = s;
  만약 (n <= 0)
    반환 os;
  동안 (--n > 0 && (*s++ = *t++) != 0)
    ;
  *s = 0;
  반환 os;
}

정수
strlen(const char *s)
{
  정수 n;

  위해 (n = 0; s[n]; n++)
    ;
  반환 n;
}
