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
start(puur_naa_nka argc, var_na **argv)
{
  puur_naa_nka r;
  extern puur_naa_nka main(puur_naa_nka argc, var_na **argv);
  r = main(argc, argv);
  exit(r);
}

var_na *
strcpy(var_na *s, const var_na *t)
{
  var_na *os;

  os = s;
  ᤀᤠ᤺ ((*s++ = *t++) != 0)
    ;
  ᤀᤡᤴ os;
}

puur_naa_nka
strcmp(const var_na *p, const var_na *q)
{
  ᤀᤠ᤺ (*p && *p == *q)
    p++, q++;
  ᤀᤡᤴ (uchar)*p - (uchar)*q;
}

uint
strlen(const var_na *s)
{
  puur_naa_nka n;

  for (n = 0; s[n]; n++)
    ;
  ᤀᤡᤴ n;
}

void *
memset(void *dst, puur_naa_nka c, uint n)
{
  var_na *cdst = (var_na *)dst;
  puur_naa_nka i;
  for (i = 0; i < n; i++) {
    cdst[i] = c;
  }
  ᤀᤡᤴ dst;
}

var_na *
strchr(const var_na *s, var_na c)
{
  for (; *s; s++)
    ᤀᤡ᤺ (*s == c)
      ᤀᤡᤴ (var_na *)s;
  ᤀᤡᤴ 0;
}

var_na *
gets(var_na *buf, puur_naa_nka max)
{
  puur_naa_nka i, cc;
  var_na c;

  for (i = 0; i + 1 < max;) {
    cc = read(0, &c, 1);
    ᤀᤡ᤺ (cc < 1)
      bha_nga;
    buf[i++] = c;
    ᤀᤡ᤺ (c == '\n' || c == '\r')
      bha_nga;
  }
  buf[i] = '\0';
  ᤀᤡᤴ buf;
}

puur_naa_nka
stat(const var_na *n, sa_nracanaa stat *st)
{
  puur_naa_nka fd;
  puur_naa_nka r;

  fd = open(n, O_RDONLY);
  ᤀᤡ᤺ (fd < 0)
    ᤀᤡᤴ -1;
  r = fstat(fd, st);
  close(fd);
  ᤀᤡᤴ r;
}

puur_naa_nka
atoi(const var_na *s)
{
  puur_naa_nka n;

  n = 0;
  ᤀᤠ᤺ ('0' <= *s && *s <= '9')
    n = n * 10 + *s++ - '0';
  ᤀᤡᤴ n;
}

void *
memmove(void *vdst, const void *vsrc, puur_naa_nka n)
{
  var_na *dst;
  const var_na *src;

  dst = vdst;
  src = vsrc;
  ᤀᤡ᤺ (src > dst) {
    ᤀᤠ᤺ (n-- > 0)
      *dst++ = *src++;
  } ᤀᤡ᤺ᤆᤡ {
    dst += n;
    src += n;
    ᤀᤠ᤺ (n-- > 0)
      *--dst = *--src;
  }
  ᤀᤡᤴ vdst;
}

puur_naa_nka
memcmp(const void *s1, const void *s2, uint n)
{
  const var_na *p1 = s1, *p2 = s2;
  ᤀᤠ᤺ (n-- > 0) {
    ᤀᤡ᤺ (*p1 != *p2) {
      ᤀᤡᤴ *p1 - *p2;
    }
    p1++;
    p2++;
  }
  ᤀᤡᤴ 0;
}

void *
memcpy(void *dst, const void *src, uint n)
{
  ᤀᤡᤴ memmove(dst, src, n);
}

var_na *
sbrk(puur_naa_nka n)
{
  ᤀᤡᤴ sys_sbrk(n, SBRK_EAGER);
}

var_na *
sbrklazy(puur_naa_nka n)
{
  ᤀᤡᤴ sys_sbrk(n, SBRK_LAZY);
}
