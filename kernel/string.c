#شامل "types.h"

تهی *
memset(تهی *dst, صحیح c, uint n)
{
  نویسه *cdst = (نویسه *)dst;
  صحیح i;
  برای (i = 0; i < n; i++) {
    cdst[i] = c;
  }
  بازگشت dst;
}

صحیح
memcmp(ثابت تهی *v1, ثابت تهی *v2, uint n)
{
  ثابت uchar *s1, *s2;

  s1 = v1;
  s2 = v2;
  وقتی (n-- > 0) {
    اگر (*s1 != *s2)
      بازگشت *s1 - *s2;
    s1++, s2++;
  }

  بازگشت 0;
}

تهی *
memmove(تهی *dst, ثابت تهی *src, uint n)
{
  ثابت نویسه *s;
  نویسه *d;

  اگر (n == 0)
    بازگشت dst;

  s = src;
  d = dst;
  اگر (s < d && s + n > d) {
    s += n;
    d += n;
    وقتی (n-- > 0)
      *--d = *--s;
  } وگرنه
    وقتی (n-- > 0)
      *d++ = *s++;

  بازگشت dst;
}

// memcpy exists to placate GCC.  Use memmove.
تهی *
memcpy(تهی *dst, ثابت تهی *src, uint n)
{
  بازگشت memmove(dst, src, n);
}

صحیح
strncmp(ثابت نویسه *p, ثابت نویسه *q, uint n)
{
  وقتی (n > 0 && *p && *p == *q)
    n--, p++, q++;
  اگر (n == 0)
    بازگشت 0;
  بازگشت (uchar)*p - (uchar)*q;
}

نویسه *
strncpy(نویسه *s, ثابت نویسه *t, صحیح n)
{
  نویسه *os;

  os = s;
  وقتی (n-- > 0 && (*s++ = *t++) != 0)
    ;
  وقتی (n-- > 0)
    *s++ = 0;
  بازگشت os;
}

// Like strncpy but guaranteed to NUL-terminate.
نویسه *
safestrcpy(نویسه *s, ثابت نویسه *t, صحیح n)
{
  نویسه *os;

  os = s;
  اگر (n <= 0)
    بازگشت os;
  وقتی (--n > 0 && (*s++ = *t++) != 0)
    ;
  *s = 0;
  بازگشت os;
}

صحیح
strlen(ثابت نویسه *s)
{
  صحیح n;

  برای (n = 0; s[n]; n++)
    ;
  بازگشت n;
}
