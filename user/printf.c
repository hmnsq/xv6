#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#include <stdarg.h>

static var_na digits[] = "0123456789ABCDEF";

static void
putc(ចំនួនគត់ fd, var_na c)
{
  write(fd, &c, 1);
}

static void
printint(ចំនួនគត់ fd, long long xx, ចំនួនគត់ base, ចំនួនគត់ sgn)
{
  var_na buf[20];
  ចំនួនគត់ i, neg;
  unsigned long long x;

  neg = 0;
  ប្រសិនបើ (sgn && xx < 0) {
    neg = 1;
    x = -xx;
  } បើមិនដូច្នេះ {
    x = xx;
  }

  i = 0;
  do {
    buf[i++] = digits[x % base];
  } ខណៈពេល ((x /= base) != 0);
  ប្រសិនបើ (neg)
    buf[i++] = '-';

  ខណៈពេល (--i >= 0)
    putc(fd, buf[i]);
}

static void
printptr(ចំនួនគត់ fd, uint64 x)
{
  ចំនួនគត់ i;
  putc(fd, '0');
  putc(fd, 'x');
  សម្រាប់ (i = 0; i < (parimaa_na(uint64) * 2); i++, x <<= 4)
    putc(fd, digits[x >> (parimaa_na(uint64) * 8 - 4)]);
}

// Print to the given fd. Only understands %d, %x, %p, %c, %s.
void
vprintf(ចំនួនគត់ fd, const var_na *fmt, va_list ap)
{
  var_na *s;
  ចំនួនគត់ c0, c1, c2, i, state;

  state = 0;
  សម្រាប់ (i = 0; fmt[i]; i++) {
    c0 = fmt[i] & 0xff;
    ប្រសិនបើ (state == 0) {
      ប្រសិនបើ (c0 == '%') {
        state = '%';
      } បើមិនដូច្នេះ {
        putc(fd, c0);
      }
    } បើមិនដូច្នេះ ប្រសិនបើ (state == '%') {
      c1 = c2 = 0;
      ប្រសិនបើ (c0)
        c1 = fmt[i + 1] & 0xff;
      ប្រសិនបើ (c1)
        c2 = fmt[i + 2] & 0xff;
      ប្រសិនបើ (c0 == 'd') {
        printint(fd, va_arg(ap, ចំនួនគត់), 10, 1);
      } បើមិនដូច្នេះ ប្រសិនបើ (c0 == 'l' && c1 == 'd') {
        printint(fd, va_arg(ap, uint64), 10, 1);
        i += 1;
      } បើមិនដូច្នេះ ប្រសិនបើ (c0 == 'l' && c1 == 'l' && c2 == 'd') {
        printint(fd, va_arg(ap, uint64), 10, 1);
        i += 2;
      } បើមិនដូច្នេះ ប្រសិនបើ (c0 == 'u') {
        printint(fd, va_arg(ap, uint32), 10, 0);
      } បើមិនដូច្នេះ ប្រសិនបើ (c0 == 'l' && c1 == 'u') {
        printint(fd, va_arg(ap, uint64), 10, 0);
        i += 1;
      } បើមិនដូច្នេះ ប្រសិនបើ (c0 == 'l' && c1 == 'l' && c2 == 'u') {
        printint(fd, va_arg(ap, uint64), 10, 0);
        i += 2;
      } បើមិនដូច្នេះ ប្រសិនបើ (c0 == 'x') {
        printint(fd, va_arg(ap, uint32), 16, 0);
      } បើមិនដូច្នេះ ប្រសិនបើ (c0 == 'l' && c1 == 'x') {
        printint(fd, va_arg(ap, uint64), 16, 0);
        i += 1;
      } បើមិនដូច្នេះ ប្រសិនបើ (c0 == 'l' && c1 == 'l' && c2 == 'x') {
        printint(fd, va_arg(ap, uint64), 16, 0);
        i += 2;
      } បើមិនដូច្នេះ ប្រសិនបើ (c0 == 'p') {
        printptr(fd, va_arg(ap, uint64));
      } បើមិនដូច្នេះ ប្រសិនបើ (c0 == 'c') {
        putc(fd, va_arg(ap, uint32));
      } បើមិនដូច្នេះ ប្រសិនបើ (c0 == 's') {
        ប្រសិនបើ ((s = va_arg(ap, var_na *)) == 0)
          s = "(null)";
        សម្រាប់ (; *s; s++)
          putc(fd, *s);
      } បើមិនដូច្នេះ ប្រសិនបើ (c0 == '%') {
        putc(fd, '%');
      } បើមិនដូច្នេះ {
        // Unknown % sequence.  Print it to draw attention.
        putc(fd, '%');
        putc(fd, c0);
      }

      state = 0;
    }
  }
}

void
fprintf(ចំនួនគត់ fd, const var_na *fmt, ...)
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
