#ضمن "types.h"

فراغ *
memset(فراغ *dst, صحيح c, uint n)
{
  حرف *cdst = (حرف *)dst;
  صحيح i;
  لكل (i = 0; i < n; i++) {
    cdst[i] = c;
  }
  إرجاع dst;
}

صحيح
memcmp(ثابت فراغ *v1, ثابت فراغ *v2, uint n)
{
  ثابت uchar *s1, *s2;

  s1 = v1;
  s2 = v2;
  بينما (n-- > 0) {
    إذا (*s1 != *s2)
      إرجاع *s1 - *s2;
    s1++, s2++;
  }

  إرجاع 0;
}

فراغ *
memmove(فراغ *dst, ثابت فراغ *src, uint n)
{
  ثابت حرف *s;
  حرف *d;

  إذا (n == 0)
    إرجاع dst;

  s = src;
  d = dst;
  إذا (s < d && s + n > d) {
    s += n;
    d += n;
    بينما (n-- > 0)
      *--d = *--s;
  } وإلا
    بينما (n-- > 0)
      *d++ = *s++;

  إرجاع dst;
}

// memcpy exists to placate GCC.  Use memmove.
فراغ *
memcpy(فراغ *dst, ثابت فراغ *src, uint n)
{
  إرجاع memmove(dst, src, n);
}

صحيح
strncmp(ثابت حرف *p, ثابت حرف *q, uint n)
{
  بينما (n > 0 && *p && *p == *q)
    n--, p++, q++;
  إذا (n == 0)
    إرجاع 0;
  إرجاع (uchar)*p - (uchar)*q;
}

حرف *
strncpy(حرف *s, ثابت حرف *t, صحيح n)
{
  حرف *os;

  os = s;
  بينما (n-- > 0 && (*s++ = *t++) != 0)
    ;
  بينما (n-- > 0)
    *s++ = 0;
  إرجاع os;
}

// Like strncpy but guaranteed to NUL-terminate.
حرف *
safestrcpy(حرف *s, ثابت حرف *t, صحيح n)
{
  حرف *os;

  os = s;
  إذا (n <= 0)
    إرجاع os;
  بينما (--n > 0 && (*s++ = *t++) != 0)
    ;
  *s = 0;
  إرجاع os;
}

صحيح
strlen(ثابت حرف *s)
{
  صحيح n;

  لكل (n = 0; s[n]; n++)
    ;
  إرجاع n;
}
