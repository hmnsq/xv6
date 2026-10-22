#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#include <stdarg.h>

static var_na digits[] = "0123456789ABCDEF";

static void
putc(ཧྲིལ fd, var_na c)
{
  write(fd, &c, 1);
}

static void
printint(ཧྲིལ fd, long long xx, ཧྲིལ base, ཧྲིལ sgn)
{
  var_na buf[20];
  ཧྲིལ i, neg;
  unsigned long long x;

  neg = 0;
  གལ་ཏེ (sgn && xx < 0) {
    neg = 1;
    x = -xx;
  } དེ་མིན {
    x = xx;
  }

  i = 0;
  do {
    buf[i++] = digits[x % base];
  } སྐབས ((x /= base) != 0);
  གལ་ཏེ (neg)
    buf[i++] = '-';

  སྐབས (--i >= 0)
    putc(fd, buf[i]);
}

static void
printptr(ཧྲིལ fd, uint64 x)
{
  ཧྲིལ i;
  putc(fd, '0');
  putc(fd, 'x');
  ཆེད་དུ (i = 0; i < (parimaa_na(uint64) * 2); i++, x <<= 4)
    putc(fd, digits[x >> (parimaa_na(uint64) * 8 - 4)]);
}

// Print to the given fd. Only understands %d, %x, %p, %c, %s.
void
vprintf(ཧྲིལ fd, const var_na *fmt, va_list ap)
{
  var_na *s;
  ཧྲིལ c0, c1, c2, i, state;

  state = 0;
  ཆེད་དུ (i = 0; fmt[i]; i++) {
    c0 = fmt[i] & 0xff;
    གལ་ཏེ (state == 0) {
      གལ་ཏེ (c0 == '%') {
        state = '%';
      } དེ་མིན {
        putc(fd, c0);
      }
    } དེ་མིན གལ་ཏེ (state == '%') {
      c1 = c2 = 0;
      གལ་ཏེ (c0)
        c1 = fmt[i + 1] & 0xff;
      གལ་ཏེ (c1)
        c2 = fmt[i + 2] & 0xff;
      གལ་ཏེ (c0 == 'd') {
        printint(fd, va_arg(ap, ཧྲིལ), 10, 1);
      } དེ་མིན གལ་ཏེ (c0 == 'l' && c1 == 'd') {
        printint(fd, va_arg(ap, uint64), 10, 1);
        i += 1;
      } དེ་མིན གལ་ཏེ (c0 == 'l' && c1 == 'l' && c2 == 'd') {
        printint(fd, va_arg(ap, uint64), 10, 1);
        i += 2;
      } དེ་མིན གལ་ཏེ (c0 == 'u') {
        printint(fd, va_arg(ap, uint32), 10, 0);
      } དེ་མིན གལ་ཏེ (c0 == 'l' && c1 == 'u') {
        printint(fd, va_arg(ap, uint64), 10, 0);
        i += 1;
      } དེ་མིན གལ་ཏེ (c0 == 'l' && c1 == 'l' && c2 == 'u') {
        printint(fd, va_arg(ap, uint64), 10, 0);
        i += 2;
      } དེ་མིན གལ་ཏེ (c0 == 'x') {
        printint(fd, va_arg(ap, uint32), 16, 0);
      } དེ་མིན གལ་ཏེ (c0 == 'l' && c1 == 'x') {
        printint(fd, va_arg(ap, uint64), 16, 0);
        i += 1;
      } དེ་མིན གལ་ཏེ (c0 == 'l' && c1 == 'l' && c2 == 'x') {
        printint(fd, va_arg(ap, uint64), 16, 0);
        i += 2;
      } དེ་མིན གལ་ཏེ (c0 == 'p') {
        printptr(fd, va_arg(ap, uint64));
      } དེ་མིན གལ་ཏེ (c0 == 'c') {
        putc(fd, va_arg(ap, uint32));
      } དེ་མིན གལ་ཏེ (c0 == 's') {
        གལ་ཏེ ((s = va_arg(ap, var_na *)) == 0)
          s = "(null)";
        ཆེད་དུ (; *s; s++)
          putc(fd, *s);
      } དེ་མིན གལ་ཏེ (c0 == '%') {
        putc(fd, '%');
      } དེ་མིན {
        // Unknown % sequence.  Print it to draw attention.
        putc(fd, '%');
        putc(fd, c0);
      }

      state = 0;
    }
  }
}

void
fprintf(ཧྲིལ fd, const var_na *fmt, ...)
{
  va_list ap;

  va_start(ap, fmt);
  vprintf(fd, fmt, ap);
}

void
printf(const var_na *fmt, ...)
{
  va_list ap;

  va_start(ap, fmt);
  vprintf(1, fmt, ap);
}
