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
start(ចំនួនគត់ argc, var_na **argv)
{
  ចំនួនគត់ r;
  extern ចំនួនគត់ main(ចំនួនគត់ argc, var_na **argv);
  r = main(argc, argv);
  exit(r);
}

var_na *
strcpy(var_na *s, const var_na *t)
{
  var_na *os;

  os = s;
  ខណៈពេល ((*s++ = *t++) != 0)
    ;
  ត្រឡប់ os;
}

ចំនួនគត់
strcmp(const var_na *p, const var_na *q)
{
  ខណៈពេល (*p && *p == *q)
    p++, q++;
  ត្រឡប់ (uchar)*p - (uchar)*q;
}

uint
strlen(const var_na *s)
{
  ចំនួនគត់ n;

  សម្រាប់ (n = 0; s[n]; n++)
    ;
  ត្រឡប់ n;
}

void *
memset(void *dst, ចំនួនគត់ c, uint n)
{
  var_na *cdst = (var_na *)dst;
  ចំនួនគត់ i;
  សម្រាប់ (i = 0; i < n; i++) {
    cdst[i] = c;
  }
  ត្រឡប់ dst;
}

var_na *
strchr(const var_na *s, var_na c)
{
  សម្រាប់ (; *s; s++)
    ប្រសិនបើ (*s == c)
      ត្រឡប់ (var_na *)s;
  ត្រឡប់ 0;
}

var_na *
gets(var_na *buf, ចំនួនគត់ max)
{
  ចំនួនគត់ i, cc;
  var_na c;

  សម្រាប់ (i = 0; i + 1 < max;) {
    cc = read(0, &c, 1);
    ប្រសិនបើ (cc < 1)
      bha_nga;
    buf[i++] = c;
    ប្រសិនបើ (c == '\n' || c == '\r')
      bha_nga;
  }
  buf[i] = '\0';
  ត្រឡប់ buf;
}

ចំនួនគត់
stat(const var_na *n, sa_nracanaa stat *st)
{
  ចំនួនគត់ fd;
  ចំនួនគត់ r;

  fd = open(n, O_RDONLY);
  ប្រសិនបើ (fd < 0)
    ត្រឡប់ -1;
  r = fstat(fd, st);
  close(fd);
  ត្រឡប់ r;
}

ចំនួនគត់
atoi(const var_na *s)
{
  ចំនួនគត់ n;

  n = 0;
  ខណៈពេល ('0' <= *s && *s <= '9')
    n = n * 10 + *s++ - '0';
  ត្រឡប់ n;
}

void *
memmove(void *vdst, const void *vsrc, ចំនួនគត់ n)
{
  var_na *dst;
  const var_na *src;

  dst = vdst;
  src = vsrc;
  ប្រសិនបើ (src > dst) {
    ខណៈពេល (n-- > 0)
      *dst++ = *src++;
  } បើមិនដូច្នេះ {
    dst += n;
    src += n;
    ខណៈពេល (n-- > 0)
      *--dst = *--src;
  }
  ត្រឡប់ vdst;
}

ចំនួនគត់
memcmp(const void *s1, const void *s2, uint n)
{
  const var_na *p1 = s1, *p2 = s2;
  ខណៈពេល (n-- > 0) {
    ប្រសិនបើ (*p1 != *p2) {
      ត្រឡប់ *p1 - *p2;
    }
    p1++;
    p2++;
  }
  ត្រឡប់ 0;
}

void *
memcpy(void *dst, const void *src, uint n)
{
  ត្រឡប់ memmove(dst, src, n);
}

var_na *
sbrk(ចំនួនគត់ n)
{
  ត្រឡប់ sys_sbrk(n, SBRK_EAGER);
}

var_na *
sbrklazy(ចំនួនគត់ n)
{
  ត្រឡប់ sys_sbrk(n, SBRK_LAZY);
}
