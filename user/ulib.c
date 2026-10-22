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
start(số_nguyên argc, char **argv)
{
  số_nguyên r;
  extern số_nguyên main(số_nguyên argc, char **argv);
  r = main(argc, argv);
  exit(r);
}

char *
strcpy(char *s, const char *t)
{
  char *os;

  os = s;
  trong_khi ((*s++ = *t++) != 0)
    ;
  trả_về os;
}

số_nguyên
strcmp(const char *p, const char *q)
{
  trong_khi (*p && *p == *q)
    p++, q++;
  trả_về (uchar)*p - (uchar)*q;
}

uint
strlen(const char *s)
{
  số_nguyên n;

  cho (n = 0; s[n]; n++)
    ;
  trả_về n;
}

void *
memset(void *dst, số_nguyên c, uint n)
{
  char *cdst = (char *)dst;
  số_nguyên i;
  cho (i = 0; i < n; i++) {
    cdst[i] = c;
  }
  trả_về dst;
}

char *
strchr(const char *s, char c)
{
  cho (; *s; s++)
    nếu (*s == c)
      trả_về (char *)s;
  trả_về 0;
}

char *
gets(char *buf, số_nguyên max)
{
  số_nguyên i, cc;
  char c;

  cho (i = 0; i + 1 < max;) {
    cc = read(0, &c, 1);
    nếu (cc < 1)
      break;
    buf[i++] = c;
    nếu (c == '\n' || c == '\r')
      break;
  }
  buf[i] = '\0';
  trả_về buf;
}

số_nguyên
stat(const char *n, struct stat *st)
{
  số_nguyên fd;
  số_nguyên r;

  fd = open(n, O_RDONLY);
  nếu (fd < 0)
    trả_về -1;
  r = fstat(fd, st);
  close(fd);
  trả_về r;
}

số_nguyên
atoi(const char *s)
{
  số_nguyên n;

  n = 0;
  trong_khi ('0' <= *s && *s <= '9')
    n = n * 10 + *s++ - '0';
  trả_về n;
}

void *
memmove(void *vdst, const void *vsrc, số_nguyên n)
{
  char *dst;
  const char *src;

  dst = vdst;
  src = vsrc;
  nếu (src > dst) {
    trong_khi (n-- > 0)
      *dst++ = *src++;
  } không_thì {
    dst += n;
    src += n;
    trong_khi (n-- > 0)
      *--dst = *--src;
  }
  trả_về vdst;
}

số_nguyên
memcmp(const void *s1, const void *s2, uint n)
{
  const char *p1 = s1, *p2 = s2;
  trong_khi (n-- > 0) {
    nếu (*p1 != *p2) {
      trả_về *p1 - *p2;
    }
    p1++;
    p2++;
  }
  trả_về 0;
}

void *
memcpy(void *dst, const void *src, uint n)
{
  trả_về memmove(dst, src, n);
}

char *
sbrk(số_nguyên n)
{
  trả_về sys_sbrk(n, SBRK_EAGER);
}

char *
sbrklazy(số_nguyên n)
{
  trả_về sys_sbrk(n, SBRK_LAZY);
}
