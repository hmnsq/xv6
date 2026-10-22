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
start(целое argc, char **argv)
{
  целое r;
  extern целое основной(целое argc, char **argv);
  r = основной(argc, argv);
  exit(r);
}

char *
strcpy(char *s, const char *t)
{
  char *os;

  os = s;
  пока ((*s++ = *t++) != 0)
    ;
  вернуть os;
}

целое
strcmp(const char *p, const char *q)
{
  пока (*p && *p == *q)
    p++, q++;
  вернуть (uchar)*p - (uchar)*q;
}

uint
strlen(const char *s)
{
  целое n;

  для (n = 0; s[n]; n++)
    ;
  вернуть n;
}

void *
memset(void *dst, целое c, uint n)
{
  char *cdst = (char *)dst;
  целое i;
  для (i = 0; i < n; i++) {
    cdst[i] = c;
  }
  вернуть dst;
}

char *
strchr(const char *s, char c)
{
  для (; *s; s++)
    если (*s == c)
      вернуть (char *)s;
  вернуть 0;
}

char *
gets(char *buf, целое max)
{
  целое i, cc;
  char c;

  для (i = 0; i + 1 < max;) {
    cc = read(0, &c, 1);
    если (cc < 1)
      break;
    buf[i++] = c;
    если (c == '\n' || c == '\r')
      break;
  }
  buf[i] = '\0';
  вернуть buf;
}

целое
stat(const char *n, struct stat *st)
{
  целое fd;
  целое r;

  fd = open(n, O_RDONLY);
  если (fd < 0)
    вернуть -1;
  r = fstat(fd, st);
  close(fd);
  вернуть r;
}

целое
atoi(const char *s)
{
  целое n;

  n = 0;
  пока ('0' <= *s && *s <= '9')
    n = n * 10 + *s++ - '0';
  вернуть n;
}

void *
memmove(void *vdst, const void *vsrc, целое n)
{
  char *dst;
  const char *src;

  dst = vdst;
  src = vsrc;
  если (src > dst) {
    пока (n-- > 0)
      *dst++ = *src++;
  } иначе {
    dst += n;
    src += n;
    пока (n-- > 0)
      *--dst = *--src;
  }
  вернуть vdst;
}

целое
memcmp(const void *s1, const void *s2, uint n)
{
  const char *p1 = s1, *p2 = s2;
  пока (n-- > 0) {
    если (*p1 != *p2) {
      вернуть *p1 - *p2;
    }
    p1++;
    p2++;
  }
  вернуть 0;
}

void *
memcpy(void *dst, const void *src, uint n)
{
  вернуть memmove(dst, src, n);
}

char *
sbrk(целое n)
{
  вернуть sys_sbrk(n, SBRK_EAGER);
}

char *
sbrklazy(целое n)
{
  вернуть sys_sbrk(n, SBRK_LAZY);
}
