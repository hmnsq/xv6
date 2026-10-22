#அந்தர்பாவ "kernel/types.h"
#அந்தர்பாவ "kernel/stat.h"
#அந்தர்பாவ "kernel/fcntl.h"
#அந்தர்பாவ "kernel/riscv.h"
#அந்தர்பாவ "kernel/vm.h"
#அந்தர்பாவ "user/user.h"

//
// wrapper so that it's OK if main() does not call exit().
//
வெறுமை
start(முழு argc, எழுத்து **argv)
{
  முழு r;
  வெளி முழு முதன்மை(முழு argc, எழுத்து **argv);
  r = முதன்மை(argc, argv);
  exit(r);
}

எழுத்து *
strcpy(எழுத்து *s, மாறிலி எழுத்து *t)
{
  எழுத்து *os;

  os = s;
  போது ((*s++ = *t++) != 0)
    ;
  திரும்பு os;
}

முழு
strcmp(மாறிலி எழுத்து *p, மாறிலி எழுத்து *q)
{
  போது (*p && *p == *q)
    p++, q++;
  திரும்பு (uchar)*p - (uchar)*q;
}

uint
strlen(மாறிலி எழுத்து *s)
{
  முழு n;

  க்கு (n = 0; s[n]; n++)
    ;
  திரும்பு n;
}

வெறுமை *
memset(வெறுமை *dst, முழு c, uint n)
{
  எழுத்து *cdst = (எழுத்து *)dst;
  முழு i;
  க்கு (i = 0; i < n; i++) {
    cdst[i] = c;
  }
  திரும்பு dst;
}

எழுத்து *
strchr(மாறிலி எழுத்து *s, எழுத்து c)
{
  க்கு (; *s; s++)
    எனில் (*s == c)
      திரும்பு (எழுத்து *)s;
  திரும்பு 0;
}

எழுத்து *
gets(எழுத்து *buf, முழு max)
{
  முழு i, cc;
  எழுத்து c;

  க்கு (i = 0; i + 1 < max;) {
    cc = பட(0, &c, 1);
    எனில் (cc < 1)
      நிறுத்து;
    buf[i++] = c;
    எனில் (c == '\n' || c == '\r')
      நிறுத்து;
  }
  buf[i] = '\0';
  திரும்பு buf;
}

முழு
stat(மாறிலி எழுத்து *n, கட்டமைப்பு stat *st)
{
  முழு fd;
  முழு r;

  fd = open(n, O_RDONLY);
  எனில் (fd < 0)
    திரும்பு -1;
  r = fstat(fd, st);
  close(fd);
  திரும்பு r;
}

முழு
atoi(மாறிலி எழுத்து *s)
{
  முழு n;

  n = 0;
  போது ('0' <= *s && *s <= '9')
    n = n * 10 + *s++ - '0';
  திரும்பு n;
}

வெறுமை *
memmove(வெறுமை *vdst, மாறிலி வெறுமை *vsrc, முழு n)
{
  எழுத்து *dst;
  மாறிலி எழுத்து *src;

  dst = vdst;
  src = vsrc;
  எனில் (src > dst) {
    போது (n-- > 0)
      *dst++ = *src++;
  } இல்லையெனில் {
    dst += n;
    src += n;
    போது (n-- > 0)
      *--dst = *--src;
  }
  திரும்பு vdst;
}

முழு
memcmp(மாறிலி வெறுமை *s1, மாறிலி வெறுமை *s2, uint n)
{
  மாறிலி எழுத்து *p1 = s1, *p2 = s2;
  போது (n-- > 0) {
    எனில் (*p1 != *p2) {
      திரும்பு *p1 - *p2;
    }
    p1++;
    p2++;
  }
  திரும்பு 0;
}

வெறுமை *
memcpy(வெறுமை *dst, மாறிலி வெறுமை *src, uint n)
{
  திரும்பு memmove(dst, src, n);
}

எழுத்து *
sbrk(முழு n)
{
  திரும்பு sys_sbrk(n, SBRK_EAGER);
}

எழுத்து *
sbrklazy(முழு n)
{
  திரும்பு sys_sbrk(n, SBRK_LAZY);
}
