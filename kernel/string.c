#ضمن "types.h"

فراغ *
memset(فراغ *dst, صحيح c, uint n)
{
  حرف *cdst = (حرف *)dst;
  صحيح i;
  עבור (i = 0; i < n; i++) {
    cdst[i] = c;
  }
  החזר dst;
}

صحيح
memcmp(ثابت فراغ *v1, ثابت فراغ *v2, uint n)
{
  ثابت uchar *s1, *s2;

  s1 = v1;
  s2 = v2;
  כלעוד (n-- > 0) {
    אם (*s1 != *s2)
      החזר *s1 - *s2;
    s1++, s2++;
  }

  החזר 0;
}

فراغ *
memmove(فراغ *dst, ثابت فراغ *src, uint n)
{
  ثابت حرف *s;
  حرف *d;

  אם (n == 0)
    החזר dst;

  s = src;
  d = dst;
  אם (s < d && s + n > d) {
    s += n;
    d += n;
    כלעוד (n-- > 0)
      *--d = *--s;
  } אחרת
    כלעוד (n-- > 0)
      *d++ = *s++;

  החזר dst;
}

// memcpy exists to placate GCC.  Use memmove.
فراغ *
memcpy(فراغ *dst, ثابت فراغ *src, uint n)
{
  החזר memmove(dst, src, n);
}

صحيح
strncmp(ثابت حرف *p, ثابت حرف *q, uint n)
{
  כלעוד (n > 0 && *p && *p == *q)
    n--, p++, q++;
  אם (n == 0)
    החזר 0;
  החזר (uchar)*p - (uchar)*q;
}

حرف *
strncpy(حرف *s, ثابت حرف *t, صحيح n)
{
  حرف *os;

  os = s;
  כלעוד (n-- > 0 && (*s++ = *t++) != 0)
    ;
  כלעוד (n-- > 0)
    *s++ = 0;
  החזר os;
}

// Like strncpy but guaranteed to NUL-terminate.
حرف *
safestrcpy(حرف *s, ثابت حرف *t, صحيح n)
{
  حرف *os;

  os = s;
  אם (n <= 0)
    החזר os;
  כלעוד (--n > 0 && (*s++ = *t++) != 0)
    ;
  *s = 0;
  החזר os;
}

صحيح
strlen(ثابت حرف *s)
{
  صحيح n;

  עבור (n = 0; s[n]; n++)
    ;
  החזר n;
}
