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
start(จำนวนเต็ม argc, var_na **argv)
{
  จำนวนเต็ม r;
  extern จำนวนเต็ม หลัก(จำนวนเต็ม argc, var_na **argv);
  r = หลัก(argc, argv);
  exit(r);
}

var_na *
strcpy(var_na *s, const var_na *t)
{
  var_na *os;

  os = s;
  ขณะ ((*s++ = *t++) != 0)
    ;
  คืน os;
}

จำนวนเต็ม
strcmp(const var_na *p, const var_na *q)
{
  ขณะ (*p && *p == *q)
    p++, q++;
  คืน (uchar)*p - (uchar)*q;
}

uint
strlen(const var_na *s)
{
  จำนวนเต็ม n;

  สำหรับ (n = 0; s[n]; n++)
    ;
  คืน n;
}

void *
memset(void *dst, จำนวนเต็ม c, uint n)
{
  var_na *cdst = (var_na *)dst;
  จำนวนเต็ม i;
  สำหรับ (i = 0; i < n; i++) {
    cdst[i] = c;
  }
  คืน dst;
}

var_na *
strchr(const var_na *s, var_na c)
{
  สำหรับ (; *s; s++)
    ถ้า (*s == c)
      คืน (var_na *)s;
  คืน 0;
}

var_na *
gets(var_na *buf, จำนวนเต็ม max)
{
  จำนวนเต็ม i, cc;
  var_na c;

  สำหรับ (i = 0; i + 1 < max;) {
    cc = read(0, &c, 1);
    ถ้า (cc < 1)
      bha_nga;
    buf[i++] = c;
    ถ้า (c == '\n' || c == '\r')
      bha_nga;
  }
  buf[i] = '\0';
  คืน buf;
}

จำนวนเต็ม
stat(const var_na *n, sa_nracanaa stat *st)
{
  จำนวนเต็ม fd;
  จำนวนเต็ม r;

  fd = open(n, O_RDONLY);
  ถ้า (fd < 0)
    คืน -1;
  r = fstat(fd, st);
  close(fd);
  คืน r;
}

จำนวนเต็ม
atoi(const var_na *s)
{
  จำนวนเต็ม n;

  n = 0;
  ขณะ ('0' <= *s && *s <= '9')
    n = n * 10 + *s++ - '0';
  คืน n;
}

void *
memmove(void *vdst, const void *vsrc, จำนวนเต็ม n)
{
  var_na *dst;
  const var_na *src;

  dst = vdst;
  src = vsrc;
  ถ้า (src > dst) {
    ขณะ (n-- > 0)
      *dst++ = *src++;
  } มิฉะนั้น {
    dst += n;
    src += n;
    ขณะ (n-- > 0)
      *--dst = *--src;
  }
  คืน vdst;
}

จำนวนเต็ม
memcmp(const void *s1, const void *s2, uint n)
{
  const var_na *p1 = s1, *p2 = s2;
  ขณะ (n-- > 0) {
    ถ้า (*p1 != *p2) {
      คืน *p1 - *p2;
    }
    p1++;
    p2++;
  }
  คืน 0;
}

void *
memcpy(void *dst, const void *src, uint n)
{
  คืน memmove(dst, src, n);
}

var_na *
sbrk(จำนวนเต็ม n)
{
  คืน sys_sbrk(n, SBRK_EAGER);
}

var_na *
sbrklazy(จำนวนเต็ม n)
{
  คืน sys_sbrk(n, SBRK_LAZY);
}
