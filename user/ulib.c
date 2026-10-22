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
start(ཧྲིལ argc, var_na **argv)
{
  ཧྲིལ r;
  extern ཧྲིལ main(ཧྲིལ argc, var_na **argv);
  r = main(argc, argv);
  exit(r);
}

var_na *
strcpy(var_na *s, const var_na *t)
{
  var_na *os;

  os = s;
  སྐབས ((*s++ = *t++) != 0)
    ;
  ལོག os;
}

ཧྲིལ
strcmp(const var_na *p, const var_na *q)
{
  སྐབས (*p && *p == *q)
    p++, q++;
  ལོག (uchar)*p - (uchar)*q;
}

uint
strlen(const var_na *s)
{
  ཧྲིལ n;

  དོན་ལུ (n = 0; s[n]; n++)
    ;
  ལོག n;
}

void *
memset(void *dst, ཧྲིལ c, uint n)
{
  var_na *cdst = (var_na *)dst;
  ཧྲིལ i;
  དོན་ལུ (i = 0; i < n; i++) {
    cdst[i] = c;
  }
  ལོག dst;
}

var_na *
strchr(const var_na *s, var_na c)
{
  དོན་ལུ (; *s; s++)
    གལ་སྲིད (*s == c)
      ལོག (var_na *)s;
  ལོག 0;
}

var_na *
gets(var_na *buf, ཧྲིལ max)
{
  ཧྲིལ i, cc;
  var_na c;

  དོན་ལུ (i = 0; i + 1 < max;) {
    cc = read(0, &c, 1);
    གལ་སྲིད (cc < 1)
      bha_nga;
    buf[i++] = c;
    གལ་སྲིད (c == '\n' || c == '\r')
      bha_nga;
  }
  buf[i] = '\0';
  ལོག buf;
}

ཧྲིལ
stat(const var_na *n, sa_nracanaa stat *st)
{
  ཧྲིལ fd;
  ཧྲིལ r;

  fd = open(n, O_RDONLY);
  གལ་སྲིད (fd < 0)
    ལོག -1;
  r = fstat(fd, st);
  close(fd);
  ལོག r;
}

ཧྲིལ
atoi(const var_na *s)
{
  ཧྲིལ n;

  n = 0;
  སྐབས ('0' <= *s && *s <= '9')
    n = n * 10 + *s++ - '0';
  ལོག n;
}

void *
memmove(void *vdst, const void *vsrc, ཧྲིལ n)
{
  var_na *dst;
  const var_na *src;

  dst = vdst;
  src = vsrc;
  གལ་སྲིད (src > dst) {
    སྐབས (n-- > 0)
      *dst++ = *src++;
  } དེ་མིན {
    dst += n;
    src += n;
    སྐབས (n-- > 0)
      *--dst = *--src;
  }
  ལོག vdst;
}

ཧྲིལ
memcmp(const void *s1, const void *s2, uint n)
{
  const var_na *p1 = s1, *p2 = s2;
  སྐབས (n-- > 0) {
    གལ་སྲིད (*p1 != *p2) {
      ལོག *p1 - *p2;
    }
    p1++;
    p2++;
  }
  ལོག 0;
}

void *
memcpy(void *dst, const void *src, uint n)
{
  ལོག memmove(dst, src, n);
}

var_na *
sbrk(ཧྲིལ n)
{
  ལོག sys_sbrk(n, SBRK_EAGER);
}

var_na *
sbrklazy(ཧྲིལ n)
{
  ལོག sys_sbrk(n, SBRK_LAZY);
}
