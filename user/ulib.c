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
start(цяло argc, char **argv)
{
  цяло r;
  extern цяло main(цяло argc, char **argv);
  r = main(argc, argv);
  exit(r);
}

char *
strcpy(char *s, const char *t)
{
  char *os;

  os = s;
  докато ((*s++ = *t++) != 0)
    ;
  върни os;
}

цяло
strcmp(const char *p, const char *q)
{
  докато (*p && *p == *q)
    p++, q++;
  върни (uchar)*p - (uchar)*q;
}

uint
strlen(const char *s)
{
  цяло n;

  за (n = 0; s[n]; n++)
    ;
  върни n;
}

void *
memset(void *dst, цяло c, uint n)
{
  char *cdst = (char *)dst;
  цяло i;
  за (i = 0; i < n; i++) {
    cdst[i] = c;
  }
  върни dst;
}

char *
strchr(const char *s, char c)
{
  за (; *s; s++)
    ако (*s == c)
      върни (char *)s;
  върни 0;
}

char *
gets(char *buf, цяло max)
{
  цяло i, cc;
  char c;

  за (i = 0; i + 1 < max;) {
    cc = read(0, &c, 1);
    ако (cc < 1)
      break;
    buf[i++] = c;
    ако (c == '\n' || c == '\r')
      break;
  }
  buf[i] = '\0';
  върни buf;
}

цяло
stat(const char *n, struct stat *st)
{
  цяло fd;
  цяло r;

  fd = open(n, O_RDONLY);
  ако (fd < 0)
    върни -1;
  r = fstat(fd, st);
  close(fd);
  върни r;
}

цяло
atoi(const char *s)
{
  цяло n;

  n = 0;
  докато ('0' <= *s && *s <= '9')
    n = n * 10 + *s++ - '0';
  върни n;
}

void *
memmove(void *vdst, const void *vsrc, цяло n)
{
  char *dst;
  const char *src;

  dst = vdst;
  src = vsrc;
  ако (src > dst) {
    докато (n-- > 0)
      *dst++ = *src++;
  } иначе {
    dst += n;
    src += n;
    докато (n-- > 0)
      *--dst = *--src;
  }
  върни vdst;
}

цяло
memcmp(const void *s1, const void *s2, uint n)
{
  const char *p1 = s1, *p2 = s2;
  докато (n-- > 0) {
    ако (*p1 != *p2) {
      върни *p1 - *p2;
    }
    p1++;
    p2++;
  }
  върни 0;
}

void *
memcpy(void *dst, const void *src, uint n)
{
  върни memmove(dst, src, n);
}

char *
sbrk(цяло n)
{
  върни sys_sbrk(n, SBRK_EAGER);
}

char *
sbrklazy(цяло n)
{
  върни sys_sbrk(n, SBRK_LAZY);
}
