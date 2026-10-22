#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/types.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/stat.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/fcntl.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/riscv.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/vm.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "user/user.h"

//
// wrapper so that it's OK if main() does not call exit().
//
ਖਾਲੀ
ਸ਼ੁਰੂ(ਪੂਰਨ argc, ਅੱਖਰ **argv)
{
  ਪੂਰਨ r;
  ਬਾਹਰੀ ਪੂਰਨ ਮੁੱਖ(ਪੂਰਨ argc, ਅੱਖਰ **argv);
  r = ਮੁੱਖ(argc, argv);
  ਬਾਹਰ_ਨਿਕਲੋ(r);
}

ਅੱਖਰ *
strcpy(ਅੱਖਰ *s, ਅਚਲ ਅੱਖਰ *t)
{
  ਅੱਖਰ *os;

  os = s;
  ਜਦੋਂ ((*s++ = *t++) != 0)
    ;
  ਵਾਪਸ os;
}

ਪੂਰਨ
strcmp(ਅਚਲ ਅੱਖਰ *p, ਅਚਲ ਅੱਖਰ *q)
{
  ਜਦੋਂ (*p && *p == *q)
    p++, q++;
  ਵਾਪਸ (uchar)*p - (uchar)*q;
}

uint
ਲੜੀ_ਲੰਬਾਈ(ਅਚਲ ਅੱਖਰ *s)
{
  ਪੂਰਨ n;

  ਲਈ (n = 0; s[n]; n++)
    ;
  ਵਾਪਸ n;
}

ਖਾਲੀ *
memset(ਖਾਲੀ *dst, ਪੂਰਨ c, uint n)
{
  ਅੱਖਰ *cdst = (ਅੱਖਰ *)dst;
  ਪੂਰਨ i;
  ਲਈ (i = 0; i < n; i++) {
    cdst[i] = c;
  }
  ਵਾਪਸ dst;
}

ਅੱਖਰ *
strchr(ਅਚਲ ਅੱਖਰ *s, ਅੱਖਰ c)
{
  ਲਈ (; *s; s++)
    ਜੇ (*s == c)
      ਵਾਪਸ (ਅੱਖਰ *)s;
  ਵਾਪਸ 0;
}

ਅੱਖਰ *
gets(ਅੱਖਰ *buf, ਪੂਰਨ max)
{
  ਪੂਰਨ i, cc;
  ਅੱਖਰ c;

  ਲਈ (i = 0; i + 1 < max;) {
    cc = ਪਠ(0, &c, 1);
    ਜੇ (cc < 1)
      ਰੋਕੋ;
    buf[i++] = c;
    ਜੇ (c == '\n' || c == '\r')
      ਰੋਕੋ;
  }
  buf[i] = '\0';
  ਵਾਪਸ buf;
}

ਪੂਰਨ
stat(ਅਚਲ ਅੱਖਰ *n, ਬਣਤਰ stat *st)
{
  ਪੂਰਨ fd;
  ਪੂਰਨ r;

  fd = open(n, O_RDONLY);
  ਜੇ (fd < 0)
    ਵਾਪਸ -1;
  r = fstat(fd, st);
  close(fd);
  ਵਾਪਸ r;
}

ਪੂਰਨ
atoi(ਅਚਲ ਅੱਖਰ *s)
{
  ਪੂਰਨ n;

  n = 0;
  ਜਦੋਂ ('0' <= *s && *s <= '9')
    n = n * 10 + *s++ - '0';
  ਵਾਪਸ n;
}

ਖਾਲੀ *
memmove(ਖਾਲੀ *vdst, ਅਚਲ ਖਾਲੀ *vsrc, ਪੂਰਨ n)
{
  ਅੱਖਰ *dst;
  ਅਚਲ ਅੱਖਰ *src;

  dst = vdst;
  src = vsrc;
  ਜੇ (src > dst) {
    ਜਦੋਂ (n-- > 0)
      *dst++ = *src++;
  } ਨਹੀਂਤਾਂ {
    dst += n;
    src += n;
    ਜਦੋਂ (n-- > 0)
      *--dst = *--src;
  }
  ਵਾਪਸ vdst;
}

ਪੂਰਨ
memcmp(ਅਚਲ ਖਾਲੀ *s1, ਅਚਲ ਖਾਲੀ *s2, uint n)
{
  ਅਚਲ ਅੱਖਰ *p1 = s1, *p2 = s2;
  ਜਦੋਂ (n-- > 0) {
    ਜੇ (*p1 != *p2) {
      ਵਾਪਸ *p1 - *p2;
    }
    p1++;
    p2++;
  }
  ਵਾਪਸ 0;
}

ਖਾਲੀ *
ਸਮ੍ਰਿਤੀ_ਨਕਲ(ਖਾਲੀ *dst, ਅਚਲ ਖਾਲੀ *src, uint n)
{
  ਵਾਪਸ memmove(dst, src, n);
}

ਅੱਖਰ *
sbrk(ਪੂਰਨ n)
{
  ਵਾਪਸ sys_sbrk(n, SBRK_EAGER);
}

ਅੱਖਰ *
sbrklazy(ਪੂਰਨ n)
{
  ਵਾਪਸ sys_sbrk(n, SBRK_LAZY);
}
