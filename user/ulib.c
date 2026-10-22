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
  ᱡᱚᱠᱷᱚᱱ ((*s++ = *t++) != 0)
    ;
  ᱨᱩᱣᱟᱹᱲ os;
}

puur_naa_nka
strcmp(const var_na *p, const var_na *q)
{
  ᱡᱚᱠᱷᱚᱱ (*p && *p == *q)
    p++, q++;
  ᱨᱩᱣᱟᱹᱲ (uchar)*p - (uchar)*q;
}

uint
strlen(const var_na *s)
{
  puur_naa_nka n;

  for (n = 0; s[n]; n++)
    ;
  ᱨᱩᱣᱟᱹᱲ n;
}

void *
memset(void *dst, puur_naa_nka c, uint n)
{
  var_na *cdst = (var_na *)dst;
  puur_naa_nka i;
  for (i = 0; i < n; i++) {
    cdst[i] = c;
  }
  ᱨᱩᱣᱟᱹᱲ dst;
}

var_na *
strchr(const var_na *s, var_na c)
{
  for (; *s; s++)
    ᱡᱩᱫᱤ (*s == c)
      ᱨᱩᱣᱟᱹᱲ (var_na *)s;
  ᱨᱩᱣᱟᱹᱲ 0;
}

var_na *
gets(var_na *buf, puur_naa_nka max)
{
  puur_naa_nka i, cc;
  var_na c;

  for (i = 0; i + 1 < max;) {
    cc = read(0, &c, 1);
    ᱡᱩᱫᱤ (cc < 1)
      bha_nga;
    buf[i++] = c;
    ᱡᱩᱫᱤ (c == '\n' || c == '\r')
      bha_nga;
  }
  buf[i] = '\0';
  ᱨᱩᱣᱟᱹᱲ buf;
}

puur_naa_nka
stat(const var_na *n, sa_nracanaa stat *st)
{
  puur_naa_nka fd;
  puur_naa_nka r;

  fd = open(n, O_RDONLY);
  ᱡᱩᱫᱤ (fd < 0)
    ᱨᱩᱣᱟᱹᱲ -1;
  r = fstat(fd, st);
  close(fd);
  ᱨᱩᱣᱟᱹᱲ r;
}

puur_naa_nka
atoi(const var_na *s)
{
  puur_naa_nka n;

  n = 0;
  ᱡᱚᱠᱷᱚᱱ ('0' <= *s && *s <= '9')
    n = n * 10 + *s++ - '0';
  ᱨᱩᱣᱟᱹᱲ n;
}

void *
memmove(void *vdst, const void *vsrc, puur_naa_nka n)
{
  var_na *dst;
  const var_na *src;

  dst = vdst;
  src = vsrc;
  ᱡᱩᱫᱤ (src > dst) {
    ᱡᱚᱠᱷᱚᱱ (n-- > 0)
      *dst++ = *src++;
  } ᱵᱟᱝ {
    dst += n;
    src += n;
    ᱡᱚᱠᱷᱚᱱ (n-- > 0)
      *--dst = *--src;
  }
  ᱨᱩᱣᱟᱹᱲ vdst;
}

puur_naa_nka
memcmp(const void *s1, const void *s2, uint n)
{
  const var_na *p1 = s1, *p2 = s2;
  ᱡᱚᱠᱷᱚᱱ (n-- > 0) {
    ᱡᱩᱫᱤ (*p1 != *p2) {
      ᱨᱩᱣᱟᱹᱲ *p1 - *p2;
    }
    p1++;
    p2++;
  }
  ᱨᱩᱣᱟᱹᱲ 0;
}

void *
memcpy(void *dst, const void *src, uint n)
{
  ᱨᱩᱣᱟᱹᱲ memmove(dst, src, n);
}

var_na *
sbrk(puur_naa_nka n)
{
  ᱨᱩᱣᱟᱹᱲ sys_sbrk(n, SBRK_EAGER);
}

var_na *
sbrklazy(puur_naa_nka n)
{
  ᱨᱩᱣᱟᱹᱲ sys_sbrk(n, SBRK_LAZY);
}
