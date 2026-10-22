#include "types.h"

void *
memset(void *dst, целое c, uint n)
{
  char *cdst = (char *)dst;
  целое i;
  для (i = 0; i < n; i++) {
    cdst[i] = c;
  }
  вернуть dst;
}

целое
memcmp(const void *v1, const void *v2, uint n)
{
  const uchar *s1, *s2;

  s1 = v1;
  s2 = v2;
  пока (n-- > 0) {
    если (*s1 != *s2)
      вернуть *s1 - *s2;
    s1++, s2++;
  }

  вернуть 0;
}

void *
memmove(void *dst, const void *src, uint n)
{
  const char *s;
  char *d;

  если (n == 0)
    вернуть dst;

  s = src;
  d = dst;
  если (s < d && s + n > d) {
    s += n;
    d += n;
    пока (n-- > 0)
      *--d = *--s;
  } иначе
    пока (n-- > 0)
      *d++ = *s++;

  вернуть dst;
}

// memcpy exists to placate GCC.  Use memmove.
void *
memcpy(void *dst, const void *src, uint n)
{
  вернуть memmove(dst, src, n);
}

целое
strncmp(const char *p, const char *q, uint n)
{
  пока (n > 0 && *p && *p == *q)
    n--, p++, q++;
  если (n == 0)
    вернуть 0;
  вернуть (uchar)*p - (uchar)*q;
}

char *
strncpy(char *s, const char *t, целое n)
{
  char *os;

  os = s;
  пока (n-- > 0 && (*s++ = *t++) != 0)
    ;
  пока (n-- > 0)
    *s++ = 0;
  вернуть os;
}

// Like strncpy but guaranteed to NUL-terminate.
char *
safestrcpy(char *s, const char *t, целое n)
{
  char *os;

  os = s;
  если (n <= 0)
    вернуть os;
  пока (--n > 0 && (*s++ = *t++) != 0)
    ;
  *s = 0;
  вернуть os;
}

целое
strlen(const char *s)
{
  целое n;

  для (n = 0; s[n]; n++)
    ;
  вернуть n;
}
