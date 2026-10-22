#include "types.h"

void *
memset(void *dst, цяло c, uint n)
{
  char *cdst = (char *)dst;
  цяло i;
  за (i = 0; i < n; i++) {
    cdst[i] = c;
  }
  върни dst;
}

цяло
memcmp(const void *v1, const void *v2, uint n)
{
  const uchar *s1, *s2;

  s1 = v1;
  s2 = v2;
  докато (n-- > 0) {
    ако (*s1 != *s2)
      върни *s1 - *s2;
    s1++, s2++;
  }

  върни 0;
}

void *
memmove(void *dst, const void *src, uint n)
{
  const char *s;
  char *d;

  ако (n == 0)
    върни dst;

  s = src;
  d = dst;
  ако (s < d && s + n > d) {
    s += n;
    d += n;
    докато (n-- > 0)
      *--d = *--s;
  } иначе
    докато (n-- > 0)
      *d++ = *s++;

  върни dst;
}

// memcpy exists to placate GCC.  Use memmove.
void *
memcpy(void *dst, const void *src, uint n)
{
  върни memmove(dst, src, n);
}

цяло
strncmp(const char *p, const char *q, uint n)
{
  докато (n > 0 && *p && *p == *q)
    n--, p++, q++;
  ако (n == 0)
    върни 0;
  върни (uchar)*p - (uchar)*q;
}

char *
strncpy(char *s, const char *t, цяло n)
{
  char *os;

  os = s;
  докато (n-- > 0 && (*s++ = *t++) != 0)
    ;
  докато (n-- > 0)
    *s++ = 0;
  върни os;
}

// Like strncpy but guaranteed to NUL-terminate.
char *
safestrcpy(char *s, const char *t, цяло n)
{
  char *os;

  os = s;
  ако (n <= 0)
    върни os;
  докато (--n > 0 && (*s++ = *t++) != 0)
    ;
  *s = 0;
  върни os;
}

цяло
strlen(const char *s)
{
  цяло n;

  за (n = 0; s[n]; n++)
    ;
  върни n;
}
