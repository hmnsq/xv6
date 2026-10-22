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
  extern 整数 メインページ(整数 argc, char **argv);
  r = メインページ(argc, argv);
  exit(r);
}

char *
strcpy(char *s, const char *t)
{
  char *os;

  os = s;
  ながら ((*s++ = *t++) != 0)
    ;
  戻る os;
}

整数
strcmp(const char *p, const char *q)
{
  ながら (*p && *p == *q)
    p++, q++;
  戻る (uchar)*p - (uchar)*q;
}

uint
strlen(const char *s)
{
  整数 n;

  ために (n = 0; s[n]; n++)
    ;
  戻る n;
}

void *
memset(void *dst, 整数 c, uint n)
{
  char *cdst = (char *)dst;
  整数 i;
  ために (i = 0; i < n; i++) {
    cdst[i] = c;
  }
  戻る dst;
}

char *
strchr(const char *s, char c)
{
  ために (; *s; s++)
    もし (*s == c)
      戻る (char *)s;
  戻る 0;
}

char *
gets(char *buf, 整数 max)
{
  整数 i, cc;
  char c;

  ために (i = 0; i + 1 < max;) {
    cc = read(0, &c, 1);
    もし (cc < 1)
      break;
    buf[i++] = c;
    もし (c == '\n' || c == '\r')
      break;
  }
  buf[i] = '\0';
  戻る buf;
}

整数
stat(const char *n, struct stat *st)
{
  整数 fd;
  整数 r;

  fd = open(n, O_RDONLY);
  もし (fd < 0)
    戻る -1;
  r = fstat(fd, st);
  close(fd);
  戻る r;
}

整数
atoi(const char *s)
{
  整数 n;

  n = 0;
  ながら ('0' <= *s && *s <= '9')
    n = n * 10 + *s++ - '0';
  戻る n;
}

void *
memmove(void *vdst, const void *vsrc, 整数 n)
{
  char *dst;
  const char *src;

  dst = vdst;
  src = vsrc;
  もし (src > dst) {
    ながら (n-- > 0)
      *dst++ = *src++;
  } 否则 {
    dst += n;
    src += n;
    ながら (n-- > 0)
      *--dst = *--src;
  }
  戻る vdst;
}

整数
memcmp(const void *s1, const void *s2, uint n)
{
  const char *p1 = s1, *p2 = s2;
  ながら (n-- > 0) {
    もし (*p1 != *p2) {
      戻る *p1 - *p2;
    }
    p1++;
    p2++;
  }
  戻る 0;
}

void *
memcpy(void *dst, const void *src, uint n)
{
  戻る memmove(dst, src, n);
}

char *
sbrk(整数 n)
{
  戻る sys_sbrk(n, SBRK_EAGER);
}

char *
sbrklazy(整数 n)
{
  戻る sys_sbrk(n, SBRK_LAZY);
}
