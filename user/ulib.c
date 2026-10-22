#ಅನ್ತರ್ಭಾವ "kernel/types.h"
#ಅನ್ತರ್ಭಾವ "kernel/stat.h"
#ಅನ್ತರ್ಭಾವ "kernel/fcntl.h"
#ಅನ್ತರ್ಭಾವ "kernel/riscv.h"
#ಅನ್ತರ್ಭಾವ "kernel/vm.h"
#ಅನ್ತರ್ಭಾವ "user/user.h"

//
// wrapper so that it's OK if main() does not call exit().
//
ಶೂನ್ಯ
start(ಪೂರ್ಣಾಙ್ಕ argc, ವರ್ಣ **argv)
{
  ಪೂರ್ಣಾಙ್ಕ r;
  ಬಾಹ್ಯ ಪೂರ್ಣಾಙ್ಕ main(ಪೂರ್ಣಾಙ್ಕ argc, ವರ್ಣ **argv);
  r = main(argc, argv);
  exit(r);
}

ವರ್ಣ *
strcpy(ವರ್ಣ *s, ಧ್ರುವ ವರ್ಣ *t)
{
  ವರ್ಣ *os;

  os = s;
  ಯಾವತ್ ((*s++ = *t++) != 0)
    ;
  ಪ್ರತ್ಯಾಗಮ os;
}

ಪೂರ್ಣಾಙ್ಕ
strcmp(ಧ್ರುವ ವರ್ಣ *p, ಧ್ರುವ ವರ್ಣ *q)
{
  ಯಾವತ್ (*p && *p == *q)
    p++, q++;
  ಪ್ರತ್ಯಾಗಮ (uchar)*p - (uchar)*q;
}

uint
strlen(ಧ್ರುವ ವರ್ಣ *s)
{
  ಪೂರ್ಣಾಙ್ಕ n;

  ಪ್ರತಿ (n = 0; s[n]; n++)
    ;
  ಪ್ರತ್ಯಾಗಮ n;
}

ಶೂನ್ಯ *
memset(ಶೂನ್ಯ *dst, ಪೂರ್ಣಾಙ್ಕ c, uint n)
{
  ವರ್ಣ *cdst = (ವರ್ಣ *)dst;
  ಪೂರ್ಣಾಙ್ಕ i;
  ಪ್ರತಿ (i = 0; i < n; i++) {
    cdst[i] = c;
  }
  ಪ್ರತ್ಯಾಗಮ dst;
}

ವರ್ಣ *
strchr(ಧ್ರುವ ವರ್ಣ *s, ವರ್ಣ c)
{
  ಪ್ರತಿ (; *s; s++)
    ಯದಿ (*s == c)
      ಪ್ರತ್ಯಾಗಮ (ವರ್ಣ *)s;
  ಪ್ರತ್ಯಾಗಮ 0;
}

ವರ್ಣ *
gets(ವರ್ಣ *buf, ಪೂರ್ಣಾಙ್ಕ max)
{
  ಪೂರ್ಣಾಙ್ಕ i, cc;
  ವರ್ಣ c;

  ಪ್ರತಿ (i = 0; i + 1 < max;) {
    cc = ಪಠ(0, &c, 1);
    ಯದಿ (cc < 1)
      ಭಙ್ಗ;
    buf[i++] = c;
    ಯದಿ (c == '\n' || c == '\r')
      ಭಙ್ಗ;
  }
  buf[i] = '\0';
  ಪ್ರತ್ಯಾಗಮ buf;
}

ಪೂರ್ಣಾಙ್ಕ
stat(ಧ್ರುವ ವರ್ಣ *n, ಸಂರಚನಾ stat *st)
{
  ಪೂರ್ಣಾಙ್ಕ fd;
  ಪೂರ್ಣಾಙ್ಕ r;

  fd = open(n, O_RDONLY);
  ಯದಿ (fd < 0)
    ಪ್ರತ್ಯಾಗಮ -1;
  r = fstat(fd, st);
  close(fd);
  ಪ್ರತ್ಯಾಗಮ r;
}

ಪೂರ್ಣಾಙ್ಕ
atoi(ಧ್ರುವ ವರ್ಣ *s)
{
  ಪೂರ್ಣಾಙ್ಕ n;

  n = 0;
  ಯಾವತ್ ('0' <= *s && *s <= '9')
    n = n * 10 + *s++ - '0';
  ಪ್ರತ್ಯಾಗಮ n;
}

ಶೂನ್ಯ *
memmove(ಶೂನ್ಯ *vdst, ಧ್ರುವ ಶೂನ್ಯ *vsrc, ಪೂರ್ಣಾಙ್ಕ n)
{
  ವರ್ಣ *dst;
  ಧ್ರುವ ವರ್ಣ *src;

  dst = vdst;
  src = vsrc;
  ಯದಿ (src > dst) {
    ಯಾವತ್ (n-- > 0)
      *dst++ = *src++;
  } ಅನ್ಯಥಾ {
    dst += n;
    src += n;
    ಯಾವತ್ (n-- > 0)
      *--dst = *--src;
  }
  ಪ್ರತ್ಯಾಗಮ vdst;
}

ಪೂರ್ಣಾಙ್ಕ
memcmp(ಧ್ರುವ ಶೂನ್ಯ *s1, ಧ್ರುವ ಶೂನ್ಯ *s2, uint n)
{
  ಧ್ರುವ ವರ್ಣ *p1 = s1, *p2 = s2;
  ಯಾವತ್ (n-- > 0) {
    ಯದಿ (*p1 != *p2) {
      ಪ್ರತ್ಯಾಗಮ *p1 - *p2;
    }
    p1++;
    p2++;
  }
  ಪ್ರತ್ಯಾಗಮ 0;
}

ಶೂನ್ಯ *
memcpy(ಶೂನ್ಯ *dst, ಧ್ರುವ ಶೂನ್ಯ *src, uint n)
{
  ಪ್ರತ್ಯಾಗಮ memmove(dst, src, n);
}

ವರ್ಣ *
sbrk(ಪೂರ್ಣಾಙ್ಕ n)
{
  ಪ್ರತ್ಯಾಗಮ sys_sbrk(n, SBRK_EAGER);
}

ವರ್ಣ *
sbrklazy(ಪೂರ್ಣಾಙ್ಕ n)
{
  ಪ್ರತ್ಯಾಗಮ sys_sbrk(n, SBRK_LAZY);
}
