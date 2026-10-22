#অন্তর্ভাব "types.h"

শূন্য *
memset(শূন্য *dst, পূর্ণসংখ্যা c, uint n)
{
  অক্ষর *cdst = (অক্ষর *)dst;
  পূর্ণসংখ্যা i;
  জন্য (i = 0; i < n; i++) {
    cdst[i] = c;
  }
  ফেরত dst;
}

পূর্ণসংখ্যা
memcmp(ধ্রুবক শূন্য *v1, ধ্রুবক শূন্য *v2, uint n)
{
  ধ্রুবক uchar *s1, *s2;

  s1 = v1;
  s2 = v2;
  যখন (n-- > 0) {
    যদি (*s1 != *s2)
      ফেরত *s1 - *s2;
    s1++, s2++;
  }

  ফেরত 0;
}

শূন্য *
memmove(শূন্য *dst, ধ্রুবক শূন্য *src, uint n)
{
  ধ্রুবক অক্ষর *s;
  অক্ষর *d;

  যদি (n == 0)
    ফেরত dst;

  s = src;
  d = dst;
  যদি (s < d && s + n > d) {
    s += n;
    d += n;
    যখন (n-- > 0)
      *--d = *--s;
  } অন্যথা
    যখন (n-- > 0)
      *d++ = *s++;

  ফেরত dst;
}

// memcpy exists to placate GCC.  Use memmove.
শূন্য *
memcpy(শূন্য *dst, ধ্রুবক শূন্য *src, uint n)
{
  ফেরত memmove(dst, src, n);
}

পূর্ণসংখ্যা
strncmp(ধ্রুবক অক্ষর *p, ধ্রুবক অক্ষর *q, uint n)
{
  যখন (n > 0 && *p && *p == *q)
    n--, p++, q++;
  যদি (n == 0)
    ফেরত 0;
  ফেরত (uchar)*p - (uchar)*q;
}

অক্ষর *
strncpy(অক্ষর *s, ধ্রুবক অক্ষর *t, পূর্ণসংখ্যা n)
{
  অক্ষর *os;

  os = s;
  যখন (n-- > 0 && (*s++ = *t++) != 0)
    ;
  যখন (n-- > 0)
    *s++ = 0;
  ফেরত os;
}

// Like strncpy but guaranteed to NUL-terminate.
অক্ষর *
safestrcpy(অক্ষর *s, ধ্রুবক অক্ষর *t, পূর্ণসংখ্যা n)
{
  অক্ষর *os;

  os = s;
  যদি (n <= 0)
    ফেরত os;
  যখন (--n > 0 && (*s++ = *t++) != 0)
    ;
  *s = 0;
  ফেরত os;
}

পূর্ণসংখ্যা
strlen(ধ্রুবক অক্ষর *s)
{
  পূর্ণসংখ্যা n;

  জন্য (n = 0; s[n]; n++)
    ;
  ফেরত n;
}
