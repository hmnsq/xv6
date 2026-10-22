#شامل "types.h"

خالی *
memset(خالی *dst, صحیح c, uint n)
{
  حرف *cdst = (حرف *)dst;
  صحیح i;
  برائے (i = 0; i < n; i++) {
    cdst[i] = c;
  }
  واپس dst;
}

صحیح
memcmp(ثابت خالی *v1, ثابت خالی *v2, uint n)
{
  ثابت uchar *s1, *s2;

  s1 = v1;
  s2 = v2;
  جب (n-- > 0) {
    اگر (*s1 != *s2)
      واپس *s1 - *s2;
    s1++, s2++;
  }

  واپس 0;
}

خالی *
memmove(خالی *dst, ثابت خالی *src, uint n)
{
  ثابت حرف *s;
  حرف *d;

  اگر (n == 0)
    واپس dst;

  s = src;
  d = dst;
  اگر (s < d && s + n > d) {
    s += n;
    d += n;
    جب (n-- > 0)
      *--d = *--s;
  } ورنہ
    جب (n-- > 0)
      *d++ = *s++;

  واپس dst;
}

// memcpy exists to placate GCC.  Use memmove.
خالی *
حافظہ_نقل(خالی *dst, ثابت خالی *src, uint n)
{
  واپس memmove(dst, src, n);
}

صحیح
لڑی_موازنہ(ثابت حرف *p, ثابت حرف *q, uint n)
{
  جب (n > 0 && *p && *p == *q)
    n--, p++, q++;
  اگر (n == 0)
    واپس 0;
  واپس (uchar)*p - (uchar)*q;
}

حرف *
strncpy(حرف *s, ثابت حرف *t, صحیح n)
{
  حرف *os;

  os = s;
  جب (n-- > 0 && (*s++ = *t++) != 0)
    ;
  جب (n-- > 0)
    *s++ = 0;
  واپس os;
}

// Like strncpy but guaranteed to NUL-terminate.
حرف *
safestrcpy(حرف *s, ثابت حرف *t, صحیح n)
{
  حرف *os;

  os = s;
  اگر (n <= 0)
    واپس os;
  جب (--n > 0 && (*s++ = *t++) != 0)
    ;
  *s = 0;
  واپس os;
}

صحیح
لڑی_لمبائی(ثابت حرف *s)
{
  صحیح n;

  برائے (n = 0; s[n]; n++)
    ;
  واپس n;
}
