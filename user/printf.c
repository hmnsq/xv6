#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#include <stdarg.h>

static var_na digits[] = "0123456789ABCDEF";

static void
putc(จำนวนเต็ม fd, var_na c)
{
  write(fd, &c, 1);
}

static void
printint(จำนวนเต็ม fd, long long xx, จำนวนเต็ม base, จำนวนเต็ม sgn)
{
  var_na buf[20];
  จำนวนเต็ม i, neg;
  unsigned long long x;

  neg = 0;
  ถ้า (sgn && xx < 0) {
    neg = 1;
    x = -xx;
  } มิฉะนั้น {
    x = xx;
  }

  i = 0;
  do {
    buf[i++] = digits[x % base];
  } ขณะ ((x /= base) != 0);
  ถ้า (neg)
    buf[i++] = '-';

  ขณะ (--i >= 0)
    putc(fd, buf[i]);
}

static void
printptr(จำนวนเต็ม fd, uint64 x)
{
  จำนวนเต็ม i;
  putc(fd, '0');
  putc(fd, 'x');
  สำหรับ (i = 0; i < (parimaa_na(uint64) * 2); i++, x <<= 4)
    putc(fd, digits[x >> (parimaa_na(uint64) * 8 - 4)]);
}

// Print to the given fd. Only understands %d, %x, %p, %c, %s.
void
vprintf(จำนวนเต็ม fd, const var_na *fmt, va_list ap)
{
  var_na *s;
  จำนวนเต็ม c0, c1, c2, i, state;

  state = 0;
  สำหรับ (i = 0; fmt[i]; i++) {
    c0 = fmt[i] & 0xff;
    ถ้า (state == 0) {
      ถ้า (c0 == '%') {
        state = '%';
      } มิฉะนั้น {
        putc(fd, c0);
      }
    } มิฉะนั้น ถ้า (state == '%') {
      c1 = c2 = 0;
      ถ้า (c0)
        c1 = fmt[i + 1] & 0xff;
      ถ้า (c1)
        c2 = fmt[i + 2] & 0xff;
      ถ้า (c0 == 'd') {
        printint(fd, va_arg(ap, จำนวนเต็ม), 10, 1);
      } มิฉะนั้น ถ้า (c0 == 'l' && c1 == 'd') {
        printint(fd, va_arg(ap, uint64), 10, 1);
        i += 1;
      } มิฉะนั้น ถ้า (c0 == 'l' && c1 == 'l' && c2 == 'd') {
        printint(fd, va_arg(ap, uint64), 10, 1);
        i += 2;
      } มิฉะนั้น ถ้า (c0 == 'u') {
        printint(fd, va_arg(ap, uint32), 10, 0);
      } มิฉะนั้น ถ้า (c0 == 'l' && c1 == 'u') {
        printint(fd, va_arg(ap, uint64), 10, 0);
        i += 1;
      } มิฉะนั้น ถ้า (c0 == 'l' && c1 == 'l' && c2 == 'u') {
        printint(fd, va_arg(ap, uint64), 10, 0);
        i += 2;
      } มิฉะนั้น ถ้า (c0 == 'x') {
        printint(fd, va_arg(ap, uint32), 16, 0);
      } มิฉะนั้น ถ้า (c0 == 'l' && c1 == 'x') {
        printint(fd, va_arg(ap, uint64), 16, 0);
        i += 1;
      } มิฉะนั้น ถ้า (c0 == 'l' && c1 == 'l' && c2 == 'x') {
        printint(fd, va_arg(ap, uint64), 16, 0);
        i += 2;
      } มิฉะนั้น ถ้า (c0 == 'p') {
        printptr(fd, va_arg(ap, uint64));
      } มิฉะนั้น ถ้า (c0 == 'c') {
        putc(fd, va_arg(ap, uint32));
      } มิฉะนั้น ถ้า (c0 == 's') {
        ถ้า ((s = va_arg(ap, var_na *)) == 0)
          s = "(null)";
        สำหรับ (; *s; s++)
          putc(fd, *s);
      } มิฉะนั้น ถ้า (c0 == '%') {
        putc(fd, '%');
      } มิฉะนั้น {
        // Unknown % sequence.  Print it to draw attention.
        putc(fd, '%');
        putc(fd, c0);
      }

      state = 0;
    }
  }
}

void
fprintf(จำนวนเต็ม fd, const var_na *fmt, ...)
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
