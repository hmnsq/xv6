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
start(صحيح argc, حرف **argv)
{
  صحيح r;
  خارجي صحيح الرئيسية(صحيح argc, حرف **argv);
  r = الرئيسية(argc, argv);
  exit(r);
}

حرف *
strcpy(حرف *s, ثابت حرف *t)
{
  حرف *os;

  os = s;
  بينما ((*s++ = *t++) != 0)
    ;
  إرجاع os;
}

صحيح
strcmp(ثابت حرف *p, ثابت حرف *q)
{
  بينما (*p && *p == *q)
    p++, q++;
  إرجاع (uchar)*p - (uchar)*q;
}

uint
strlen(ثابت حرف *s)
{
  صحيح n;

  لكل (n = 0; s[n]; n++)
    ;
  إرجاع n;
}

فراغ *
memset(فراغ *dst, صحيح c, uint n)
{
  حرف *cdst = (حرف *)dst;
  صحيح i;
  لكل (i = 0; i < n; i++) {
    cdst[i] = c;
  }
  إرجاع dst;
}

حرف *
strchr(ثابت حرف *s, حرف c)
{
  لكل (; *s; s++)
    إذا (*s == c)
      إرجاع (حرف *)s;
  إرجاع 0;
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
  إرجاع buf;
}

صحيح
stat(ثابت حرف *n, بنية stat *st)
{
  صحيح fd;
  صحيح r;

  fd = open(n, O_RDONLY);
  إذا (fd < 0)
    إرجاع -1;
  r = fstat(fd, st);
  close(fd);
  إرجاع r;
}

صحيح
atoi(ثابت حرف *s)
{
  صحيح n;

  n = 0;
  بينما ('0' <= *s && *s <= '9')
    n = n * 10 + *s++ - '0';
  إرجاع n;
}

فراغ *
memmove(فراغ *vdst, ثابت فراغ *vsrc, صحيح n)
{
  حرف *dst;
  ثابت حرف *src;

  dst = vdst;
  src = vsrc;
  إذا (src > dst) {
    بينما (n-- > 0)
      *dst++ = *src++;
  } وإلا {
    dst += n;
    src += n;
    بينما (n-- > 0)
      *--dst = *--src;
  }
  إرجاع vdst;
}

صحيح
memcmp(ثابت فراغ *s1, ثابت فراغ *s2, uint n)
{
  ثابت حرف *p1 = s1, *p2 = s2;
  بينما (n-- > 0) {
    إذا (*p1 != *p2) {
      إرجاع *p1 - *p2;
    }
    p1++;
    p2++;
  }
  إرجاع 0;
}

فراغ *
memcpy(فراغ *dst, ثابت فراغ *src, uint n)
{
  إرجاع memmove(dst, src, n);
}

حرف *
sbrk(صحيح n)
{
  إرجاع sys_sbrk(n, SBRK_EAGER);
}

حرف *
sbrklazy(صحيح n)
{
  إرجاع sys_sbrk(n, SBRK_LAZY);
}
