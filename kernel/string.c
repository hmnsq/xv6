#ਅਨ੍ਤਰ੍ਭਾਵ "types.h"

ਖਾਲੀ *
memset(ਖਾਲੀ *dst, ਪੂਰਨ c, uint n)
{
  ਅੱਖਰ *cdst = (ਅੱਖਰ *)dst;
  ਪੂਰਨ i;
  ਲਈ (i = 0; i < n; i++) {
    cdst[i] = c;
  }
  ਵਾਪਸ dst;
}

ਪੂਰਨ
memcmp(ਅਚਲ ਖਾਲੀ *v1, ਅਚਲ ਖਾਲੀ *v2, uint n)
{
  ਅਚਲ uchar *s1, *s2;

  s1 = v1;
  s2 = v2;
  ਜਦੋਂ (n-- > 0) {
    ਜੇ (*s1 != *s2)
      ਵਾਪਸ *s1 - *s2;
    s1++, s2++;
  }

  ਵਾਪਸ 0;
}

ਖਾਲੀ *
memmove(ਖਾਲੀ *dst, ਅਚਲ ਖਾਲੀ *src, uint n)
{
  ਅਚਲ ਅੱਖਰ *s;
  ਅੱਖਰ *d;

  ਜੇ (n == 0)
    ਵਾਪਸ dst;

  s = src;
  d = dst;
  ਜੇ (s < d && s + n > d) {
    s += n;
    d += n;
    ਜਦੋਂ (n-- > 0)
      *--d = *--s;
  } ਨਹੀਂਤਾਂ
    ਜਦੋਂ (n-- > 0)
      *d++ = *s++;

  ਵਾਪਸ dst;
}

// memcpy exists to placate GCC.  Use memmove.
ਖਾਲੀ *
ਸਮ੍ਰਿਤੀ_ਨਕਲ(ਖਾਲੀ *dst, ਅਚਲ ਖਾਲੀ *src, uint n)
{
  ਵਾਪਸ memmove(dst, src, n);
}

ਪੂਰਨ
ਲੜੀ_ਤੁਲਨਾ(ਅਚਲ ਅੱਖਰ *p, ਅਚਲ ਅੱਖਰ *q, uint n)
{
  ਜਦੋਂ (n > 0 && *p && *p == *q)
    n--, p++, q++;
  ਜੇ (n == 0)
    ਵਾਪਸ 0;
  ਵਾਪਸ (uchar)*p - (uchar)*q;
}

ਅੱਖਰ *
strncpy(ਅੱਖਰ *s, ਅਚਲ ਅੱਖਰ *t, ਪੂਰਨ n)
{
  ਅੱਖਰ *os;

  os = s;
  ਜਦੋਂ (n-- > 0 && (*s++ = *t++) != 0)
    ;
  ਜਦੋਂ (n-- > 0)
    *s++ = 0;
  ਵਾਪਸ os;
}

// Like strncpy but guaranteed to NUL-terminate.
ਅੱਖਰ *
safestrcpy(ਅੱਖਰ *s, ਅਚਲ ਅੱਖਰ *t, ਪੂਰਨ n)
{
  ਅੱਖਰ *os;

  os = s;
  ਜੇ (n <= 0)
    ਵਾਪਸ os;
  ਜਦੋਂ (--n > 0 && (*s++ = *t++) != 0)
    ;
  *s = 0;
  ਵਾਪਸ os;
}

ਪੂਰਨ
ਲੜੀ_ਲੰਬਾਈ(ਅਚਲ ਅੱਖਰ *s)
{
  ਪੂਰਨ n;

  ਲਈ (n = 0; s[n]; n++)
    ;
  ਵਾਪਸ n;
}
