#অন্তর্ভাব "kernel/types.h"
#অন্তর্ভাব "kernel/stat.h"
#অন্তর্ভাব "kernel/fcntl.h"
#অন্তর্ভাব "kernel/riscv.h"
#অন্তর্ভাব "kernel/vm.h"
#অন্তর্ভাব "user/user.h"

//
// wrapper so that it's OK if main() does not call exit().
//
শূন্য
start(পূর্ণসংখ্যা argc, অক্ষর **argv)
{
  পূর্ণসংখ্যা r;
  বাহ্যিক পূর্ণসংখ্যা প্রধান(পূর্ণসংখ্যা argc, অক্ষর **argv);
  r = প্রধান(argc, argv);
  exit(r);
}

অক্ষর *
strcpy(অক্ষর *s, ধ্রুবক অক্ষর *t)
{
  অক্ষর *os;

  os = s;
  যখন ((*s++ = *t++) != 0)
    ;
  ফেরত os;
}

পূর্ণসংখ্যা
strcmp(ধ্রুবক অক্ষর *p, ধ্রুবক অক্ষর *q)
{
  যখন (*p && *p == *q)
    p++, q++;
  ফেরত (uchar)*p - (uchar)*q;
}

uint
strlen(ধ্রুবক অক্ষর *s)
{
  পূর্ণসংখ্যা n;

  জন্য (n = 0; s[n]; n++)
    ;
  ফেরত n;
}

শূন্য *
memset(শূন্য *dst, পূর্ণসংখ্যা c, uint n)
{
  অক্ষর *cdst = (অক্ষর *)dst;
  পূর্ণসংখ্যা i;
  জন্য (i = 0; i < n; i++) {
    cdst[i] = c;
  }
  ফেরত dst;
}

অক্ষর *
strchr(ধ্রুবক অক্ষর *s, অক্ষর c)
{
  জন্য (; *s; s++)
    যদি (*s == c)
      ফেরত (অক্ষর *)s;
  ফেরত 0;
}

অক্ষর *
gets(অক্ষর *buf, পূর্ণসংখ্যা max)
{
  পূর্ণসংখ্যা i, cc;
  অক্ষর c;

  জন্য (i = 0; i + 1 < max;) {
    cc = পঠ(0, &c, 1);
    যদি (cc < 1)
      বিরতি;
    buf[i++] = c;
    যদি (c == '\n' || c == '\r')
      বিরতি;
  }
  buf[i] = '\0';
  ফেরত buf;
}

পূর্ণসংখ্যা
stat(ধ্রুবক অক্ষর *n, কাঠামো stat *st)
{
  পূর্ণসংখ্যা fd;
  পূর্ণসংখ্যা r;

  fd = open(n, O_RDONLY);
  যদি (fd < 0)
    ফেরত -1;
  r = fstat(fd, st);
  close(fd);
  ফেরত r;
}

পূর্ণসংখ্যা
atoi(ধ্রুবক অক্ষর *s)
{
  পূর্ণসংখ্যা n;

  n = 0;
  যখন ('0' <= *s && *s <= '9')
    n = n * 10 + *s++ - '0';
  ফেরত n;
}

শূন্য *
memmove(শূন্য *vdst, ধ্রুবক শূন্য *vsrc, পূর্ণসংখ্যা n)
{
  অক্ষর *dst;
  ধ্রুবক অক্ষর *src;

  dst = vdst;
  src = vsrc;
  যদি (src > dst) {
    যখন (n-- > 0)
      *dst++ = *src++;
  } অন্যথা {
    dst += n;
    src += n;
    যখন (n-- > 0)
      *--dst = *--src;
  }
  ফেরত vdst;
}

পূর্ণসংখ্যা
memcmp(ধ্রুবক শূন্য *s1, ধ্রুবক শূন্য *s2, uint n)
{
  ধ্রুবক অক্ষর *p1 = s1, *p2 = s2;
  যখন (n-- > 0) {
    যদি (*p1 != *p2) {
      ফেরত *p1 - *p2;
    }
    p1++;
    p2++;
  }
  ফেরত 0;
}

শূন্য *
memcpy(শূন্য *dst, ধ্রুবক শূন্য *src, uint n)
{
  ফেরত memmove(dst, src, n);
}

অক্ষর *
sbrk(পূর্ণসংখ্যা n)
{
  ফেরত sys_sbrk(n, SBRK_EAGER);
}

অক্ষর *
sbrklazy(পূর্ণসংখ্যা n)
{
  ফেরত sys_sbrk(n, SBRK_LAZY);
}
