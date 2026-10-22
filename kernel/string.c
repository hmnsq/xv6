#अन्तर्भाव "types.h"

शून्य *
memset(शून्य *dst, पूर्णाङ्क c, uint n)
{
  वर्ण *cdst = (वर्ण *)dst;
  पूर्णाङ्क i;
  प्रति (i = 0; i < n; i++) {
    cdst[i] = c;
  }
  प्रत्यागम dst;
}

पूर्णाङ्क
memcmp(ध्रुव शून्य *v1, ध्रुव शून्य *v2, uint n)
{
  ध्रुव uchar *s1, *s2;

  s1 = v1;
  s2 = v2;
  जाव (n-- > 0) {
    जइ (*s1 != *s2)
      प्रत्यागम *s1 - *s2;
    s1++, s2++;
  }

  प्रत्यागम 0;
}

शून्य *
memmove(शून्य *dst, ध्रुव शून्य *src, uint n)
{
  ध्रुव वर्ण *s;
  वर्ण *d;

  जइ (n == 0)
    प्रत्यागम dst;

  s = src;
  d = dst;
  जइ (s < d && s + n > d) {
    s += n;
    d += n;
    जाव (n-- > 0)
      *--d = *--s;
  } अण्णहा
    जाव (n-- > 0)
      *d++ = *s++;

  प्रत्यागम dst;
}

// memcpy exists to placate GCC.  Use memmove.
शून्य *
memcpy(शून्य *dst, ध्रुव शून्य *src, uint n)
{
  प्रत्यागम memmove(dst, src, n);
}

पूर्णाङ्क
strncmp(ध्रुव वर्ण *p, ध्रुव वर्ण *q, uint n)
{
  जाव (n > 0 && *p && *p == *q)
    n--, p++, q++;
  जइ (n == 0)
    प्रत्यागम 0;
  प्रत्यागम (uchar)*p - (uchar)*q;
}

वर्ण *
strncpy(वर्ण *s, ध्रुव वर्ण *t, पूर्णाङ्क n)
{
  वर्ण *os;

  os = s;
  जाव (n-- > 0 && (*s++ = *t++) != 0)
    ;
  जाव (n-- > 0)
    *s++ = 0;
  प्रत्यागम os;
}

// Like strncpy but guaranteed to NUL-terminate.
वर्ण *
safestrcpy(वर्ण *s, ध्रुव वर्ण *t, पूर्णाङ्क n)
{
  वर्ण *os;

  os = s;
  जइ (n <= 0)
    प्रत्यागम os;
  जाव (--n > 0 && (*s++ = *t++) != 0)
    ;
  *s = 0;
  प्रत्यागम os;
}

पूर्णाङ्क
strlen(ध्रुव वर्ण *s)
{
  पूर्णाङ्क n;

  प्रति (n = 0; s[n]; n++)
    ;
  प्रत्यागम n;
}
