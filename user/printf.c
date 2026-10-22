#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#include <stdarg.h>

static char digits[] = "0123456789ABCDEF";

static void
putc(정수 fd, char c)
{
  write(fd, &c, 1);
}

static void
printint(정수 fd, long long xx, 정수 base, 정수 sgn)
{
  char buf[20];
  정수 i, neg;
  unsigned long long x;

  neg = 0;
  만약 (sgn && xx < 0) {
    neg = 1;
    x = -xx;
  } 아니면 {
    x = xx;
  }

  i = 0;
  do {
    buf[i++] = digits[x % base];
  } 동안 ((x /= base) != 0);
  만약 (neg)
    buf[i++] = '-';

  동안 (--i >= 0)
    putc(fd, buf[i]);
}

static void
printptr(정수 fd, uint64 x)
{
  정수 i;
  putc(fd, '0');
  putc(fd, 'x');
  위해 (i = 0; i < (sizeof(uint64) * 2); i++, x <<= 4)
    putc(fd, digits[x >> (sizeof(uint64) * 8 - 4)]);
}

// Print to the given fd. Only understands %d, %x, %p, %c, %s.
void
vprintf(정수 fd, const char *fmt, va_list ap)
{
  char *s;
  정수 c0, c1, c2, i, state;

  state = 0;
  위해 (i = 0; fmt[i]; i++) {
    c0 = fmt[i] & 0xff;
    만약 (state == 0) {
      만약 (c0 == '%') {
        state = '%';
      } 아니면 {
        putc(fd, c0);
      }
    } 아니면 만약 (state == '%') {
      c1 = c2 = 0;
      만약 (c0)
        c1 = fmt[i + 1] & 0xff;
      만약 (c1)
        c2 = fmt[i + 2] & 0xff;
      만약 (c0 == 'd') {
        printint(fd, va_arg(ap, 정수), 10, 1);
      } 아니면 만약 (c0 == 'l' && c1 == 'd') {
        printint(fd, va_arg(ap, uint64), 10, 1);
        i += 1;
      } 아니면 만약 (c0 == 'l' && c1 == 'l' && c2 == 'd') {
        printint(fd, va_arg(ap, uint64), 10, 1);
        i += 2;
      } 아니면 만약 (c0 == 'u') {
        printint(fd, va_arg(ap, uint32), 10, 0);
      } 아니면 만약 (c0 == 'l' && c1 == 'u') {
        printint(fd, va_arg(ap, uint64), 10, 0);
        i += 1;
      } 아니면 만약 (c0 == 'l' && c1 == 'l' && c2 == 'u') {
        printint(fd, va_arg(ap, uint64), 10, 0);
        i += 2;
      } 아니면 만약 (c0 == 'x') {
        printint(fd, va_arg(ap, uint32), 16, 0);
      } 아니면 만약 (c0 == 'l' && c1 == 'x') {
        printint(fd, va_arg(ap, uint64), 16, 0);
        i += 1;
      } 아니면 만약 (c0 == 'l' && c1 == 'l' && c2 == 'x') {
        printint(fd, va_arg(ap, uint64), 16, 0);
        i += 2;
      } 아니면 만약 (c0 == 'p') {
        printptr(fd, va_arg(ap, uint64));
      } 아니면 만약 (c0 == 'c') {
        putc(fd, va_arg(ap, uint32));
      } 아니면 만약 (c0 == 's') {
        만약 ((s = va_arg(ap, char *)) == 0)
          s = "(null)";
        위해 (; *s; s++)
          putc(fd, *s);
      } 아니면 만약 (c0 == '%') {
        putc(fd, '%');
      } 아니면 {
        // Unknown % sequence.  Print it to draw attention.
        putc(fd, '%');
        putc(fd, c0);
      }

      state = 0;
    }
  }
}

void
fprintf(정수 fd, const char *fmt, ...)
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
