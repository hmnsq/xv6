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
  जाव ((*s++ = *t++) != 0)
    ;
  प्रत्यागम os;
}

पूर्णाङ्क
strcmp(ध्रुव वर्ण *p, ध्रुव वर्ण *q)
{
  जाव (*p && *p == *q)
    p++, q++;
  प्रत्यागम (uchar)*p - (uchar)*q;
}

uint
strlen(ध्रुव वर्ण *s)
{
  पूर्णाङ्क n;

  प्रति (n = 0; s[n]; n++)
    ;
  प्रत्यागम n;
}

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

वर्ण *
strchr(ध्रुव वर्ण *s, वर्ण c)
{
  प्रति (; *s; s++)
    जइ (*s == c)
      प्रत्यागम (वर्ण *)s;
  प्रत्यागम 0;
}

वर्ण *
gets(वर्ण *buf, पूर्णाङ्क max)
{
  पूर्णाङ्क i, cc;
  वर्ण c;

  प्रति (i = 0; i + 1 < max;) {
    cc = पठ(0, &c, 1);
    जइ (cc < 1)
      भङ्ग;
    buf[i++] = c;
    जइ (c == '\n' || c == '\r')
      भङ्ग;
  }
  buf[i] = '\0';
  प्रत्यागम buf;
}

पूर्णाङ्क
stat(ध्रुव वर्ण *n, संरचना stat *st)
{
  पूर्णाङ्क fd;
  पूर्णाङ्क r;

  fd = open(n, O_RDONLY);
  जइ (fd < 0)
    प्रत्यागम -1;
  r = fstat(fd, st);
  close(fd);
  प्रत्यागम r;
}

पूर्णाङ्क
atoi(ध्रुव वर्ण *s)
{
  पूर्णाङ्क n;

  n = 0;
  जाव ('0' <= *s && *s <= '9')
    n = n * 10 + *s++ - '0';
  प्रत्यागम n;
}

शून्य *
memmove(शून्य *vdst, ध्रुव शून्य *vsrc, पूर्णाङ्क n)
{
  वर्ण *dst;
  ध्रुव वर्ण *src;

  dst = vdst;
  src = vsrc;
  जइ (src > dst) {
    जाव (n-- > 0)
      *dst++ = *src++;
  } अण्णहा {
    dst += n;
    src += n;
    जाव (n-- > 0)
      *--dst = *--src;
  }
  प्रत्यागम vdst;
}

पूर्णाङ्क
memcmp(ध्रुव शून्य *s1, ध्रुव शून्य *s2, uint n)
{
  ध्रुव वर्ण *p1 = s1, *p2 = s2;
  जाव (n-- > 0) {
    जइ (*p1 != *p2) {
      प्रत्यागम *p1 - *p2;
    }
    p1++;
    p2++;
  }
  प्रत्यागम 0;
}

शून्य *
memcpy(शून्य *dst, ध्रुव शून्य *src, uint n)
{
  प्रत्यागम memmove(dst, src, n);
}

वर्ण *
sbrk(पूर्णाङ्क n)
{
  प्रत्यागम sys_sbrk(n, SBRK_EAGER);
}

वर्ण *
sbrklazy(पूर्णाङ्क n)
{
  प्रत्यागम sys_sbrk(n, SBRK_LAZY);
}
