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
start(පූර්ණ_සංඛ්‍යා argc, var_na **argv)
{
  පූර්ණ_සංඛ්‍යා r;
  extern පූර්ණ_සංඛ්‍යා main(පූර්ණ_සංඛ්‍යා argc, var_na **argv);
  r = main(argc, argv);
  exit(r);
}

var_na *
strcpy(var_na *s, const var_na *t)
{
  var_na *os;

  os = s;
  අතරතුර ((*s++ = *t++) != 0)
    ;
  ආපසු os;
}

පූර්ණ_සංඛ්‍යා
strcmp(const var_na *p, const var_na *q)
{
  අතරතුර (*p && *p == *q)
    p++, q++;
  ආපසු (uchar)*p - (uchar)*q;
}

uint
strlen(const var_na *s)
{
  පූර්ණ_සංඛ්‍යා n;

  සඳහා (n = 0; s[n]; n++)
    ;
  ආපසු n;
}

void *
memset(void *dst, පූර්ණ_සංඛ්‍යා c, uint n)
{
  var_na *cdst = (var_na *)dst;
  පූර්ණ_සංඛ්‍යා i;
  සඳහා (i = 0; i < n; i++) {
    cdst[i] = c;
  }
  ආපසු dst;
}

var_na *
strchr(const var_na *s, var_na c)
{
  සඳහා (; *s; s++)
    නම් (*s == c)
      ආපසු (var_na *)s;
  ආපසු 0;
}

var_na *
gets(var_na *buf, පූර්ණ_සංඛ්‍යා max)
{
  පූර්ණ_සංඛ්‍යා i, cc;
  var_na c;

  සඳහා (i = 0; i + 1 < max;) {
    cc = read(0, &c, 1);
    නම් (cc < 1)
      bha_nga;
    buf[i++] = c;
    නම් (c == '\n' || c == '\r')
      bha_nga;
  }
  buf[i] = '\0';
  ආපසු buf;
}

පූර්ණ_සංඛ්‍යා
stat(const var_na *n, sa_nracanaa stat *st)
{
  පූර්ණ_සංඛ්‍යා fd;
  පූර්ණ_සංඛ්‍යා r;

  fd = open(n, O_RDONLY);
  නම් (fd < 0)
    ආපසු -1;
  r = fstat(fd, st);
  close(fd);
  ආපසු r;
}

පූර්ණ_සංඛ්‍යා
atoi(const var_na *s)
{
  පූර්ණ_සංඛ්‍යා n;

  n = 0;
  අතරතුර ('0' <= *s && *s <= '9')
    n = n * 10 + *s++ - '0';
  ආපසු n;
}

void *
memmove(void *vdst, const void *vsrc, පූර්ණ_සංඛ්‍යා n)
{
  var_na *dst;
  const var_na *src;

  dst = vdst;
  src = vsrc;
  නම් (src > dst) {
    අතරතුර (n-- > 0)
      *dst++ = *src++;
  } නැතහොත් {
    dst += n;
    src += n;
    අතරතුර (n-- > 0)
      *--dst = *--src;
  }
  ආපසු vdst;
}

පූර්ණ_සංඛ්‍යා
memcmp(const void *s1, const void *s2, uint n)
{
  const var_na *p1 = s1, *p2 = s2;
  අතරතුර (n-- > 0) {
    නම් (*p1 != *p2) {
      ආපසු *p1 - *p2;
    }
    p1++;
    p2++;
  }
  ආපසු 0;
}

void *
memcpy(void *dst, const void *src, uint n)
{
  ආපසු memmove(dst, src, n);
}

var_na *
sbrk(පූර්ණ_සංඛ්‍යා n)
{
  ආපසු sys_sbrk(n, SBRK_EAGER);
}

var_na *
sbrklazy(පූර්ණ_සංඛ්‍යා n)
{
  ආපසු sys_sbrk(n, SBRK_LAZY);
}
