#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#include <stdarg.h>

static var_na digits[] = "0123456789ABCDEF";

static void
putc(පූර්ණ_සංඛ්‍යා fd, var_na c)
{
  write(fd, &c, 1);
}

static void
printint(පූර්ණ_සංඛ්‍යා fd, long long xx, පූර්ණ_සංඛ්‍යා base, පූර්ණ_සංඛ්‍යා sgn)
{
  var_na buf[20];
  පූර්ණ_සංඛ්‍යා i, neg;
  unsigned long long x;

  neg = 0;
  නම් (sgn && xx < 0) {
    neg = 1;
    x = -xx;
  } නැතහොත් {
    x = xx;
  }

  i = 0;
  do {
    buf[i++] = digits[x % base];
  } අතරතුර ((x /= base) != 0);
  නම් (neg)
    buf[i++] = '-';

  අතරතුර (--i >= 0)
    putc(fd, buf[i]);
}

static void
printptr(පූර්ණ_සංඛ්‍යා fd, uint64 x)
{
  පූර්ණ_සංඛ්‍යා i;
  putc(fd, '0');
  putc(fd, 'x');
  සඳහා (i = 0; i < (parimaa_na(uint64) * 2); i++, x <<= 4)
    putc(fd, digits[x >> (parimaa_na(uint64) * 8 - 4)]);
}

// Print to the given fd. Only understands %d, %x, %p, %c, %s.
void
vprintf(පූර්ණ_සංඛ්‍යා fd, const var_na *fmt, va_list ap)
{
  var_na *s;
  පූර්ණ_සංඛ්‍යා c0, c1, c2, i, state;

  state = 0;
  සඳහා (i = 0; fmt[i]; i++) {
    c0 = fmt[i] & 0xff;
    නම් (state == 0) {
      නම් (c0 == '%') {
        state = '%';
      } නැතහොත් {
        putc(fd, c0);
      }
    } නැතහොත් නම් (state == '%') {
      c1 = c2 = 0;
      නම් (c0)
        c1 = fmt[i + 1] & 0xff;
      නම් (c1)
        c2 = fmt[i + 2] & 0xff;
      නම් (c0 == 'd') {
        printint(fd, va_arg(ap, පූර්ණ_සංඛ්‍යා), 10, 1);
      } නැතහොත් නම් (c0 == 'l' && c1 == 'd') {
        printint(fd, va_arg(ap, uint64), 10, 1);
        i += 1;
      } නැතහොත් නම් (c0 == 'l' && c1 == 'l' && c2 == 'd') {
        printint(fd, va_arg(ap, uint64), 10, 1);
        i += 2;
      } නැතහොත් නම් (c0 == 'u') {
        printint(fd, va_arg(ap, uint32), 10, 0);
      } නැතහොත් නම් (c0 == 'l' && c1 == 'u') {
        printint(fd, va_arg(ap, uint64), 10, 0);
        i += 1;
      } නැතහොත් නම් (c0 == 'l' && c1 == 'l' && c2 == 'u') {
        printint(fd, va_arg(ap, uint64), 10, 0);
        i += 2;
      } නැතහොත් නම් (c0 == 'x') {
        printint(fd, va_arg(ap, uint32), 16, 0);
      } නැතහොත් නම් (c0 == 'l' && c1 == 'x') {
        printint(fd, va_arg(ap, uint64), 16, 0);
        i += 1;
      } නැතහොත් නම් (c0 == 'l' && c1 == 'l' && c2 == 'x') {
        printint(fd, va_arg(ap, uint64), 16, 0);
        i += 2;
      } නැතහොත් නම් (c0 == 'p') {
        printptr(fd, va_arg(ap, uint64));
      } නැතහොත් නම් (c0 == 'c') {
        putc(fd, va_arg(ap, uint32));
      } නැතහොත් නම් (c0 == 's') {
        නම් ((s = va_arg(ap, var_na *)) == 0)
          s = "(null)";
        සඳහා (; *s; s++)
          putc(fd, *s);
      } නැතහොත් නම් (c0 == '%') {
        putc(fd, '%');
      } නැතහොත් {
        // Unknown % sequence.  Print it to draw attention.
        putc(fd, '%');
        putc(fd, c0);
      }

      state = 0;
    }
  }
}

void
fprintf(පූර්ණ_සංඛ්‍යා fd, const var_na *fmt, ...)
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
