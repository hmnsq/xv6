#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#include <stdarg.h>

static char digits[] = "0123456789ABCDEF";

static void
putc(ціле fd, char c)
{
  write(fd, &c, 1);
}

static void
printint(ціле fd, long long xx, ціле base, ціле sgn)
{
  char buf[20];
  ціле i, neg;
  unsigned long long x;

  neg = 0;
  якщо (sgn && xx < 0) {
    neg = 1;
    x = -xx;
  } інакше {
    x = xx;
  }

  i = 0;
  do {
    buf[i++] = digits[x % base];
  } поки ((x /= base) != 0);
  якщо (neg)
    buf[i++] = '-';

  поки (--i >= 0)
    putc(fd, buf[i]);
}

static void
printptr(ціле fd, uint64 x)
{
  ціле i;
  putc(fd, '0');
  putc(fd, 'x');
  для (i = 0; i < (sizeof(uint64) * 2); i++, x <<= 4)
    putc(fd, digits[x >> (sizeof(uint64) * 8 - 4)]);
}

// Print to the given fd. Only understands %d, %x, %p, %c, %s.
void
vprintf(ціле fd, const char *fmt, va_list ap)
{
  char *s;
  ціле c0, c1, c2, i, state;

  state = 0;
  для (i = 0; fmt[i]; i++) {
    c0 = fmt[i] & 0xff;
    якщо (state == 0) {
      якщо (c0 == '%') {
        state = '%';
      } інакше {
        putc(fd, c0);
      }
    } інакше якщо (state == '%') {
      c1 = c2 = 0;
      якщо (c0)
        c1 = fmt[i + 1] & 0xff;
      якщо (c1)
        c2 = fmt[i + 2] & 0xff;
      якщо (c0 == 'd') {
        printint(fd, va_arg(ap, ціле), 10, 1);
      } інакше якщо (c0 == 'l' && c1 == 'd') {
        printint(fd, va_arg(ap, uint64), 10, 1);
        i += 1;
      } інакше якщо (c0 == 'l' && c1 == 'l' && c2 == 'd') {
        printint(fd, va_arg(ap, uint64), 10, 1);
        i += 2;
      } інакше якщо (c0 == 'u') {
        printint(fd, va_arg(ap, uint32), 10, 0);
      } інакше якщо (c0 == 'l' && c1 == 'u') {
        printint(fd, va_arg(ap, uint64), 10, 0);
        i += 1;
      } інакше якщо (c0 == 'l' && c1 == 'l' && c2 == 'u') {
        printint(fd, va_arg(ap, uint64), 10, 0);
        i += 2;
      } інакше якщо (c0 == 'x') {
        printint(fd, va_arg(ap, uint32), 16, 0);
      } інакше якщо (c0 == 'l' && c1 == 'x') {
        printint(fd, va_arg(ap, uint64), 16, 0);
        i += 1;
      } інакше якщо (c0 == 'l' && c1 == 'l' && c2 == 'x') {
        printint(fd, va_arg(ap, uint64), 16, 0);
        i += 2;
      } інакше якщо (c0 == 'p') {
        printptr(fd, va_arg(ap, uint64));
      } інакше якщо (c0 == 'c') {
        putc(fd, va_arg(ap, uint32));
      } інакше якщо (c0 == 's') {
        якщо ((s = va_arg(ap, char *)) == 0)
          s = "(null)";
        для (; *s; s++)
          putc(fd, *s);
      } інакше якщо (c0 == '%') {
        putc(fd, '%');
      } інакше {
        // Unknown % sequence.  Print it to draw attention.
        putc(fd, '%');
        putc(fd, c0);
      }

      state = 0;
    }
  }
}

void
fprintf(ціле fd, const char *fmt, ...)
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
