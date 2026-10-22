#ضمن "kernel/types.h"
#ضمن "kernel/stat.h"
#ضمن "kernel/fcntl.h"
#ضمن "kernel/riscv.h"
#ضمن "kernel/vm.h"
#ضمن "user/user.h"

//
// wrapper so that it's OK if main() does not call exit().
//
فراغ
شروعات(صحيح argc, حرف **argv)
{
  صحيح r;
  خارجي صحيح مکيه(صحيح argc, حرف **argv);
  r = مکيه(argc, argv);
  ٻاهر_نڪرو(r);
}

حرف *
strcpy(حرف *s, ثابت حرف *t)
{
  حرف *os;

  os = s;
  طالما ((*s++ = *t++) != 0)
    ;
  أرجع os;
}

صحيح
strcmp(ثابت حرف *p, ثابت حرف *q)
{
  طالما (*p && *p == *q)
    p++, q++;
  أرجع (uchar)*p - (uchar)*q;
}

uint
لڙي_ڊيگهه(ثابت حرف *s)
{
  صحيح n;

  لكل (n = 0; s[n]; n++)
    ;
  أرجع n;
}

فراغ *
memset(فراغ *dst, صحيح c, uint n)
{
  حرف *cdst = (حرف *)dst;
  صحيح i;
  لكل (i = 0; i < n; i++) {
    cdst[i] = c;
  }
  أرجع dst;
}

حرف *
strchr(ثابت حرف *s, حرف c)
{
  لكل (; *s; s++)
    إذا (*s == c)
      أرجع (حرف *)s;
  أرجع 0;
}

حرف *
gets(حرف *buf, صحيح max)
{
  صحيح i, cc;
  حرف c;

  لكل (i = 0; i + 1 < max;) {
    cc = اقرأ(0, &c, 1);
    إذا (cc < 1)
      اكسر;
    buf[i++] = c;
    إذا (c == '\n' || c == '\r')
      اكسر;
  }
  buf[i] = '\0';
  أرجع buf;
}

صحيح
stat(ثابت حرف *n, بنية stat *st)
{
  صحيح fd;
  صحيح r;

  fd = open(n, O_RDONLY);
  إذا (fd < 0)
    أرجع -1;
  r = fstat(fd, st);
  close(fd);
  أرجع r;
}

صحيح
atoi(ثابت حرف *s)
{
  صحيح n;

  n = 0;
  طالما ('0' <= *s && *s <= '9')
    n = n * 10 + *s++ - '0';
  أرجع n;
}

فراغ *
memmove(فراغ *vdst, ثابت فراغ *vsrc, صحيح n)
{
  حرف *dst;
  ثابت حرف *src;

  dst = vdst;
  src = vsrc;
  إذا (src > dst) {
    طالما (n-- > 0)
      *dst++ = *src++;
  } وإلا {
    dst += n;
    src += n;
    طالما (n-- > 0)
      *--dst = *--src;
  }
  أرجع vdst;
}

صحيح
memcmp(ثابت فراغ *s1, ثابت فراغ *s2, uint n)
{
  ثابت حرف *p1 = s1, *p2 = s2;
  طالما (n-- > 0) {
    إذا (*p1 != *p2) {
      أرجع *p1 - *p2;
    }
    p1++;
    p2++;
  }
  أرجع 0;
}

فراغ *
يادگيري_نقل(فراغ *dst, ثابت فراغ *src, uint n)
{
  أرجع memmove(dst, src, n);
}

حرف *
sbrk(صحيح n)
{
  أرجع sys_sbrk(n, SBRK_EAGER);
}

حرف *
sbrklazy(صحيح n)
{
  أرجع sys_sbrk(n, SBRK_LAZY);
}
