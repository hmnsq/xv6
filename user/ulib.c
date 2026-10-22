#अन्तर्भाव "kernel/types.h"
#अन्तर्भाव "kernel/stat.h"
#अन्तर्भाव "kernel/fcntl.h"
#अन्तर्भाव "kernel/riscv.h"
#अन्तर्भाव "kernel/vm.h"
#अन्तर्भाव "user/user.h"

//
// wrapper so that it's OK if main() does not call exit().
//
शून्य
start(पूर्णाङ्क argc, वर्ण **argv)
{
  पूर्णाङ्क r;
  बाह्य पूर्णाङ्क main(पूर्णाङ्क argc, वर्ण **argv);
  r = main(argc, argv);
  exit(r);
}

वर्ण *
strcpy(वर्ण *s, ध्रुव वर्ण *t)
{
  वर्ण *os;

  os = s;
  याव ((*s++ = *t++) != 0)
    ;
  पटिनिवत्त os;
}

पूर्णाङ्क
strcmp(ध्रुव वर्ण *p, ध्रुव वर्ण *q)
{
  याव (*p && *p == *q)
    p++, q++;
  पटिनिवत्त (uchar)*p - (uchar)*q;
}

uint
strlen(ध्रुव वर्ण *s)
{
  पूर्णाङ्क n;

  अत्थाय (n = 0; s[n]; n++)
    ;
  पटिनिवत्त n;
}

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

वर्ण *
strchr(ध्रुव वर्ण *s, वर्ण c)
{
  अत्थाय (; *s; s++)
    सचे (*s == c)
      पटिनिवत्त (वर्ण *)s;
  पटिनिवत्त 0;
}

वर्ण *
gets(वर्ण *buf, पूर्णाङ्क max)
{
  पूर्णाङ्क i, cc;
  वर्ण c;

  अत्थाय (i = 0; i + 1 < max;) {
    cc = पठ(0, &c, 1);
    सचे (cc < 1)
      भङ्ग;
    buf[i++] = c;
    सचे (c == '\n' || c == '\r')
      भङ्ग;
  }
  buf[i] = '\0';
  पटिनिवत्त buf;
}

पूर्णाङ्क
stat(ध्रुव वर्ण *n, संरचना stat *st)
{
  पूर्णाङ्क fd;
  पूर्णाङ्क r;

  fd = open(n, O_RDONLY);
  सचे (fd < 0)
    पटिनिवत्त -1;
  r = fstat(fd, st);
  close(fd);
  पटिनिवत्त r;
}

पूर्णाङ्क
atoi(ध्रुव वर्ण *s)
{
  पूर्णाङ्क n;

  n = 0;
  याव ('0' <= *s && *s <= '9')
    n = n * 10 + *s++ - '0';
  पटिनिवत्त n;
}

शून्य *
memmove(शून्य *vdst, ध्रुव शून्य *vsrc, पूर्णाङ्क n)
{
  वर्ण *dst;
  ध्रुव वर्ण *src;

  dst = vdst;
  src = vsrc;
  सचे (src > dst) {
    याव (n-- > 0)
      *dst++ = *src++;
  } अञ्ञथा {
    dst += n;
    src += n;
    याव (n-- > 0)
      *--dst = *--src;
  }
  पटिनिवत्त vdst;
}

पूर्णाङ्क
memcmp(ध्रुव शून्य *s1, ध्रुव शून्य *s2, uint n)
{
  ध्रुव वर्ण *p1 = s1, *p2 = s2;
  याव (n-- > 0) {
    सचे (*p1 != *p2) {
      पटिनिवत्त *p1 - *p2;
    }
    p1++;
    p2++;
  }
  पटिनिवत्त 0;
}

शून्य *
memcpy(शून्य *dst, ध्रुव शून्य *src, uint n)
{
  पटिनिवत्त memmove(dst, src, n);
}

वर्ण *
sbrk(पूर्णाङ्क n)
{
  पटिनिवत्त sys_sbrk(n, SBRK_EAGER);
}

वर्ण *
sbrklazy(पूर्णाङ्क n)
{
  पटिनिवत्त sys_sbrk(n, SBRK_LAZY);
}
