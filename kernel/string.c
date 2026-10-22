#include "types.h"

void *
memset(void *dst, 整数 c, uint n)
{
  char *cdst = (char *)dst;
  整数 i;
  ために (i = 0; i < n; i++) {
    cdst[i] = c;
  }
  戻る dst;
}

整数
memcmp(const void *v1, const void *v2, uint n)
{
  const uchar *s1, *s2;

  s1 = v1;
  s2 = v2;
  ながら (n-- > 0) {
    もし (*s1 != *s2)
      戻る *s1 - *s2;
    s1++, s2++;
  }

  戻る 0;
}

void *
memmove(void *dst, const void *src, uint n)
{
  const char *s;
  char *d;

  もし (n == 0)
    戻る dst;

  s = src;
  d = dst;
  もし (s < d && s + n > d) {
    s += n;
    d += n;
    ながら (n-- > 0)
      *--d = *--s;
  } 否则
    ながら (n-- > 0)
      *d++ = *s++;

  戻る dst;
}

// memcpy exists to placate GCC.  Use memmove.
void *
memcpy(void *dst, const void *src, uint n)
{
  戻る memmove(dst, src, n);
}

整数
strncmp(const char *p, const char *q, uint n)
{
  ながら (n > 0 && *p && *p == *q)
    n--, p++, q++;
  もし (n == 0)
    戻る 0;
  戻る (uchar)*p - (uchar)*q;
}

char *
strncpy(char *s, const char *t, 整数 n)
{
  char *os;

  os = s;
  ながら (n-- > 0 && (*s++ = *t++) != 0)
    ;
  ながら (n-- > 0)
    *s++ = 0;
  戻る os;
}

// Like strncpy but guaranteed to NUL-terminate.
char *
safestrcpy(char *s, const char *t, 整数 n)
{
  char *os;

  os = s;
  もし (n <= 0)
    戻る os;
  ながら (--n > 0 && (*s++ = *t++) != 0)
    ;
  *s = 0;
  戻る os;
}

整数
strlen(const char *s)
{
  整数 n;

  ために (n = 0; s[n]; n++)
    ;
  戻る n;
}
