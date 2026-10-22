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
  אם (sgn && xx < 0) {
    neg = 1;
    x = -xx;
  } אחרת {
    x = xx;
  }

  i = 0;
  עשה {
    buf[i++] = digits[x % base];
  } כלעוד ((x /= base) != 0);
  אם (neg)
    buf[i++] = '-';

  כלעוד (--i >= 0)
    putc(fd, buf[i]);
}

ساكن فراغ
printptr(صحيح fd, uint64 x)
{
  صحيح i;
  putc(fd, '0');
  putc(fd, 'x');
  עבור (i = 0; i < (حجم(uint64) * 2); i++, x <<= 4)
    putc(fd, digits[x >> (حجم(uint64) * 8 - 4)]);
}

// Print to the given fd. Only understands %d, %x, %p, %c, %s.
فراغ
vprintf(صحيح fd, ثابت حرف *fmt, va_list ap)
{
  حرف *s;
  صحيح c0, c1, c2, i, state;

  state = 0;
  עבור (i = 0; fmt[i]; i++) {
    c0 = fmt[i] & 0xff;
    אם (state == 0) {
      אם (c0 == '%') {
        state = '%';
      } אחרת {
        putc(fd, c0);
      }
    } אחרת אם (state == '%') {
      c1 = c2 = 0;
      אם (c0)
        c1 = fmt[i + 1] & 0xff;
      אם (c1)
        c2 = fmt[i + 2] & 0xff;
      אם (c0 == 'd') {
        printint(fd, va_arg(ap, صحيح), 10, 1);
      } אחרת אם (c0 == 'l' && c1 == 'd') {
        printint(fd, va_arg(ap, uint64), 10, 1);
        i += 1;
      } אחרת אם (c0 == 'l' && c1 == 'l' && c2 == 'd') {
        printint(fd, va_arg(ap, uint64), 10, 1);
        i += 2;
      } אחרת אם (c0 == 'u') {
        printint(fd, va_arg(ap, uint32), 10, 0);
      } אחרת אם (c0 == 'l' && c1 == 'u') {
        printint(fd, va_arg(ap, uint64), 10, 0);
        i += 1;
      } אחרת אם (c0 == 'l' && c1 == 'l' && c2 == 'u') {
        printint(fd, va_arg(ap, uint64), 10, 0);
        i += 2;
      } אחרת אם (c0 == 'x') {
        printint(fd, va_arg(ap, uint32), 16, 0);
      } אחרת אם (c0 == 'l' && c1 == 'x') {
        printint(fd, va_arg(ap, uint64), 16, 0);
        i += 1;
      } אחרת אם (c0 == 'l' && c1 == 'l' && c2 == 'x') {
        printint(fd, va_arg(ap, uint64), 16, 0);
        i += 2;
      } אחרת אם (c0 == 'p') {
        printptr(fd, va_arg(ap, uint64));
      } אחרת אם (c0 == 'c') {
        putc(fd, va_arg(ap, uint32));
      } אחרת אם (c0 == 's') {
        אם ((s = va_arg(ap, حرف *)) == 0)
          s = "(null)";
        עבור (; *s; s++)
          putc(fd, *s);
      } אחרת אם (c0 == '%') {
        putc(fd, '%');
      } אחרת {
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
