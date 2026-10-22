#અન્તર્ભાવ "types.h"

શૂન્ય *
memset(શૂન્ય *dst, પૂર્ણાંક c, uint n)
{
  અક્ષર *cdst = (અક્ષર *)dst;
  પૂર્ણાંક i;
  માટે (i = 0; i < n; i++) {
    cdst[i] = c;
  }
  પાછા dst;
}

પૂર્ણાંક
memcmp(અચલ શૂન્ય *v1, અચલ શૂન્ય *v2, uint n)
{
  અચલ uchar *s1, *s2;

  s1 = v1;
  s2 = v2;
  જ્યારે (n-- > 0) {
    જો (*s1 != *s2)
      પાછા *s1 - *s2;
    s1++, s2++;
  }

  પાછા 0;
}

શૂન્ય *
memmove(શૂન્ય *dst, અચલ શૂન્ય *src, uint n)
{
  અચલ અક્ષર *s;
  અક્ષર *d;

  જો (n == 0)
    પાછા dst;

  s = src;
  d = dst;
  જો (s < d && s + n > d) {
    s += n;
    d += n;
    જ્યારે (n-- > 0)
      *--d = *--s;
  } નહીંતર
    જ્યારે (n-- > 0)
      *d++ = *s++;

  પાછા dst;
}

// memcpy exists to placate GCC.  Use memmove.
શૂન્ય *
memcpy(શૂન્ય *dst, અચલ શૂન્ય *src, uint n)
{
  પાછા memmove(dst, src, n);
}

પૂર્ણાંક
strncmp(અચલ અક્ષર *p, અચલ અક્ષર *q, uint n)
{
  જ્યારે (n > 0 && *p && *p == *q)
    n--, p++, q++;
  જો (n == 0)
    પાછા 0;
  પાછા (uchar)*p - (uchar)*q;
}

અક્ષર *
strncpy(અક્ષર *s, અચલ અક્ષર *t, પૂર્ણાંક n)
{
  અક્ષર *os;

  os = s;
  જ્યારે (n-- > 0 && (*s++ = *t++) != 0)
    ;
  જ્યારે (n-- > 0)
    *s++ = 0;
  પાછા os;
}

// Like strncpy but guaranteed to NUL-terminate.
અક્ષર *
safestrcpy(અક્ષર *s, અચલ અક્ષર *t, પૂર્ણાંક n)
{
  અક્ષર *os;

  os = s;
  જો (n <= 0)
    પાછા os;
  જ્યારે (--n > 0 && (*s++ = *t++) != 0)
    ;
  *s = 0;
  પાછા os;
}

પૂર્ણાંક
strlen(અચલ અક્ષર *s)
{
  પૂર્ણાંક n;

  માટે (n = 0; s[n]; n++)
    ;
  પાછા n;
}
