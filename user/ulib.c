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
  כלעוד ((*s++ = *t++) != 0)
    ;
  החזר os;
}

صحيح
strcmp(ثابت حرف *p, ثابت حرف *q)
{
  כלעוד (*p && *p == *q)
    p++, q++;
  החזר (uchar)*p - (uchar)*q;
}

uint
strlen(ثابت حرف *s)
{
  صحيح n;

  עבור (n = 0; s[n]; n++)
    ;
  החזר n;
}

فراغ *
memset(فراغ *dst, صحيح c, uint n)
{
  حرف *cdst = (حرف *)dst;
  صحيح i;
  עבור (i = 0; i < n; i++) {
    cdst[i] = c;
  }
  החזר dst;
}

حرف *
strchr(ثابت حرف *s, حرف c)
{
  עבור (; *s; s++)
    אם (*s == c)
      החזר (حرف *)s;
  החזר 0;
}

حرف *
gets(حرف *buf, صحيح max)
{
  صحيح i, cc;
  حرف c;

  עבור (i = 0; i + 1 < max;) {
    cc = اقرأ(0, &c, 1);
    אם (cc < 1)
      اكسر;
    buf[i++] = c;
    אם (c == '\n' || c == '\r')
      اكسر;
  }
  buf[i] = '\0';
  החזר buf;
}

صحيح
stat(ثابت حرف *n, بنية stat *st)
{
  صحيح fd;
  صحيح r;

  fd = open(n, O_RDONLY);
  אם (fd < 0)
    החזר -1;
  r = fstat(fd, st);
  close(fd);
  החזר r;
}

صحيح
atoi(ثابت حرف *s)
{
  صحيح n;

  n = 0;
  כלעוד ('0' <= *s && *s <= '9')
    n = n * 10 + *s++ - '0';
  החזר n;
}

فراغ *
memmove(فراغ *vdst, ثابت فراغ *vsrc, صحيح n)
{
  حرف *dst;
  ثابت حرف *src;

  dst = vdst;
  src = vsrc;
  אם (src > dst) {
    כלעוד (n-- > 0)
      *dst++ = *src++;
  } אחרת {
    dst += n;
    src += n;
    כלעוד (n-- > 0)
      *--dst = *--src;
  }
  החזר vdst;
}

صحيح
memcmp(ثابت فراغ *s1, ثابت فراغ *s2, uint n)
{
  ثابت حرف *p1 = s1, *p2 = s2;
  כלעוד (n-- > 0) {
    אם (*p1 != *p2) {
      החזר *p1 - *p2;
    }
    p1++;
    p2++;
  }
  החזר 0;
}

فراغ *
memcpy(فراغ *dst, ثابت فراغ *src, uint n)
{
  החזר memmove(dst, src, n);
}

حرف *
sbrk(صحيح n)
{
  החזר sys_sbrk(n, SBRK_EAGER);
}

حرف *
sbrklazy(صحيح n)
{
  החזר sys_sbrk(n, SBRK_LAZY);
}
