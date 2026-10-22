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
start(整数 argc, char **argv)
{
  整数 r;
  extern 整数 主要(整数 argc, char **argv);
  r = 主要(argc, argv);
  exit(r);
}

char *
strcpy(char *s, const char *t)
{
  char *os;

  os = s;
  当 ((*s++ = *t++) != 0)
    ;
  返回 os;
}

整数
strcmp(const char *p, const char *q)
{
  当 (*p && *p == *q)
    p++, q++;
  返回 (uchar)*p - (uchar)*q;
}

uint
strlen(const char *s)
{
  整数 n;

  为 (n = 0; s[n]; n++)
    ;
  返回 n;
}

void *
memset(void *dst, 整数 c, uint n)
{
  char *cdst = (char *)dst;
  整数 i;
  为 (i = 0; i < n; i++) {
    cdst[i] = c;
  }
  返回 dst;
}

char *
strchr(const char *s, char c)
{
  为 (; *s; s++)
    如果 (*s == c)
      返回 (char *)s;
  返回 0;
}

char *
gets(char *buf, 整数 max)
{
  整数 i, cc;
  char c;

  为 (i = 0; i + 1 < max;) {
    cc = read(0, &c, 1);
    如果 (cc < 1)
      break;
    buf[i++] = c;
    如果 (c == '\n' || c == '\r')
      break;
  }
  buf[i] = '\0';
  返回 buf;
}

整数
stat(const char *n, struct stat *st)
{
  整数 fd;
  整数 r;

  fd = open(n, O_RDONLY);
  如果 (fd < 0)
    返回 -1;
  r = fstat(fd, st);
  close(fd);
  返回 r;
}

整数
atoi(const char *s)
{
  整数 n;

  n = 0;
  当 ('0' <= *s && *s <= '9')
    n = n * 10 + *s++ - '0';
  返回 n;
}

void *
memmove(void *vdst, const void *vsrc, 整数 n)
{
  char *dst;
  const char *src;

  dst = vdst;
  src = vsrc;
  如果 (src > dst) {
    当 (n-- > 0)
      *dst++ = *src++;
  } 否则 {
    dst += n;
    src += n;
    当 (n-- > 0)
      *--dst = *--src;
  }
  返回 vdst;
}

整数
memcmp(const void *s1, const void *s2, uint n)
{
  const char *p1 = s1, *p2 = s2;
  当 (n-- > 0) {
    如果 (*p1 != *p2) {
      返回 *p1 - *p2;
    }
    p1++;
    p2++;
  }
  返回 0;
}

void *
memcpy(void *dst, const void *src, uint n)
{
  返回 memmove(dst, src, n);
}

char *
sbrk(整数 n)
{
  返回 sys_sbrk(n, SBRK_EAGER);
}

char *
sbrklazy(整数 n)
{
  返回 sys_sbrk(n, SBRK_LAZY);
}
