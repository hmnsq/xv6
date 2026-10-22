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
  خارجي صحيح main(صحيح argc, حرف **argv);
  r = main(argc, argv);
  exit(r);
}

حرف *
strcpy(حرف *s, ثابت حرف *t)
{
  حرف *os;

  os = s;
  ܟܡܐ ((*s++ = *t++) != 0)
    ;
  ܦܢܐ os;
}

صحيح
strcmp(ثابت حرف *p, ثابت حرف *q)
{
  ܟܡܐ (*p && *p == *q)
    p++, q++;
  ܦܢܐ (uchar)*p - (uchar)*q;
}

uint
strlen(ثابت حرف *s)
{
  صحيح n;

  لكل (n = 0; s[n]; n++)
    ;
  ܦܢܐ n;
}

فراغ *
memset(فراغ *dst, صحيح c, uint n)
{
  حرف *cdst = (حرف *)dst;
  صحيح i;
  لكل (i = 0; i < n; i++) {
    cdst[i] = c;
  }
  ܦܢܐ dst;
}

حرف *
strchr(ثابت حرف *s, حرف c)
{
  لكل (; *s; s++)
    ܐܢ (*s == c)
      ܦܢܐ (حرف *)s;
  ܦܢܐ 0;
}

حرف *
gets(حرف *buf, صحيح max)
{
  صحيح i, cc;
  حرف c;

  لكل (i = 0; i + 1 < max;) {
    cc = اقرأ(0, &c, 1);
    ܐܢ (cc < 1)
      اكسر;
    buf[i++] = c;
    ܐܢ (c == '\n' || c == '\r')
      اكسر;
  }
  buf[i] = '\0';
  ܦܢܐ buf;
}

صحيح
stat(ثابت حرف *n, بنية stat *st)
{
  صحيح fd;
  صحيح r;

  fd = open(n, O_RDONLY);
  ܐܢ (fd < 0)
    ܦܢܐ -1;
  r = fstat(fd, st);
  close(fd);
  ܦܢܐ r;
}

صحيح
atoi(ثابت حرف *s)
{
  صحيح n;

  n = 0;
  ܟܡܐ ('0' <= *s && *s <= '9')
    n = n * 10 + *s++ - '0';
  ܦܢܐ n;
}

فراغ *
memmove(فراغ *vdst, ثابت فراغ *vsrc, صحيح n)
{
  حرف *dst;
  ثابت حرف *src;

  dst = vdst;
  src = vsrc;
  ܐܢ (src > dst) {
    ܟܡܐ (n-- > 0)
      *dst++ = *src++;
  } ܐܚܪܢܐ {
    dst += n;
    src += n;
    ܟܡܐ (n-- > 0)
      *--dst = *--src;
  }
  ܦܢܐ vdst;
}

صحيح
memcmp(ثابت فراغ *s1, ثابت فراغ *s2, uint n)
{
  ثابت حرف *p1 = s1, *p2 = s2;
  ܟܡܐ (n-- > 0) {
    ܐܢ (*p1 != *p2) {
      ܦܢܐ *p1 - *p2;
    }
    p1++;
    p2++;
  }
  ܦܢܐ 0;
}

فراغ *
memcpy(فراغ *dst, ثابت فراغ *src, uint n)
{
  ܦܢܐ memmove(dst, src, n);
}

حرف *
sbrk(صحيح n)
{
  ܦܢܐ sys_sbrk(n, SBRK_EAGER);
}

حرف *
sbrklazy(صحيح n)
{
  ܦܢܐ sys_sbrk(n, SBRK_LAZY);
}
