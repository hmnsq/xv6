#অন্তর্ভাৱ "kernel/types.h"
#অন্তর্ভাৱ "kernel/stat.h"
#অন্তর্ভাৱ "kernel/fcntl.h"
#অন্তর্ভাৱ "kernel/riscv.h"
#অন্তর্ভাৱ "kernel/vm.h"
#অন্তর্ভাৱ "user/user.h"

//
// wrapper so that it's OK if main() does not call exit().
//
শূন্য
start(পূর্ণাঙ্ক argc, ৱর্ণ **argv)
{
  পূর্ণাঙ্ক r;
  বাহ্য পূর্ণাঙ্ক main(পূর্ণাঙ্ক argc, ৱর্ণ **argv);
  r = main(argc, argv);
  exit(r);
}

ৱর্ণ *
strcpy(ৱর্ণ *s, ধ্রুৱ ৱর্ণ *t)
{
  ৱর্ণ *os;

  os = s;
  যেতিয়ালৈ ((*s++ = *t++) != 0)
    ;
  ঘূৰাই os;
}

পূর্ণাঙ্ক
strcmp(ধ্রুৱ ৱর্ণ *p, ধ্রুৱ ৱর্ণ *q)
{
  যেতিয়ালৈ (*p && *p == *q)
    p++, q++;
  ঘূৰাই (uchar)*p - (uchar)*q;
}

uint
strlen(ধ্রুৱ ৱর্ণ *s)
{
  পূর্ণাঙ্ক n;

  বাবে (n = 0; s[n]; n++)
    ;
  ঘূৰাই n;
}

শূন্য *
memset(শূন্য *dst, পূর্ণাঙ্ক c, uint n)
{
  ৱর্ণ *cdst = (ৱর্ণ *)dst;
  পূর্ণাঙ্ক i;
  বাবে (i = 0; i < n; i++) {
    cdst[i] = c;
  }
  ঘূৰাই dst;
}

ৱর্ণ *
strchr(ধ্রুৱ ৱর্ণ *s, ৱর্ণ c)
{
  বাবে (; *s; s++)
    যদি (*s == c)
      ঘূৰাই (ৱর্ণ *)s;
  ঘূৰাই 0;
}

ৱর্ণ *
gets(ৱর্ণ *buf, পূর্ণাঙ্ক max)
{
  পূর্ণাঙ্ক i, cc;
  ৱর্ণ c;

  বাবে (i = 0; i + 1 < max;) {
    cc = পঠ(0, &c, 1);
    যদি (cc < 1)
      ভঙ্গ;
    buf[i++] = c;
    যদি (c == '\n' || c == '\r')
      ভঙ্গ;
  }
  buf[i] = '\0';
  ঘূৰাই buf;
}

পূর্ণাঙ্ক
stat(ধ্রুৱ ৱর্ণ *n, সংরচনা stat *st)
{
  পূর্ণাঙ্ক fd;
  পূর্ণাঙ্ক r;

  fd = open(n, O_RDONLY);
  যদি (fd < 0)
    ঘূৰাই -1;
  r = fstat(fd, st);
  close(fd);
  ঘূৰাই r;
}

পূর্ণাঙ্ক
atoi(ধ্রুৱ ৱর্ণ *s)
{
  পূর্ণাঙ্ক n;

  n = 0;
  যেতিয়ালৈ ('0' <= *s && *s <= '9')
    n = n * 10 + *s++ - '0';
  ঘূৰাই n;
}

শূন্য *
memmove(শূন্য *vdst, ধ্রুৱ শূন্য *vsrc, পূর্ণাঙ্ক n)
{
  ৱর্ণ *dst;
  ধ্রুৱ ৱর্ণ *src;

  dst = vdst;
  src = vsrc;
  যদি (src > dst) {
    যেতিয়ালৈ (n-- > 0)
      *dst++ = *src++;
  } অন্যথা {
    dst += n;
    src += n;
    যেতিয়ালৈ (n-- > 0)
      *--dst = *--src;
  }
  ঘূৰাই vdst;
}

পূর্ণাঙ্ক
memcmp(ধ্রুৱ শূন্য *s1, ধ্রুৱ শূন্য *s2, uint n)
{
  ধ্রুৱ ৱর্ণ *p1 = s1, *p2 = s2;
  যেতিয়ালৈ (n-- > 0) {
    যদি (*p1 != *p2) {
      ঘূৰাই *p1 - *p2;
    }
    p1++;
    p2++;
  }
  ঘূৰাই 0;
}

শূন্য *
memcpy(শূন্য *dst, ধ্রুৱ শূন্য *src, uint n)
{
  ঘূৰাই memmove(dst, src, n);
}

ৱর্ণ *
sbrk(পূর্ণাঙ্ক n)
{
  ঘূৰাই sys_sbrk(n, SBRK_EAGER);
}

ৱর্ণ *
sbrklazy(পূর্ণাঙ্ক n)
{
  ঘূৰাই sys_sbrk(n, SBRK_LAZY);
}
