#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#include <stdarg.h>

static char digits[] = "0123456789ABCDEF";

static void
putc(целое fd, char c)
{
  write(fd, &c, 1);
}

static void
printint(целое fd, long long xx, целое base, целое sgn)
{
  char buf[20];
  целое i, neg;
  unsigned long long x;

  neg = 0;
  если (sgn && xx < 0) {
    neg = 1;
    x = -xx;
  } иначе {
    x = xx;
  }

  i = 0;
  do {
    buf[i++] = digits[x % base];
  } пока ((x /= base) != 0);
  если (neg)
    buf[i++] = '-';

  пока (--i >= 0)
    putc(fd, buf[i]);
}

static void
printptr(целое fd, uint64 x)
{
  целое i;
  putc(fd, '0');
  putc(fd, 'x');
  для (i = 0; i < (sizeof(uint64) * 2); i++, x <<= 4)
    putc(fd, digits[x >> (sizeof(uint64) * 8 - 4)]);
}

// Print to the given fd. Only understands %d, %x, %p, %c, %s.
void
vprintf(целое fd, const char *fmt, va_list ap)
{
  char *s;
  целое c0, c1, c2, i, state;

  state = 0;
  для (i = 0; fmt[i]; i++) {
    c0 = fmt[i] & 0xff;
    если (state == 0) {
      если (c0 == '%') {
        state = '%';
      } иначе {
        putc(fd, c0);
      }
    } иначе если (state == '%') {
      c1 = c2 = 0;
      если (c0)
        c1 = fmt[i + 1] & 0xff;
      если (c1)
        c2 = fmt[i + 2] & 0xff;
      если (c0 == 'd') {
        printint(fd, va_arg(ap, целое), 10, 1);
      } иначе если (c0 == 'l' && c1 == 'd') {
        printint(fd, va_arg(ap, uint64), 10, 1);
        i += 1;
      } иначе если (c0 == 'l' && c1 == 'l' && c2 == 'd') {
        printint(fd, va_arg(ap, uint64), 10, 1);
        i += 2;
      } иначе если (c0 == 'u') {
        printint(fd, va_arg(ap, uint32), 10, 0);
      } иначе если (c0 == 'l' && c1 == 'u') {
        printint(fd, va_arg(ap, uint64), 10, 0);
        i += 1;
      } иначе если (c0 == 'l' && c1 == 'l' && c2 == 'u') {
        printint(fd, va_arg(ap, uint64), 10, 0);
        i += 2;
      } иначе если (c0 == 'x') {
        printint(fd, va_arg(ap, uint32), 16, 0);
      } иначе если (c0 == 'l' && c1 == 'x') {
        printint(fd, va_arg(ap, uint64), 16, 0);
        i += 1;
      } иначе если (c0 == 'l' && c1 == 'l' && c2 == 'x') {
        printint(fd, va_arg(ap, uint64), 16, 0);
        i += 2;
      } иначе если (c0 == 'p') {
        printptr(fd, va_arg(ap, uint64));
      } иначе если (c0 == 'c') {
        putc(fd, va_arg(ap, uint32));
      } иначе если (c0 == 's') {
        если ((s = va_arg(ap, char *)) == 0)
          s = "(null)";
        для (; *s; s++)
          putc(fd, *s);
      } иначе если (c0 == '%') {
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
fprintf(целое fd, const char *fmt, ...)
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
