#অন্তর্ভাৱ "types.h"

শূন্য *
memset(শূন্য *dst, পূর্ণাঙ্ক c, uint n)
{
  ৱর্ণ *cdst = (ৱর্ণ *)dst;
  পূর্ণাঙ্ক i;
  বাবে (i = 0; i < n; i++) {
    cdst[i] = c;
  }
  ঘূৰাই dst;
}

পূর্ণাঙ্ক
memcmp(ধ্রুৱ শূন্য *v1, ধ্রুৱ শূন্য *v2, uint n)
{
  ধ্রুৱ uchar *s1, *s2;

  s1 = v1;
  s2 = v2;
  যেতিয়ালৈ (n-- > 0) {
    যদি (*s1 != *s2)
      ঘূৰাই *s1 - *s2;
    s1++, s2++;
  }

  ঘূৰাই 0;
}

শূন্য *
memmove(শূন্য *dst, ধ্রুৱ শূন্য *src, uint n)
{
  ধ্রুৱ ৱর্ণ *s;
  ৱর্ণ *d;

  যদি (n == 0)
    ঘূৰাই dst;

  s = src;
  d = dst;
  যদি (s < d && s + n > d) {
    s += n;
    d += n;
    যেতিয়ালৈ (n-- > 0)
      *--d = *--s;
  } অন্যথা
    যেতিয়ালৈ (n-- > 0)
      *d++ = *s++;

  ঘূৰাই dst;
}

// memcpy exists to placate GCC.  Use memmove.
শূন্য *
memcpy(শূন্য *dst, ধ্রুৱ শূন্য *src, uint n)
{
  ঘূৰাই memmove(dst, src, n);
}

পূর্ণাঙ্ক
strncmp(ধ্রুৱ ৱর্ণ *p, ধ্রুৱ ৱর্ণ *q, uint n)
{
  যেতিয়ালৈ (n > 0 && *p && *p == *q)
    n--, p++, q++;
  যদি (n == 0)
    ঘূৰাই 0;
  ঘূৰাই (uchar)*p - (uchar)*q;
}

ৱর্ণ *
strncpy(ৱর্ণ *s, ধ্রুৱ ৱর্ণ *t, পূর্ণাঙ্ক n)
{
  ৱর্ণ *os;

  os = s;
  যেতিয়ালৈ (n-- > 0 && (*s++ = *t++) != 0)
    ;
  যেতিয়ালৈ (n-- > 0)
    *s++ = 0;
  ঘূৰাই os;
}

// Like strncpy but guaranteed to NUL-terminate.
ৱর্ণ *
safestrcpy(ৱর্ণ *s, ধ্রুৱ ৱর্ণ *t, পূর্ণাঙ্ক n)
{
  ৱর্ণ *os;

  os = s;
  যদি (n <= 0)
    ঘূৰাই os;
  যেতিয়ালৈ (--n > 0 && (*s++ = *t++) != 0)
    ;
  *s = 0;
  ঘূৰাই os;
}

পূর্ণাঙ্ক
strlen(ধ্রুৱ ৱর্ণ *s)
{
  পূর্ণাঙ্ক n;

  বাবে (n = 0; s[n]; n++)
    ;
  ঘূৰাই n;
}
