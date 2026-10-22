#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#include <stdarg.h>

static var_na digits[] = "0123456789ABCDEF";

static void
putc(puur_naa_nka fd, var_na c)
{
  write(fd, &c, 1);
}

static void
printint(puur_naa_nka fd, long long xx, puur_naa_nka base, puur_naa_nka sgn)
{
  var_na buf[20];
  puur_naa_nka i, neg;
  unsigned long long x;

  neg = 0;
  ᰀᰧ (sgn && xx < 0) {
    neg = 1;
    x = -xx;
  } ᰀᰧᰀ {
    x = xx;
  }

  i = 0;
  do {
    buf[i++] = digits[x % base];
  } ᰀᰬ ((x /= base) != 0);
  ᰀᰧ (neg)
    buf[i++] = '-';

  ᰀᰬ (--i >= 0)
    putc(fd, buf[i]);
}

static void
printptr(puur_naa_nka fd, uint64 x)
{
  puur_naa_nka i;
  putc(fd, '0');
  putc(fd, 'x');
  for (i = 0; i < (parimaa_na(uint64) * 2); i++, x <<= 4)
    putc(fd, digits[x >> (parimaa_na(uint64) * 8 - 4)]);
}

// Print to the given fd. Only understands %d, %x, %p, %c, %s.
void
vprintf(puur_naa_nka fd, const var_na *fmt, va_list ap)
{
  var_na *s;
  puur_naa_nka c0, c1, c2, i, state;

  state = 0;
  for (i = 0; fmt[i]; i++) {
    c0 = fmt[i] & 0xff;
    ᰀᰧ (state == 0) {
      ᰀᰧ (c0 == '%') {
        state = '%';
      } ᰀᰧᰀ {
        putc(fd, c0);
      }
    } ᰀᰧᰀ ᰀᰧ (state == '%') {
      c1 = c2 = 0;
      ᰀᰧ (c0)
        c1 = fmt[i + 1] & 0xff;
      ᰀᰧ (c1)
        c2 = fmt[i + 2] & 0xff;
      ᰀᰧ (c0 == 'd') {
        printint(fd, va_arg(ap, puur_naa_nka), 10, 1);
      } ᰀᰧᰀ ᰀᰧ (c0 == 'l' && c1 == 'd') {
        printint(fd, va_arg(ap, uint64), 10, 1);
        i += 1;
      } ᰀᰧᰀ ᰀᰧ (c0 == 'l' && c1 == 'l' && c2 == 'd') {
        printint(fd, va_arg(ap, uint64), 10, 1);
        i += 2;
      } ᰀᰧᰀ ᰀᰧ (c0 == 'u') {
        printint(fd, va_arg(ap, uint32), 10, 0);
      } ᰀᰧᰀ ᰀᰧ (c0 == 'l' && c1 == 'u') {
        printint(fd, va_arg(ap, uint64), 10, 0);
        i += 1;
      } ᰀᰧᰀ ᰀᰧ (c0 == 'l' && c1 == 'l' && c2 == 'u') {
        printint(fd, va_arg(ap, uint64), 10, 0);
        i += 2;
      } ᰀᰧᰀ ᰀᰧ (c0 == 'x') {
        printint(fd, va_arg(ap, uint32), 16, 0);
      } ᰀᰧᰀ ᰀᰧ (c0 == 'l' && c1 == 'x') {
        printint(fd, va_arg(ap, uint64), 16, 0);
        i += 1;
      } ᰀᰧᰀ ᰀᰧ (c0 == 'l' && c1 == 'l' && c2 == 'x') {
        printint(fd, va_arg(ap, uint64), 16, 0);
        i += 2;
      } ᰀᰧᰀ ᰀᰧ (c0 == 'p') {
        printptr(fd, va_arg(ap, uint64));
      } ᰀᰧᰀ ᰀᰧ (c0 == 'c') {
        putc(fd, va_arg(ap, uint32));
      } ᰀᰧᰀ ᰀᰧ (c0 == 's') {
        ᰀᰧ ((s = va_arg(ap, var_na *)) == 0)
          s = "(null)";
        for (; *s; s++)
          putc(fd, *s);
      } ᰀᰧᰀ ᰀᰧ (c0 == '%') {
        putc(fd, '%');
      } ᰀᰧᰀ {
        // Unknown % sequence.  Print it to draw attention.
        putc(fd, '%');
        putc(fd, c0);
      }

      state = 0;
    }
  }
}

void
fprintf(puur_naa_nka fd, const var_na *fmt, ...)
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
