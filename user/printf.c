#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#include <stdarg.h>

static char digits[] = "0123456789ABCDEF";

static void
putc(số_nguyên fd, char c)
{
  write(fd, &c, 1);
}

static void
printint(số_nguyên fd, long long xx, số_nguyên base, số_nguyên sgn)
{
  char buf[20];
  số_nguyên i, neg;
  unsigned long long x;

  neg = 0;
  nếu (sgn && xx < 0) {
    neg = 1;
    x = -xx;
  } không_thì {
    x = xx;
  }

  i = 0;
  do {
    buf[i++] = digits[x % base];
  } trong_khi ((x /= base) != 0);
  nếu (neg)
    buf[i++] = '-';

  trong_khi (--i >= 0)
    putc(fd, buf[i]);
}

static void
printptr(số_nguyên fd, uint64 x)
{
  số_nguyên i;
  putc(fd, '0');
  putc(fd, 'x');
  cho (i = 0; i < (sizeof(uint64) * 2); i++, x <<= 4)
    putc(fd, digits[x >> (sizeof(uint64) * 8 - 4)]);
}

// Print to the given fd. Only understands %d, %x, %p, %c, %s.
void
vprintf(số_nguyên fd, const char *fmt, va_list ap)
{
  char *s;
  số_nguyên c0, c1, c2, i, state;

  state = 0;
  cho (i = 0; fmt[i]; i++) {
    c0 = fmt[i] & 0xff;
    nếu (state == 0) {
      nếu (c0 == '%') {
        state = '%';
      } không_thì {
        putc(fd, c0);
      }
    } không_thì nếu (state == '%') {
      c1 = c2 = 0;
      nếu (c0)
        c1 = fmt[i + 1] & 0xff;
      nếu (c1)
        c2 = fmt[i + 2] & 0xff;
      nếu (c0 == 'd') {
        printint(fd, va_arg(ap, số_nguyên), 10, 1);
      } không_thì nếu (c0 == 'l' && c1 == 'd') {
        printint(fd, va_arg(ap, uint64), 10, 1);
        i += 1;
      } không_thì nếu (c0 == 'l' && c1 == 'l' && c2 == 'd') {
        printint(fd, va_arg(ap, uint64), 10, 1);
        i += 2;
      } không_thì nếu (c0 == 'u') {
        printint(fd, va_arg(ap, uint32), 10, 0);
      } không_thì nếu (c0 == 'l' && c1 == 'u') {
        printint(fd, va_arg(ap, uint64), 10, 0);
        i += 1;
      } không_thì nếu (c0 == 'l' && c1 == 'l' && c2 == 'u') {
        printint(fd, va_arg(ap, uint64), 10, 0);
        i += 2;
      } không_thì nếu (c0 == 'x') {
        printint(fd, va_arg(ap, uint32), 16, 0);
      } không_thì nếu (c0 == 'l' && c1 == 'x') {
        printint(fd, va_arg(ap, uint64), 16, 0);
        i += 1;
      } không_thì nếu (c0 == 'l' && c1 == 'l' && c2 == 'x') {
        printint(fd, va_arg(ap, uint64), 16, 0);
        i += 2;
      } không_thì nếu (c0 == 'p') {
        printptr(fd, va_arg(ap, uint64));
      } không_thì nếu (c0 == 'c') {
        putc(fd, va_arg(ap, uint32));
      } không_thì nếu (c0 == 's') {
        nếu ((s = va_arg(ap, char *)) == 0)
          s = "(null)";
        cho (; *s; s++)
          putc(fd, *s);
      } không_thì nếu (c0 == '%') {
        putc(fd, '%');
      } không_thì {
        // Unknown % sequence.  Print it to draw attention.
        putc(fd, '%');
        putc(fd, c0);
      }

      state = 0;
    }
  }
}

void
fprintf(số_nguyên fd, const char *fmt, ...)
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
