#include "types.h"

void *
memset(void *dst, ціле c, uint n)
{
  char *cdst = (char *)dst;
  ціле i;
  для (i = 0; i < n; i++) {
    cdst[i] = c;
  }
  повернути dst;
}

ціле
memcmp(const void *v1, const void *v2, uint n)
{
  const uchar *s1, *s2;

  s1 = v1;
  s2 = v2;
  поки (n-- > 0) {
    якщо (*s1 != *s2)
      повернути *s1 - *s2;
    s1++, s2++;
  }

  повернути 0;
}

void *
memmove(void *dst, const void *src, uint n)
{
  const char *s;
  char *d;

  якщо (n == 0)
    повернути dst;

  s = src;
  d = dst;
  якщо (s < d && s + n > d) {
    s += n;
    d += n;
    поки (n-- > 0)
      *--d = *--s;
  } інакше
    поки (n-- > 0)
      *d++ = *s++;

  повернути dst;
}

// memcpy exists to placate GCC.  Use memmove.
void *
memcpy(void *dst, const void *src, uint n)
{
  повернути memmove(dst, src, n);
}

ціле
strncmp(const char *p, const char *q, uint n)
{
  поки (n > 0 && *p && *p == *q)
    n--, p++, q++;
  якщо (n == 0)
    повернути 0;
  повернути (uchar)*p - (uchar)*q;
}

char *
strncpy(char *s, const char *t, ціле n)
{
  char *os;

  os = s;
  поки (n-- > 0 && (*s++ = *t++) != 0)
    ;
  поки (n-- > 0)
    *s++ = 0;
  повернути os;
}

// Like strncpy but guaranteed to NUL-terminate.
char *
safestrcpy(char *s, const char *t, ціле n)
{
  char *os;

  os = s;
  якщо (n <= 0)
    повернути os;
  поки (--n > 0 && (*s++ = *t++) != 0)
    ;
  *s = 0;
  повернути os;
}

ціле
strlen(const char *s)
{
  ціле n;

  для (n = 0; s[n]; n++)
    ;
  повернути n;
}
