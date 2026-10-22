#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#include <stdarg.h>

static char digits[] = "0123456789ABCDEF";

static void
putc(整数 fd, char c)
{
  write(fd, &c, 1);
}

static void
printint(整数 fd, long long xx, 整数 base, 整数 sgn)
{
  char buf[20];
  整数 i, neg;
  unsigned long long x;

  neg = 0;
  如果 (sgn && xx < 0) {
    neg = 1;
    x = -xx;
  } 否则 {
    x = xx;
  }

  i = 0;
  do {
    buf[i++] = digits[x % base];
  } 当 ((x /= base) != 0);
  如果 (neg)
    buf[i++] = '-';

  当 (--i >= 0)
    putc(fd, buf[i]);
}

static void
printptr(整数 fd, uint64 x)
{
  整数 i;
  putc(fd, '0');
  putc(fd, 'x');
  为 (i = 0; i < (sizeof(uint64) * 2); i++, x <<= 4)
    putc(fd, digits[x >> (sizeof(uint64) * 8 - 4)]);
}

// Print to the given fd. Only understands %d, %x, %p, %c, %s.
void
vprintf(整数 fd, const char *fmt, va_list ap)
{
  char *s;
  整数 c0, c1, c2, i, state;

  state = 0;
  为 (i = 0; fmt[i]; i++) {
    c0 = fmt[i] & 0xff;
    如果 (state == 0) {
      如果 (c0 == '%') {
        state = '%';
      } 否则 {
        putc(fd, c0);
      }
    } 否则 如果 (state == '%') {
      c1 = c2 = 0;
      如果 (c0)
        c1 = fmt[i + 1] & 0xff;
      如果 (c1)
        c2 = fmt[i + 2] & 0xff;
      如果 (c0 == 'd') {
        printint(fd, va_arg(ap, 整数), 10, 1);
      } 否则 如果 (c0 == 'l' && c1 == 'd') {
        printint(fd, va_arg(ap, uint64), 10, 1);
        i += 1;
      } 否则 如果 (c0 == 'l' && c1 == 'l' && c2 == 'd') {
        printint(fd, va_arg(ap, uint64), 10, 1);
        i += 2;
      } 否则 如果 (c0 == 'u') {
        printint(fd, va_arg(ap, uint32), 10, 0);
      } 否则 如果 (c0 == 'l' && c1 == 'u') {
        printint(fd, va_arg(ap, uint64), 10, 0);
        i += 1;
      } 否则 如果 (c0 == 'l' && c1 == 'l' && c2 == 'u') {
        printint(fd, va_arg(ap, uint64), 10, 0);
        i += 2;
      } 否则 如果 (c0 == 'x') {
        printint(fd, va_arg(ap, uint32), 16, 0);
      } 否则 如果 (c0 == 'l' && c1 == 'x') {
        printint(fd, va_arg(ap, uint64), 16, 0);
        i += 1;
      } 否则 如果 (c0 == 'l' && c1 == 'l' && c2 == 'x') {
        printint(fd, va_arg(ap, uint64), 16, 0);
        i += 2;
      } 否则 如果 (c0 == 'p') {
        printptr(fd, va_arg(ap, uint64));
      } 否则 如果 (c0 == 'c') {
        putc(fd, va_arg(ap, uint32));
      } 否则 如果 (c0 == 's') {
        如果 ((s = va_arg(ap, char *)) == 0)
          s = "(null)";
        为 (; *s; s++)
          putc(fd, *s);
      } 否则 如果 (c0 == '%') {
        putc(fd, '%');
      } 否则 {
        // Unknown % sequence.  Print it to draw attention.
        putc(fd, '%');
        putc(fd, c0);
      }

      state = 0;
    }
  }
}

void
fprintf(整数 fd, const char *fmt, ...)
{
  va_list ap;

  va_start(ap, fmt);
  vprintf(fd, fmt, ap);
}

void
printf(const char *fmt, ...)
{
  va_list ap;

  va_start(ap, fmt);
  vprintf(1, fmt, ap);
}
