#ضمن "types.h"

فراغ *
memset(فراغ *dst, صحيح c, uint n)
{
  حرف *cdst = (حرف *)dst;
  صحيح i;
  لكل (i = 0; i < n; i++) {
    cdst[i] = c;
  }
  أرجع dst;
}

صحيح
memcmp(ثابت فراغ *v1, ثابت فراغ *v2, uint n)
{
  ثابت uchar *s1, *s2;

  s1 = v1;
  s2 = v2;
  طالما (n-- > 0) {
    إذا (*s1 != *s2)
      أرجع *s1 - *s2;
    s1++, s2++;
  }

  أرجع 0;
}

فراغ *
memmove(فراغ *dst, ثابت فراغ *src, uint n)
{
  ثابت حرف *s;
  حرف *d;

  إذا (n == 0)
    أرجع dst;

  s = src;
  d = dst;
  إذا (s < d && s + n > d) {
    s += n;
    d += n;
    طالما (n-- > 0)
      *--d = *--s;
  } وإلا
    طالما (n-- > 0)
      *d++ = *s++;

  أرجع dst;
}

// memcpy exists to placate GCC.  Use memmove.
فراغ *
کپی_حافظه(فراغ *dst, ثابت فراغ *src, uint n)
{
  أرجع memmove(dst, src, n);
}

صحيح
مقایسه_رشته(ثابت حرف *p, ثابت حرف *q, uint n)
{
  طالما (n > 0 && *p && *p == *q)
    n--, p++, q++;
  إذا (n == 0)
    أرجع 0;
  أرجع (uchar)*p - (uchar)*q;
}

حرف *
strncpy(حرف *s, ثابت حرف *t, صحيح n)
{
  حرف *os;

  os = s;
  طالما (n-- > 0 && (*s++ = *t++) != 0)
    ;
  طالما (n-- > 0)
    *s++ = 0;
  أرجع os;
}

// Like strncpy but guaranteed to NUL-terminate.
حرف *
safestrcpy(حرف *s, ثابت حرف *t, صحيح n)
{
  حرف *os;

  os = s;
  إذا (n <= 0)
    أرجع os;
  طالما (--n > 0 && (*s++ = *t++) != 0)
    ;
  *s = 0;
  أرجع os;
}

صحيح
طول_رشته(ثابت حرف *s)
{
  صحيح n;

  لكل (n = 0; s[n]; n++)
    ;
  أرجع n;
}
