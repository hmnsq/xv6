#અન્તર્ભાવ "kernel/types.h"
#અન્તર્ભાવ "kernel/stat.h"
#અન્તર્ભાવ "kernel/fcntl.h"
#અન્તર્ભાવ "kernel/riscv.h"
#અન્તર્ભાવ "kernel/vm.h"
#અન્તર્ભાવ "user/user.h"

//
// wrapper so that it's OK if main() does not call exit().
//
શૂન્ય
start(પૂર્ણાંક argc, અક્ષર **argv)
{
  પૂર્ણાંક r;
  બાહ્ય પૂર્ણાંક મુખ્ય(પૂર્ણાંક argc, અક્ષર **argv);
  r = મુખ્ય(argc, argv);
  exit(r);
}

અક્ષર *
strcpy(અક્ષર *s, અચલ અક્ષર *t)
{
  અક્ષર *os;

  os = s;
  જ્યારે ((*s++ = *t++) != 0)
    ;
  પાછા os;
}

પૂર્ણાંક
strcmp(અચલ અક્ષર *p, અચલ અક્ષર *q)
{
  જ્યારે (*p && *p == *q)
    p++, q++;
  પાછા (uchar)*p - (uchar)*q;
}

uint
strlen(અચલ અક્ષર *s)
{
  પૂર્ણાંક n;

  માટે (n = 0; s[n]; n++)
    ;
  પાછા n;
}

શૂન્ય *
memset(શૂન્ય *dst, પૂર્ણાંક c, uint n)
{
  અક્ષર *cdst = (અક્ષર *)dst;
  પૂર્ણાંક i;
  માટે (i = 0; i < n; i++) {
    cdst[i] = c;
  }
  પાછા dst;
}

અક્ષર *
strchr(અચલ અક્ષર *s, અક્ષર c)
{
  માટે (; *s; s++)
    જો (*s == c)
      પાછા (અક્ષર *)s;
  પાછા 0;
}

અક્ષર *
gets(અક્ષર *buf, પૂર્ણાંક max)
{
  પૂર્ણાંક i, cc;
  અક્ષર c;

  માટે (i = 0; i + 1 < max;) {
    cc = પઠ(0, &c, 1);
    જો (cc < 1)
      તોડો;
    buf[i++] = c;
    જો (c == '\n' || c == '\r')
      તોડો;
  }
  buf[i] = '\0';
  પાછા buf;
}

પૂર્ણાંક
stat(અચલ અક્ષર *n, સંરચના stat *st)
{
  પૂર્ણાંક fd;
  પૂર્ણાંક r;

  fd = open(n, O_RDONLY);
  જો (fd < 0)
    પાછા -1;
  r = fstat(fd, st);
  close(fd);
  પાછા r;
}

પૂર્ણાંક
atoi(અચલ અક્ષર *s)
{
  પૂર્ણાંક n;

  n = 0;
  જ્યારે ('0' <= *s && *s <= '9')
    n = n * 10 + *s++ - '0';
  પાછા n;
}

શૂન્ય *
memmove(શૂન્ય *vdst, અચલ શૂન્ય *vsrc, પૂર્ણાંક n)
{
  અક્ષર *dst;
  અચલ અક્ષર *src;

  dst = vdst;
  src = vsrc;
  જો (src > dst) {
    જ્યારે (n-- > 0)
      *dst++ = *src++;
  } નહીંતર {
    dst += n;
    src += n;
    જ્યારે (n-- > 0)
      *--dst = *--src;
  }
  પાછા vdst;
}

પૂર્ણાંક
memcmp(અચલ શૂન્ય *s1, અચલ શૂન્ય *s2, uint n)
{
  અચલ અક્ષર *p1 = s1, *p2 = s2;
  જ્યારે (n-- > 0) {
    જો (*p1 != *p2) {
      પાછા *p1 - *p2;
    }
    p1++;
    p2++;
  }
  પાછા 0;
}

શૂન્ય *
memcpy(શૂન્ય *dst, અચલ શૂન્ય *src, uint n)
{
  પાછા memmove(dst, src, n);
}

અક્ષર *
sbrk(પૂર્ણાંક n)
{
  પાછા sys_sbrk(n, SBRK_EAGER);
}

અક્ષર *
sbrklazy(પૂર્ણાંક n)
{
  પાછા sys_sbrk(n, SBRK_LAZY);
}
