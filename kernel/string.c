#include "types.h"

void *
memset(void *dst, цео c, uint n)
{
  char *cdst = (char *)dst;
  цео i;
  за (i = 0; i < n; i++) {
    cdst[i] = c;
  }
  врати dst;
}

цео
memcmp(const void *v1, const void *v2, uint n)
{
  const uchar *s1, *s2;

  s1 = v1;
  s2 = v2;
  док (n-- > 0) {
    ако (*s1 != *s2)
      врати *s1 - *s2;
    s1++, s2++;
  }

  врати 0;
}

void *
memmove(void *dst, const void *src, uint n)
{
  const char *s;
  char *d;

  ако (n == 0)
    врати dst;

  s = src;
  d = dst;
  ако (s < d && s + n > d) {
    s += n;
    d += n;
    док (n-- > 0)
      *--d = *--s;
  } инако
    док (n-- > 0)
      *d++ = *s++;

  врати dst;
}

// memcpy exists to placate GCC.  Use memmove.
void *
memcpy(void *dst, const void *src, uint n)
{
  врати memmove(dst, src, n);
}

цео
strncmp(const char *p, const char *q, uint n)
{
  док (n > 0 && *p && *p == *q)
    n--, p++, q++;
  ако (n == 0)
    врати 0;
  врати (uchar)*p - (uchar)*q;
}

char *
strncpy(char *s, const char *t, цео n)
{
  char *os;

  os = s;
  док (n-- > 0 && (*s++ = *t++) != 0)
    ;
  док (n-- > 0)
    *s++ = 0;
  врати os;
}

// Like strncpy but guaranteed to NUL-terminate.
char *
safestrcpy(char *s, const char *t, цео n)
{
  char *os;

  os = s;
  ако (n <= 0)
    врати os;
  док (--n > 0 && (*s++ = *t++) != 0)
    ;
  *s = 0;
  врати os;
}

цео
strlen(const char *s)
{
  цео n;

  за (n = 0; s[n]; n++)
    ;
  врати n;
}
