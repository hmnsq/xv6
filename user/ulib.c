#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "kernel/riscv.h"
#include "kernel/vm.h"
#include "user/user.h"

//
// wrapper so that it's OK if main() does not call exit().
//
void
start(ကိန်းပြည့် argc, var_na **argv)
{
  ကိန်းပြည့် r;
  extern ကိန်းပြည့် main(ကိန်းပြည့် argc, var_na **argv);
  r = main(argc, argv);
  exit(r);
}

var_na *
strcpy(var_na *s, const var_na *t)
{
  var_na *os;

  os = s;
  နေစဉ် ((*s++ = *t++) != 0)
    ;
  ပြန်ပေး os;
}

ကိန်းပြည့်
strcmp(const var_na *p, const var_na *q)
{
  နေစဉ် (*p && *p == *q)
    p++, q++;
  ပြန်ပေး (uchar)*p - (uchar)*q;
}

uint
strlen(const var_na *s)
{
  ကိန်းပြည့် n;

  အတွက် (n = 0; s[n]; n++)
    ;
  ပြန်ပေး n;
}

void *
memset(void *dst, ကိန်းပြည့် c, uint n)
{
  var_na *cdst = (var_na *)dst;
  ကိန်းပြည့် i;
  အတွက် (i = 0; i < n; i++) {
    cdst[i] = c;
  }
  ပြန်ပေး dst;
}

var_na *
strchr(const var_na *s, var_na c)
{
  အတွက် (; *s; s++)
    အကယ်၍ (*s == c)
      ပြန်ပေး (var_na *)s;
  ပြန်ပေး 0;
}

var_na *
gets(var_na *buf, ကိန်းပြည့် max)
{
  ကိန်းပြည့် i, cc;
  var_na c;

  အတွက် (i = 0; i + 1 < max;) {
    cc = read(0, &c, 1);
    အကယ်၍ (cc < 1)
      bha_nga;
    buf[i++] = c;
    အကယ်၍ (c == '\n' || c == '\r')
      bha_nga;
  }
  buf[i] = '\0';
  ပြန်ပေး buf;
}

ကိန်းပြည့်
stat(const var_na *n, sa_nracanaa stat *st)
{
  ကိန်းပြည့် fd;
  ကိန်းပြည့် r;

  fd = open(n, O_RDONLY);
  အကယ်၍ (fd < 0)
    ပြန်ပေး -1;
  r = fstat(fd, st);
  close(fd);
  ပြန်ပေး r;
}

ကိန်းပြည့်
atoi(const var_na *s)
{
  ကိန်းပြည့် n;

  n = 0;
  နေစဉ် ('0' <= *s && *s <= '9')
    n = n * 10 + *s++ - '0';
  ပြန်ပေး n;
}

void *
memmove(void *vdst, const void *vsrc, ကိန်းပြည့် n)
{
  var_na *dst;
  const var_na *src;

  dst = vdst;
  src = vsrc;
  အကယ်၍ (src > dst) {
    နေစဉ် (n-- > 0)
      *dst++ = *src++;
  } မဟုတ်ပါက {
    dst += n;
    src += n;
    နေစဉ် (n-- > 0)
      *--dst = *--src;
  }
  ပြန်ပေး vdst;
}

ကိန်းပြည့်
memcmp(const void *s1, const void *s2, uint n)
{
  const var_na *p1 = s1, *p2 = s2;
  နေစဉ် (n-- > 0) {
    အကယ်၍ (*p1 != *p2) {
      ပြန်ပေး *p1 - *p2;
    }
    p1++;
    p2++;
  }
  ပြန်ပေး 0;
}

void *
memcpy(void *dst, const void *src, uint n)
{
  ပြန်ပေး memmove(dst, src, n);
}

var_na *
sbrk(ကိန်းပြည့် n)
{
  ပြန်ပေး sys_sbrk(n, SBRK_EAGER);
}

var_na *
sbrklazy(ကိန်းပြည့် n)
{
  ပြန်ပေး sys_sbrk(n, SBRK_LAZY);
}
