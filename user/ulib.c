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
start(цео argc, char **argv)
{
  цео r;
  extern цео main(цео argc, char **argv);
  r = main(argc, argv);
  exit(r);
}

char *
strcpy(char *s, const char *t)
{
  char *os;

  os = s;
  док ((*s++ = *t++) != 0)
    ;
  врати os;
}

цео
strcmp(const char *p, const char *q)
{
  док (*p && *p == *q)
    p++, q++;
  врати (uchar)*p - (uchar)*q;
}

uint
strlen(const char *s)
{
  цео n;

  за (n = 0; s[n]; n++)
    ;
  врати n;
}

void *
memset(void *dst, цео c, uint n)
{
  char *cdst = (char *)dst;
  цео i;
  за (i = 0; i < n; i++) {
    cdst[i] = c;
  }
  врати dst;
}

char *
strchr(const char *s, char c)
{
  за (; *s; s++)
    ако (*s == c)
      врати (char *)s;
  врати 0;
}

char *
gets(char *buf, цео max)
{
  цео i, cc;
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
  врати buf;
}

цео
stat(const char *n, struct stat *st)
{
  цео fd;
  цео r;

  fd = open(n, O_RDONLY);
  ако (fd < 0)
    врати -1;
  r = fstat(fd, st);
  close(fd);
  врати r;
}

цео
atoi(const char *s)
{
  цео n;

  n = 0;
  док ('0' <= *s && *s <= '9')
    n = n * 10 + *s++ - '0';
  врати n;
}

void *
memmove(void *vdst, const void *vsrc, цео n)
{
  char *dst;
  const char *src;

  dst = vdst;
  src = vsrc;
  ако (src > dst) {
    док (n-- > 0)
      *dst++ = *src++;
  } инако {
    dst += n;
    src += n;
    док (n-- > 0)
      *--dst = *--src;
  }
  врати vdst;
}

цео
memcmp(const void *s1, const void *s2, uint n)
{
  const char *p1 = s1, *p2 = s2;
  док (n-- > 0) {
    ако (*p1 != *p2) {
      врати *p1 - *p2;
    }
    p1++;
    p2++;
  }
  врати 0;
}

void *
memcpy(void *dst, const void *src, uint n)
{
  врати memmove(dst, src, n);
}

char *
sbrk(цео n)
{
  врати sys_sbrk(n, SBRK_EAGER);
}

char *
sbrklazy(цео n)
{
  врати sys_sbrk(n, SBRK_LAZY);
}
