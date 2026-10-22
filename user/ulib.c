#شامل "kernel/types.h"
#شامل "kernel/stat.h"
#شامل "kernel/fcntl.h"
#شامل "kernel/riscv.h"
#شامل "kernel/vm.h"
#شامل "user/user.h"

//
// wrapper so that it's OK if main() does not call exit().
//
خالی
آغاز(صحیح argc, حرف **argv)
{
  صحیح r;
  خارجی صحیح مین(صحیح argc, حرف **argv);
  r = مین(argc, argv);
  باہر_نکلو(r);
}

حرف *
strcpy(حرف *s, ثابت حرف *t)
{
  حرف *os;

  os = s;
  جب ((*s++ = *t++) != 0)
    ;
  واپس os;
}

صحیح
strcmp(ثابت حرف *p, ثابت حرف *q)
{
  جب (*p && *p == *q)
    p++, q++;
  واپس (uchar)*p - (uchar)*q;
}

uint
لڑی_لمبائی(ثابت حرف *s)
{
  صحیح n;

  برائے (n = 0; s[n]; n++)
    ;
  واپس n;
}

خالی *
memset(خالی *dst, صحیح c, uint n)
{
  حرف *cdst = (حرف *)dst;
  صحیح i;
  برائے (i = 0; i < n; i++) {
    cdst[i] = c;
  }
  واپس dst;
}

حرف *
strchr(ثابت حرف *s, حرف c)
{
  برائے (; *s; s++)
    اگر (*s == c)
      واپس (حرف *)s;
  واپس 0;
}

حرف *
gets(حرف *buf, صحیح max)
{
  صحیح i, cc;
  حرف c;

  برائے (i = 0; i + 1 < max;) {
    cc = اقرأ(0, &c, 1);
    اگر (cc < 1)
      توڑو;
    buf[i++] = c;
    اگر (c == '\n' || c == '\r')
      توڑو;
  }
  buf[i] = '\0';
  واپس buf;
}

صحیح
stat(ثابت حرف *n, ساخت stat *st)
{
  صحیح fd;
  صحیح r;

  fd = open(n, O_RDONLY);
  اگر (fd < 0)
    واپس -1;
  r = fstat(fd, st);
  close(fd);
  واپس r;
}

صحیح
atoi(ثابت حرف *s)
{
  صحیح n;

  n = 0;
  جب ('0' <= *s && *s <= '9')
    n = n * 10 + *s++ - '0';
  واپس n;
}

خالی *
memmove(خالی *vdst, ثابت خالی *vsrc, صحیح n)
{
  حرف *dst;
  ثابت حرف *src;

  dst = vdst;
  src = vsrc;
  اگر (src > dst) {
    جب (n-- > 0)
      *dst++ = *src++;
  } ورنہ {
    dst += n;
    src += n;
    جب (n-- > 0)
      *--dst = *--src;
  }
  واپس vdst;
}

صحیح
memcmp(ثابت خالی *s1, ثابت خالی *s2, uint n)
{
  ثابت حرف *p1 = s1, *p2 = s2;
  جب (n-- > 0) {
    اگر (*p1 != *p2) {
      واپس *p1 - *p2;
    }
    p1++;
    p2++;
  }
  واپس 0;
}

خالی *
حافظہ_نقل(خالی *dst, ثابت خالی *src, uint n)
{
  واپس memmove(dst, src, n);
}

حرف *
sbrk(صحیح n)
{
  واپس sys_sbrk(n, SBRK_EAGER);
}

حرف *
sbrklazy(صحیح n)
{
  واپس sys_sbrk(n, SBRK_LAZY);
}
