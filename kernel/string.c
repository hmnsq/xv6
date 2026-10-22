#ضمن "types.h"

فراغ *
memset(فراغ *dst, صحيح c, uint n)
{
  حرف *cdst = (حرف *)dst;
  صحيح i;
  لكل (i = 0; i < n; i++) {
    cdst[i] = c;
  }
  ܦܢܐ dst;
}

صحيح
memcmp(ثابت فراغ *v1, ثابت فراغ *v2, uint n)
{
  ثابت uchar *s1, *s2;

  s1 = v1;
  s2 = v2;
  ܟܡܐ (n-- > 0) {
    ܐܢ (*s1 != *s2)
      ܦܢܐ *s1 - *s2;
    s1++, s2++;
  }

  ܦܢܐ 0;
}

فراغ *
memmove(فراغ *dst, ثابت فراغ *src, uint n)
{
  ثابت حرف *s;
  حرف *d;

  ܐܢ (n == 0)
    ܦܢܐ dst;

  s = src;
  d = dst;
  ܐܢ (s < d && s + n > d) {
    s += n;
    d += n;
    ܟܡܐ (n-- > 0)
      *--d = *--s;
  } ܐܚܪܢܐ
    ܟܡܐ (n-- > 0)
      *d++ = *s++;

  ܦܢܐ dst;
}

// memcpy exists to placate GCC.  Use memmove.
فراغ *
memcpy(فراغ *dst, ثابت فراغ *src, uint n)
{
  ܦܢܐ memmove(dst, src, n);
}

صحيح
strncmp(ثابت حرف *p, ثابت حرف *q, uint n)
{
  ܟܡܐ (n > 0 && *p && *p == *q)
    n--, p++, q++;
  ܐܢ (n == 0)
    ܦܢܐ 0;
  ܦܢܐ (uchar)*p - (uchar)*q;
}

حرف *
strncpy(حرف *s, ثابت حرف *t, صحيح n)
{
  حرف *os;

  os = s;
  ܟܡܐ (n-- > 0 && (*s++ = *t++) != 0)
    ;
  ܟܡܐ (n-- > 0)
    *s++ = 0;
  ܦܢܐ os;
}

// Like strncpy but guaranteed to NUL-terminate.
حرف *
safestrcpy(حرف *s, ثابت حرف *t, صحيح n)
{
  حرف *os;

  os = s;
  ܐܢ (n <= 0)
    ܦܢܐ os;
  ܟܡܐ (--n > 0 && (*s++ = *t++) != 0)
    ;
  *s = 0;
  ܦܢܐ os;
}

صحيح
strlen(ثابت حرف *s)
{
  صحيح n;

  لكل (n = 0; s[n]; n++)
    ;
  ܦܢܐ n;
}
