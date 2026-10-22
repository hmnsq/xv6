#شامل "kernel/types.h"
#شامل "kernel/stat.h"
#شامل "user/user.h"

#شامل <stdarg.h>

ایستا نویسه digits[] = "0123456789ABCDEF";

ایستا تهی
putc(صحیح fd, نویسه c)
{
  اكتب(fd, &c, 1);
}

ایستا تهی
printint(صحیح fd, long long xx, صحیح base, صحیح sgn)
{
  نویسه buf[20];
  صحیح i, neg;
  unsigned long long x;

  neg = 0;
  اگر (sgn && xx < 0) {
    neg = 1;
    x = -xx;
  } وگرنه {
    x = xx;
  }

  i = 0;
  انجامبده {
    buf[i++] = digits[x % base];
  } وقتی ((x /= base) != 0);
  اگر (neg)
    buf[i++] = '-';

  وقتی (--i >= 0)
    putc(fd, buf[i]);
}

ایستا تهی
printptr(صحیح fd, uint64 x)
{
  صحیح i;
  putc(fd, '0');
  putc(fd, 'x');
  برای (i = 0; i < (اندازه(uint64) * 2); i++, x <<= 4)
    putc(fd, digits[x >> (اندازه(uint64) * 8 - 4)]);
}

// Print to the given fd. Only understands %d, %x, %p, %c, %s.
تهی
vprintf(صحیح fd, ثابت نویسه *fmt, va_list ap)
{
  نویسه *s;
  صحیح c0, c1, c2, i, state;

  state = 0;
  برای (i = 0; fmt[i]; i++) {
    c0 = fmt[i] & 0xff;
    اگر (state == 0) {
      اگر (c0 == '%') {
        state = '%';
      } وگرنه {
        putc(fd, c0);
      }
    } وگرنه اگر (state == '%') {
      c1 = c2 = 0;
      اگر (c0)
        c1 = fmt[i + 1] & 0xff;
      اگر (c1)
        c2 = fmt[i + 2] & 0xff;
      اگر (c0 == 'd') {
        printint(fd, va_arg(ap, صحیح), 10, 1);
      } وگرنه اگر (c0 == 'l' && c1 == 'd') {
        printint(fd, va_arg(ap, uint64), 10, 1);
        i += 1;
      } وگرنه اگر (c0 == 'l' && c1 == 'l' && c2 == 'd') {
        printint(fd, va_arg(ap, uint64), 10, 1);
        i += 2;
      } وگرنه اگر (c0 == 'u') {
        printint(fd, va_arg(ap, uint32), 10, 0);
      } وگرنه اگر (c0 == 'l' && c1 == 'u') {
        printint(fd, va_arg(ap, uint64), 10, 0);
        i += 1;
      } وگرنه اگر (c0 == 'l' && c1 == 'l' && c2 == 'u') {
        printint(fd, va_arg(ap, uint64), 10, 0);
        i += 2;
      } وگرنه اگر (c0 == 'x') {
        printint(fd, va_arg(ap, uint32), 16, 0);
      } وگرنه اگر (c0 == 'l' && c1 == 'x') {
        printint(fd, va_arg(ap, uint64), 16, 0);
        i += 1;
      } وگرنه اگر (c0 == 'l' && c1 == 'l' && c2 == 'x') {
        printint(fd, va_arg(ap, uint64), 16, 0);
        i += 2;
      } وگرنه اگر (c0 == 'p') {
        printptr(fd, va_arg(ap, uint64));
      } وگرنه اگر (c0 == 'c') {
        putc(fd, va_arg(ap, uint32));
      } وگرنه اگر (c0 == 's') {
        اگر ((s = va_arg(ap, نویسه *)) == 0)
          s = "(null)";
        برای (; *s; s++)
          putc(fd, *s);
      } وگرنه اگر (c0 == '%') {
        putc(fd, '%');
      } وگرنه {
        // Unknown % sequence.  Print it to draw attention.
        putc(fd, '%');
        putc(fd, c0);
      }

      state = 0;
    }
  }
}

تهی
fprintf(صحیح fd, ثابت نویسه *fmt, ...)
{
  va_list ap;

  va_start(ap, fmt);
  vprintf(fd, fmt, ap);
}

تهی
printf(ثابت نویسه *fmt, ...)
{
  va_list ap;

  va_start(ap, fmt);
  vprintf(1, fmt, ap);
}
