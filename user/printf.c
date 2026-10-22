#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#include <stdarg.h>

static char digits[] = "0123456789ABCDEF";

static void
putc(цяло fd, char c)
{
  write(fd, &c, 1);
}

static void
printint(цяло fd, long long xx, цяло base, цяло sgn)
{
  char buf[20];
  цяло i, neg;
  unsigned long long x;

  neg = 0;
  ако (sgn && xx < 0) {
    neg = 1;
    x = -xx;
  } иначе {
    x = xx;
  }

  i = 0;
  do {
    buf[i++] = digits[x % base];
  } докато ((x /= base) != 0);
  ако (neg)
    buf[i++] = '-';

  докато (--i >= 0)
    putc(fd, buf[i]);
}

static void
printptr(цяло fd, uint64 x)
{
  цяло i;
  putc(fd, '0');
  putc(fd, 'x');
  за (i = 0; i < (sizeof(uint64) * 2); i++, x <<= 4)
    putc(fd, digits[x >> (sizeof(uint64) * 8 - 4)]);
}

// Print to the given fd. Only understands %d, %x, %p, %c, %s.
void
vprintf(цяло fd, const char *fmt, va_list ap)
{
  char *s;
  цяло c0, c1, c2, i, state;

  state = 0;
  за (i = 0; fmt[i]; i++) {
    c0 = fmt[i] & 0xff;
    ако (state == 0) {
      ако (c0 == '%') {
        state = '%';
      } иначе {
        putc(fd, c0);
      }
    } иначе ако (state == '%') {
      c1 = c2 = 0;
      ако (c0)
        c1 = fmt[i + 1] & 0xff;
      ако (c1)
        c2 = fmt[i + 2] & 0xff;
      ако (c0 == 'd') {
        printint(fd, va_arg(ap, цяло), 10, 1);
      } иначе ако (c0 == 'l' && c1 == 'd') {
        printint(fd, va_arg(ap, uint64), 10, 1);
        i += 1;
      } иначе ако (c0 == 'l' && c1 == 'l' && c2 == 'd') {
        printint(fd, va_arg(ap, uint64), 10, 1);
        i += 2;
      } иначе ако (c0 == 'u') {
        printint(fd, va_arg(ap, uint32), 10, 0);
      } иначе ако (c0 == 'l' && c1 == 'u') {
        printint(fd, va_arg(ap, uint64), 10, 0);
        i += 1;
      } иначе ако (c0 == 'l' && c1 == 'l' && c2 == 'u') {
        printint(fd, va_arg(ap, uint64), 10, 0);
        i += 2;
      } иначе ако (c0 == 'x') {
        printint(fd, va_arg(ap, uint32), 16, 0);
      } иначе ако (c0 == 'l' && c1 == 'x') {
        printint(fd, va_arg(ap, uint64), 16, 0);
        i += 1;
      } иначе ако (c0 == 'l' && c1 == 'l' && c2 == 'x') {
        printint(fd, va_arg(ap, uint64), 16, 0);
        i += 2;
      } иначе ако (c0 == 'p') {
        printptr(fd, va_arg(ap, uint64));
      } иначе ако (c0 == 'c') {
        putc(fd, va_arg(ap, uint32));
      } иначе ако (c0 == 's') {
        ако ((s = va_arg(ap, char *)) == 0)
          s = "(null)";
        за (; *s; s++)
          putc(fd, *s);
      } иначе ако (c0 == '%') {
        putc(fd, '%');
      } иначе {
        // Unknown % sequence.  Print it to draw attention.
        putc(fd, '%');
        putc(fd, c0);
      }

      state = 0;
    }
  }
}

void
fprintf(цяло fd, const char *fmt, ...)
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
