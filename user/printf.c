#ضمن "kernel/types.h"
#ضمن "kernel/stat.h"
#ضمن "user/user.h"

#ضمن <stdarg.h>

ساكن حرف digits[] = "0123456789ABCDEF";

ساكن فراغ
putc(صحيح fd, حرف c)
{
  اكتب(fd, &c, 1);
}

ساكن فراغ
printint(صحيح fd, long long xx, صحيح base, صحيح sgn)
{
  حرف buf[20];
  صحيح i, neg;
  unsigned long long x;

  neg = 0;
  إذا (sgn && xx < 0) {
    neg = 1;
    x = -xx;
  } وإلا {
    x = xx;
  }

  i = 0;
  افعل {
    buf[i++] = digits[x % base];
  } طالما ((x /= base) != 0);
  إذا (neg)
    buf[i++] = '-';

  طالما (--i >= 0)
    putc(fd, buf[i]);
}

ساكن فراغ
printptr(صحيح fd, uint64 x)
{
  صحيح i;
  putc(fd, '0');
  putc(fd, 'x');
  لكل (i = 0; i < (حجم(uint64) * 2); i++, x <<= 4)
    putc(fd, digits[x >> (حجم(uint64) * 8 - 4)]);
}

// Print to the given fd. Only understands %d, %x, %p, %c, %s.
فراغ
vprintf(صحيح fd, ثابت حرف *fmt, va_list ap)
{
  حرف *s;
  صحيح c0, c1, c2, i, state;

  state = 0;
  لكل (i = 0; fmt[i]; i++) {
    c0 = fmt[i] & 0xff;
    إذا (state == 0) {
      إذا (c0 == '%') {
        state = '%';
      } وإلا {
        putc(fd, c0);
      }
    } وإلا إذا (state == '%') {
      c1 = c2 = 0;
      إذا (c0)
        c1 = fmt[i + 1] & 0xff;
      إذا (c1)
        c2 = fmt[i + 2] & 0xff;
      إذا (c0 == 'd') {
        printint(fd, va_arg(ap, صحيح), 10, 1);
      } وإلا إذا (c0 == 'l' && c1 == 'd') {
        printint(fd, va_arg(ap, uint64), 10, 1);
        i += 1;
      } وإلا إذا (c0 == 'l' && c1 == 'l' && c2 == 'd') {
        printint(fd, va_arg(ap, uint64), 10, 1);
        i += 2;
      } وإلا إذا (c0 == 'u') {
        printint(fd, va_arg(ap, uint32), 10, 0);
      } وإلا إذا (c0 == 'l' && c1 == 'u') {
        printint(fd, va_arg(ap, uint64), 10, 0);
        i += 1;
      } وإلا إذا (c0 == 'l' && c1 == 'l' && c2 == 'u') {
        printint(fd, va_arg(ap, uint64), 10, 0);
        i += 2;
      } وإلا إذا (c0 == 'x') {
        printint(fd, va_arg(ap, uint32), 16, 0);
      } وإلا إذا (c0 == 'l' && c1 == 'x') {
        printint(fd, va_arg(ap, uint64), 16, 0);
        i += 1;
      } وإلا إذا (c0 == 'l' && c1 == 'l' && c2 == 'x') {
        printint(fd, va_arg(ap, uint64), 16, 0);
        i += 2;
      } وإلا إذا (c0 == 'p') {
        printptr(fd, va_arg(ap, uint64));
      } وإلا إذا (c0 == 'c') {
        putc(fd, va_arg(ap, uint32));
      } وإلا إذا (c0 == 's') {
        إذا ((s = va_arg(ap, حرف *)) == 0)
          s = "(null)";
        لكل (; *s; s++)
          putc(fd, *s);
      } وإلا إذا (c0 == '%') {
        putc(fd, '%');
      } وإلا {
        // Unknown % sequence.  Print it to draw attention.
        putc(fd, '%');
        putc(fd, c0);
      }

      state = 0;
    }
  }
}

فراغ
fprintf(صحيح fd, ثابت حرف *fmt, ...)
{
  va_list ap;

  va_start(ap, fmt);
  vprintf(fd, fmt, ap);
}

فراغ
printf(ثابت حرف *fmt, ...)
{
  va_list ap;

  va_start(ap, fmt);
  vprintf(1, fmt, ap);
}
