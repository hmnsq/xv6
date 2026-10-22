#अन्तर्भाव "types.h"

शून्य *
memset(शून्य *dst, पूर्णाङ्क c, uint n)
{
  वर्ण *cdst = (वर्ण *)dst;
  पूर्णाङ्क i;
  अत्थाय (i = 0; i < n; i++) {
    cdst[i] = c;
  }
  पटिनिवत्त dst;
}

पूर्णाङ्क
memcmp(ध्रुव शून्य *v1, ध्रुव शून्य *v2, uint n)
{
  ध्रुव uchar *s1, *s2;

  s1 = v1;
  s2 = v2;
  याव (n-- > 0) {
    सचे (*s1 != *s2)
      पटिनिवत्त *s1 - *s2;
    s1++, s2++;
  }

  पटिनिवत्त 0;
}

शून्य *
memmove(शून्य *dst, ध्रुव शून्य *src, uint n)
{
  ध्रुव वर्ण *s;
  वर्ण *d;

  सचे (n == 0)
    पटिनिवत्त dst;

  s = src;
  d = dst;
  सचे (s < d && s + n > d) {
    s += n;
    d += n;
    याव (n-- > 0)
      *--d = *--s;
  } अञ्ञथा
    याव (n-- > 0)
      *d++ = *s++;

  पटिनिवत्त dst;
}

// memcpy exists to placate GCC.  Use memmove.
शून्य *
memcpy(शून्य *dst, ध्रुव शून्य *src, uint n)
{
  पटिनिवत्त memmove(dst, src, n);
}

पूर्णाङ्क
strncmp(ध्रुव वर्ण *p, ध्रुव वर्ण *q, uint n)
{
  याव (n > 0 && *p && *p == *q)
    n--, p++, q++;
  सचे (n == 0)
    पटिनिवत्त 0;
  पटिनिवत्त (uchar)*p - (uchar)*q;
}

वर्ण *
strncpy(वर्ण *s, ध्रुव वर्ण *t, पूर्णाङ्क n)
{
  वर्ण *os;

  os = s;
  याव (n-- > 0 && (*s++ = *t++) != 0)
    ;
  याव (n-- > 0)
    *s++ = 0;
  पटिनिवत्त os;
}

// Like strncpy but guaranteed to NUL-terminate.
वर्ण *
safestrcpy(वर्ण *s, ध्रुव वर्ण *t, पूर्णाङ्क n)
{
  वर्ण *os;

  os = s;
  सचे (n <= 0)
    पटिनिवत्त os;
  याव (--n > 0 && (*s++ = *t++) != 0)
    ;
  *s = 0;
  पटिनिवत्त os;
}

पूर्णाङ्क
strlen(ध्रुव वर्ण *s)
{
  पूर्णाङ्क n;

  अत्थाय (n = 0; s[n]; n++)
    ;
  पटिनिवत्त n;
}
