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
start(ціле argc, char **argv)
{
  ціле r;
  extern ціле main(ціле argc, char **argv);
  r = main(argc, argv);
  exit(r);
}

char *
strcpy(char *s, const char *t)
{
  char *os;

  os = s;
  поки ((*s++ = *t++) != 0)
    ;
  повернути os;
}

ціле
strcmp(const char *p, const char *q)
{
  поки (*p && *p == *q)
    p++, q++;
  повернути (uchar)*p - (uchar)*q;
}

uint
strlen(const char *s)
{
  ціле n;

  для (n = 0; s[n]; n++)
    ;
  повернути n;
}

void *
memset(void *dst, ціле c, uint n)
{
  char *cdst = (char *)dst;
  ціле i;
  для (i = 0; i < n; i++) {
    cdst[i] = c;
  }
  повернути dst;
}

char *
strchr(const char *s, char c)
{
  для (; *s; s++)
    якщо (*s == c)
      повернути (char *)s;
  повернути 0;
}

char *
gets(char *buf, ціле max)
{
  ціле i, cc;
  char c;

  для (i = 0; i + 1 < max;) {
    cc = read(0, &c, 1);
    якщо (cc < 1)
      break;
    buf[i++] = c;
    якщо (c == '\n' || c == '\r')
      break;
  }
  buf[i] = '\0';
  повернути buf;
}

ціле
stat(const char *n, struct stat *st)
{
  ціле fd;
  ціле r;

  fd = open(n, O_RDONLY);
  якщо (fd < 0)
    повернути -1;
  r = fstat(fd, st);
  close(fd);
  повернути r;
}

ціле
atoi(const char *s)
{
  ціле n;

  n = 0;
  поки ('0' <= *s && *s <= '9')
    n = n * 10 + *s++ - '0';
  повернути n;
}

void *
memmove(void *vdst, const void *vsrc, ціле n)
{
  char *dst;
  const char *src;

  dst = vdst;
  src = vsrc;
  якщо (src > dst) {
    поки (n-- > 0)
      *dst++ = *src++;
  } інакше {
    dst += n;
    src += n;
    поки (n-- > 0)
      *--dst = *--src;
  }
  повернути vdst;
}

ціле
memcmp(const void *s1, const void *s2, uint n)
{
  const char *p1 = s1, *p2 = s2;
  поки (n-- > 0) {
    якщо (*p1 != *p2) {
      повернути *p1 - *p2;
    }
    p1++;
    p2++;
  }
  повернути 0;
}

void *
memcpy(void *dst, const void *src, uint n)
{
  повернути memmove(dst, src, n);
}

char *
sbrk(ціле n)
{
  повернути sys_sbrk(n, SBRK_EAGER);
}

char *
sbrklazy(ціле n)
{
  повернути sys_sbrk(n, SBRK_LAZY);
}
