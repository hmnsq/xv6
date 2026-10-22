#شامل "kernel/types.h"
#شامل "kernel/stat.h"
#شامل "kernel/fcntl.h"
#شامل "kernel/riscv.h"
#شامل "kernel/vm.h"
#شامل "user/user.h"

//
// wrapper so that it's OK if main() does not call exit().
//
تهی
start(صحیح argc, نویسه **argv)
{
  صحیح r;
  بیرونی صحیح اصلی(صحیح argc, نویسه **argv);
  r = اصلی(argc, argv);
  exit(r);
}

نویسه *
strcpy(نویسه *s, ثابت نویسه *t)
{
  نویسه *os;

  os = s;
  وقتی ((*s++ = *t++) != 0)
    ;
  بازگشت os;
}

صحیح
strcmp(ثابت نویسه *p, ثابت نویسه *q)
{
  وقتی (*p && *p == *q)
    p++, q++;
  بازگشت (uchar)*p - (uchar)*q;
}

uint
strlen(ثابت نویسه *s)
{
  صحیح n;

  برای (n = 0; s[n]; n++)
    ;
  بازگشت n;
}

تهی *
memset(تهی *dst, صحیح c, uint n)
{
  نویسه *cdst = (نویسه *)dst;
  صحیح i;
  برای (i = 0; i < n; i++) {
    cdst[i] = c;
  }
  بازگشت dst;
}

نویسه *
strchr(ثابت نویسه *s, نویسه c)
{
  برای (; *s; s++)
    اگر (*s == c)
      بازگشت (نویسه *)s;
  بازگشت 0;
}

نویسه *
gets(نویسه *buf, صحیح max)
{
  صحیح i, cc;
  نویسه c;

  برای (i = 0; i + 1 < max;) {
    cc = اقرأ(0, &c, 1);
    اگر (cc < 1)
      بشکن;
    buf[i++] = c;
    اگر (c == '\n' || c == '\r')
      بشکن;
  }
  buf[i] = '\0';
  بازگشت buf;
}

صحیح
stat(ثابت نویسه *n, ساختار stat *st)
{
  صحیح fd;
  صحیح r;

  fd = open(n, O_RDONLY);
  اگر (fd < 0)
    بازگشت -1;
  r = fstat(fd, st);
  close(fd);
  بازگشت r;
}

صحیح
atoi(ثابت نویسه *s)
{
  صحیح n;

  n = 0;
  وقتی ('0' <= *s && *s <= '9')
    n = n * 10 + *s++ - '0';
  بازگشت n;
}

تهی *
memmove(تهی *vdst, ثابت تهی *vsrc, صحیح n)
{
  نویسه *dst;
  ثابت نویسه *src;

  dst = vdst;
  src = vsrc;
  اگر (src > dst) {
    وقتی (n-- > 0)
      *dst++ = *src++;
  } وگرنه {
    dst += n;
    src += n;
    وقتی (n-- > 0)
      *--dst = *--src;
  }
  بازگشت vdst;
}

صحیح
memcmp(ثابت تهی *s1, ثابت تهی *s2, uint n)
{
  ثابت نویسه *p1 = s1, *p2 = s2;
  وقتی (n-- > 0) {
    اگر (*p1 != *p2) {
      بازگشت *p1 - *p2;
    }
    p1++;
    p2++;
  }
  بازگشت 0;
}

تهی *
memcpy(تهی *dst, ثابت تهی *src, uint n)
{
  بازگشت memmove(dst, src, n);
}

نویسه *
sbrk(صحیح n)
{
  بازگشت sys_sbrk(n, SBRK_EAGER);
}

نویسه *
sbrklazy(صحیح n)
{
  بازگشت sys_sbrk(n, SBRK_LAZY);
}
