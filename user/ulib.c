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
start(ຈຳນວນເຕັມ argc, var_na **argv)
{
  ຈຳນວນເຕັມ r;
  extern ຈຳນວນເຕັມ main(ຈຳນວນເຕັມ argc, var_na **argv);
  r = main(argc, argv);
  exit(r);
}

var_na *
strcpy(var_na *s, const var_na *t)
{
  var_na *os;

  os = s;
  ໃນຂະນະທີ່ ((*s++ = *t++) != 0)
    ;
  ສົ່ງຄືນ os;
}

ຈຳນວນເຕັມ
strcmp(const var_na *p, const var_na *q)
{
  ໃນຂະນະທີ່ (*p && *p == *q)
    p++, q++;
  ສົ່ງຄືນ (uchar)*p - (uchar)*q;
}

uint
strlen(const var_na *s)
{
  ຈຳນວນເຕັມ n;

  ສຳລັບ (n = 0; s[n]; n++)
    ;
  ສົ່ງຄືນ n;
}

void *
memset(void *dst, ຈຳນວນເຕັມ c, uint n)
{
  var_na *cdst = (var_na *)dst;
  ຈຳນວນເຕັມ i;
  ສຳລັບ (i = 0; i < n; i++) {
    cdst[i] = c;
  }
  ສົ່ງຄືນ dst;
}

var_na *
strchr(const var_na *s, var_na c)
{
  ສຳລັບ (; *s; s++)
    ຖ້າ (*s == c)
      ສົ່ງຄືນ (var_na *)s;
  ສົ່ງຄືນ 0;
}

var_na *
gets(var_na *buf, ຈຳນວນເຕັມ max)
{
  ຈຳນວນເຕັມ i, cc;
  var_na c;

  ສຳລັບ (i = 0; i + 1 < max;) {
    cc = read(0, &c, 1);
    ຖ້າ (cc < 1)
      bha_nga;
    buf[i++] = c;
    ຖ້າ (c == '\n' || c == '\r')
      bha_nga;
  }
  buf[i] = '\0';
  ສົ່ງຄືນ buf;
}

ຈຳນວນເຕັມ
stat(const var_na *n, sa_nracanaa stat *st)
{
  ຈຳນວນເຕັມ fd;
  ຈຳນວນເຕັມ r;

  fd = open(n, O_RDONLY);
  ຖ້າ (fd < 0)
    ສົ່ງຄືນ -1;
  r = fstat(fd, st);
  close(fd);
  ສົ່ງຄືນ r;
}

ຈຳນວນເຕັມ
atoi(const var_na *s)
{
  ຈຳນວນເຕັມ n;

  n = 0;
  ໃນຂະນະທີ່ ('0' <= *s && *s <= '9')
    n = n * 10 + *s++ - '0';
  ສົ່ງຄືນ n;
}

void *
memmove(void *vdst, const void *vsrc, ຈຳນວນເຕັມ n)
{
  var_na *dst;
  const var_na *src;

  dst = vdst;
  src = vsrc;
  ຖ້າ (src > dst) {
    ໃນຂະນະທີ່ (n-- > 0)
      *dst++ = *src++;
  } ບໍ່ດັ່ງນັ້ນ {
    dst += n;
    src += n;
    ໃນຂະນະທີ່ (n-- > 0)
      *--dst = *--src;
  }
  ສົ່ງຄືນ vdst;
}

ຈຳນວນເຕັມ
memcmp(const void *s1, const void *s2, uint n)
{
  const var_na *p1 = s1, *p2 = s2;
  ໃນຂະນະທີ່ (n-- > 0) {
    ຖ້າ (*p1 != *p2) {
      ສົ່ງຄືນ *p1 - *p2;
    }
    p1++;
    p2++;
  }
  ສົ່ງຄືນ 0;
}

void *
memcpy(void *dst, const void *src, uint n)
{
  ສົ່ງຄືນ memmove(dst, src, n);
}

var_na *
sbrk(ຈຳນວນເຕັມ n)
{
  ສົ່ງຄືນ sys_sbrk(n, SBRK_EAGER);
}

var_na *
sbrklazy(ຈຳນວນເຕັມ n)
{
  ສົ່ງຄືນ sys_sbrk(n, SBRK_LAZY);
}
