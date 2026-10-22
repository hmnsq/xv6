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
start(정수 argc, char **argv)
{
  정수 r;
  extern 정수 main(정수 argc, char **argv);
  r = main(argc, argv);
  exit(r);
}

char *
strcpy(char *s, const char *t)
{
  char *os;

  os = s;
  동안 ((*s++ = *t++) != 0)
    ;
  반환 os;
}

정수
strcmp(const char *p, const char *q)
{
  동안 (*p && *p == *q)
    p++, q++;
  반환 (uchar)*p - (uchar)*q;
}

uint
strlen(const char *s)
{
  정수 n;

  위해 (n = 0; s[n]; n++)
    ;
  반환 n;
}

void *
memset(void *dst, 정수 c, uint n)
{
  char *cdst = (char *)dst;
  정수 i;
  위해 (i = 0; i < n; i++) {
    cdst[i] = c;
  }
  반환 dst;
}

char *
strchr(const char *s, char c)
{
  위해 (; *s; s++)
    만약 (*s == c)
      반환 (char *)s;
  반환 0;
}

char *
gets(char *buf, 정수 max)
{
  정수 i, cc;
  char c;

  위해 (i = 0; i + 1 < max;) {
    cc = read(0, &c, 1);
    만약 (cc < 1)
      break;
    buf[i++] = c;
    만약 (c == '\n' || c == '\r')
      break;
  }
  buf[i] = '\0';
  반환 buf;
}

정수
stat(const char *n, struct stat *st)
{
  정수 fd;
  정수 r;

  fd = open(n, O_RDONLY);
  만약 (fd < 0)
    반환 -1;
  r = fstat(fd, st);
  close(fd);
  반환 r;
}

정수
atoi(const char *s)
{
  정수 n;

  n = 0;
  동안 ('0' <= *s && *s <= '9')
    n = n * 10 + *s++ - '0';
  반환 n;
}

void *
memmove(void *vdst, const void *vsrc, 정수 n)
{
  char *dst;
  const char *src;

  dst = vdst;
  src = vsrc;
  만약 (src > dst) {
    동안 (n-- > 0)
      *dst++ = *src++;
  } 아니면 {
    dst += n;
    src += n;
    동안 (n-- > 0)
      *--dst = *--src;
  }
  반환 vdst;
}

정수
memcmp(const void *s1, const void *s2, uint n)
{
  const char *p1 = s1, *p2 = s2;
  동안 (n-- > 0) {
    만약 (*p1 != *p2) {
      반환 *p1 - *p2;
    }
    p1++;
    p2++;
  }
  반환 0;
}

void *
memcpy(void *dst, const void *src, uint n)
{
  반환 memmove(dst, src, n);
}

char *
sbrk(정수 n)
{
  반환 sys_sbrk(n, SBRK_EAGER);
}

char *
sbrklazy(정수 n)
{
  반환 sys_sbrk(n, SBRK_LAZY);
}
