#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#include <stdarg.h>

static var_na digits[] = "0123456789ABCDEF";

static void
putc(ຈຳນວນເຕັມ fd, var_na c)
{
  write(fd, &c, 1);
}

static void
printint(ຈຳນວນເຕັມ fd, long long xx, ຈຳນວນເຕັມ base, ຈຳນວນເຕັມ sgn)
{
  var_na buf[20];
  ຈຳນວນເຕັມ i, neg;
  unsigned long long x;

  neg = 0;
  ຖ້າ (sgn && xx < 0) {
    neg = 1;
    x = -xx;
  } ບໍ່ດັ່ງນັ້ນ {
    x = xx;
  }

  i = 0;
  do {
    buf[i++] = digits[x % base];
  } ໃນຂະນະທີ່ ((x /= base) != 0);
  ຖ້າ (neg)
    buf[i++] = '-';

  ໃນຂະນະທີ່ (--i >= 0)
    putc(fd, buf[i]);
}

static void
printptr(ຈຳນວນເຕັມ fd, uint64 x)
{
  ຈຳນວນເຕັມ i;
  putc(fd, '0');
  putc(fd, 'x');
  ສຳລັບ (i = 0; i < (parimaa_na(uint64) * 2); i++, x <<= 4)
    putc(fd, digits[x >> (parimaa_na(uint64) * 8 - 4)]);
}

// Print to the given fd. Only understands %d, %x, %p, %c, %s.
void
vprintf(ຈຳນວນເຕັມ fd, const var_na *fmt, va_list ap)
{
  var_na *s;
  ຈຳນວນເຕັມ c0, c1, c2, i, state;

  state = 0;
  ສຳລັບ (i = 0; fmt[i]; i++) {
    c0 = fmt[i] & 0xff;
    ຖ້າ (state == 0) {
      ຖ້າ (c0 == '%') {
        state = '%';
      } ບໍ່ດັ່ງນັ້ນ {
        putc(fd, c0);
      }
    } ບໍ່ດັ່ງນັ້ນ ຖ້າ (state == '%') {
      c1 = c2 = 0;
      ຖ້າ (c0)
        c1 = fmt[i + 1] & 0xff;
      ຖ້າ (c1)
        c2 = fmt[i + 2] & 0xff;
      ຖ້າ (c0 == 'd') {
        printint(fd, va_arg(ap, ຈຳນວນເຕັມ), 10, 1);
      } ບໍ່ດັ່ງນັ້ນ ຖ້າ (c0 == 'l' && c1 == 'd') {
        printint(fd, va_arg(ap, uint64), 10, 1);
        i += 1;
      } ບໍ່ດັ່ງນັ້ນ ຖ້າ (c0 == 'l' && c1 == 'l' && c2 == 'd') {
        printint(fd, va_arg(ap, uint64), 10, 1);
        i += 2;
      } ບໍ່ດັ່ງນັ້ນ ຖ້າ (c0 == 'u') {
        printint(fd, va_arg(ap, uint32), 10, 0);
      } ບໍ່ດັ່ງນັ້ນ ຖ້າ (c0 == 'l' && c1 == 'u') {
        printint(fd, va_arg(ap, uint64), 10, 0);
        i += 1;
      } ບໍ່ດັ່ງນັ້ນ ຖ້າ (c0 == 'l' && c1 == 'l' && c2 == 'u') {
        printint(fd, va_arg(ap, uint64), 10, 0);
        i += 2;
      } ບໍ່ດັ່ງນັ້ນ ຖ້າ (c0 == 'x') {
        printint(fd, va_arg(ap, uint32), 16, 0);
      } ບໍ່ດັ່ງນັ້ນ ຖ້າ (c0 == 'l' && c1 == 'x') {
        printint(fd, va_arg(ap, uint64), 16, 0);
        i += 1;
      } ບໍ່ດັ່ງນັ້ນ ຖ້າ (c0 == 'l' && c1 == 'l' && c2 == 'x') {
        printint(fd, va_arg(ap, uint64), 16, 0);
        i += 2;
      } ບໍ່ດັ່ງນັ້ນ ຖ້າ (c0 == 'p') {
        printptr(fd, va_arg(ap, uint64));
      } ບໍ່ດັ່ງນັ້ນ ຖ້າ (c0 == 'c') {
        putc(fd, va_arg(ap, uint32));
      } ບໍ່ດັ່ງນັ້ນ ຖ້າ (c0 == 's') {
        ຖ້າ ((s = va_arg(ap, var_na *)) == 0)
          s = "(null)";
        ສຳລັບ (; *s; s++)
          putc(fd, *s);
      } ບໍ່ດັ່ງນັ້ນ ຖ້າ (c0 == '%') {
        putc(fd, '%');
      } ບໍ່ດັ່ງນັ້ນ {
        // Unknown % sequence.  Print it to draw attention.
        putc(fd, '%');
        putc(fd, c0);
      }

      state = 0;
    }
  }
}

void
fprintf(ຈຳນວນເຕັມ fd, const var_na *fmt, ...)
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
