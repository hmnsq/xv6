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
start(পূর্ণাঙ্ক argc, বর্ণ **argv)
{
  পূর্ণাঙ্ক r;
  বাহ্য পূর্ণাঙ্ক main(পূর্ণাঙ্ক argc, বর্ণ **argv);
  r = main(argc, argv);
  exit(r);
}

বর্ণ *
strcpy(বর্ণ *s, ধ্রুব বর্ণ *t)
{
  বর্ণ *os;

  os = s;
  যাবত্ ((*s++ = *t++) != 0)
    ;
  প্রত্যাগম os;
}

পূর্ণাঙ্ক
strcmp(ধ্রুব বর্ণ *p, ধ্রুব বর্ণ *q)
{
  যাবত্ (*p && *p == *q)
    p++, q++;
  প্রত্যাগম (uchar)*p - (uchar)*q;
}

uint
strlen(ধ্রুব বর্ণ *s)
{
  পূর্ণাঙ্ক n;

  প্রতি (n = 0; s[n]; n++)
    ;
  প্রত্যাগম n;
}

শূন্য *
memset(শূন্য *dst, পূর্ণাঙ্ক c, uint n)
{
  বর্ণ *cdst = (বর্ণ *)dst;
  পূর্ণাঙ্ক i;
  প্রতি (i = 0; i < n; i++) {
    cdst[i] = c;
  }
  প্রত্যাগম dst;
}

বর্ণ *
strchr(ধ্রুব বর্ণ *s, বর্ণ c)
{
  প্রতি (; *s; s++)
    যদি (*s == c)
      প্রত্যাগম (বর্ণ *)s;
  প্রত্যাগম 0;
}

বর্ণ *
gets(বর্ণ *buf, পূর্ণাঙ্ক max)
{
  পূর্ণাঙ্ক i, cc;
  বর্ণ c;

  প্রতি (i = 0; i + 1 < max;) {
    cc = পঠ(0, &c, 1);
    যদি (cc < 1)
      ভঙ্গ;
    buf[i++] = c;
    যদি (c == '\n' || c == '\r')
      ভঙ্গ;
  }
  buf[i] = '\0';
  প্রত্যাগম buf;
}

পূর্ণাঙ্ক
stat(ধ্রুব বর্ণ *n, সংরচনা stat *st)
{
  পূর্ণাঙ্ক fd;
  পূর্ণাঙ্ক r;

  fd = open(n, O_RDONLY);
  যদি (fd < 0)
    প্রত্যাগম -1;
  r = fstat(fd, st);
  close(fd);
  প্রত্যাগম r;
}

পূর্ণাঙ্ক
atoi(ধ্রুব বর্ণ *s)
{
  পূর্ণাঙ্ক n;

  n = 0;
  যাবত্ ('0' <= *s && *s <= '9')
    n = n * 10 + *s++ - '0';
  প্রত্যাগম n;
}

শূন্য *
memmove(শূন্য *vdst, ধ্রুব শূন্য *vsrc, পূর্ণাঙ্ক n)
{
  বর্ণ *dst;
  ধ্রুব বর্ণ *src;

  dst = vdst;
  src = vsrc;
  যদি (src > dst) {
    যাবত্ (n-- > 0)
      *dst++ = *src++;
  } অন্যথা {
    dst += n;
    src += n;
    যাবত্ (n-- > 0)
      *--dst = *--src;
  }
  প্রত্যাগম vdst;
}

পূর্ণাঙ্ক
memcmp(ধ্রুব শূন্য *s1, ধ্রুব শূন্য *s2, uint n)
{
  ধ্রুব বর্ণ *p1 = s1, *p2 = s2;
  যাবত্ (n-- > 0) {
    যদি (*p1 != *p2) {
      প্রত্যাগম *p1 - *p2;
    }
    p1++;
    p2++;
  }
  প্রত্যাগম 0;
}

শূন্য *
memcpy(শূন্য *dst, ধ্রুব শূন্য *src, uint n)
{
  প্রত্যাগম memmove(dst, src, n);
}

বর্ণ *
sbrk(পূর্ণাঙ্ক n)
{
  প্রত্যাগম sys_sbrk(n, SBRK_EAGER);
}

বর্ণ *
sbrklazy(পূর্ণাঙ্ক n)
{
  প্রত্যাগম sys_sbrk(n, SBRK_LAZY);
}
