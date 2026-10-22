#شامل "kernel/types.h"
#شامل "kernel/stat.h"
#شامل "user/user.h"

#شامل <stdarg.h>

ساکن حرف digits[] = "0123456789ABCDEF";

ساکن خالی
putc(صحیح fd, حرف c)
{
  اكتب(fd, &c, 1);
}

ساکن خالی
printint(صحیح fd, طویل طویل xx, صحیح بنیاد, صحیح sgn)
{
  حرف buf[20];
  صحیح i, neg;
  غیرعلامتی طویل طویل x;

  neg = 0;
  اگر (sgn && xx < 0) {
    neg = 1;
    x = -xx;
  } ورنہ {
    x = xx;
  }

  i = 0;
  کرو {
    buf[i++] = digits[x % بنیاد];
  } جب ((x /= بنیاد) != 0);
  اگر (neg)
    buf[i++] = '-';

  جب (--i >= 0)
    putc(fd, buf[i]);
}

ساکن خالی
printptr(صحیح fd, uint64 x)
{
  صحیح i;
  putc(fd, '0');
  putc(fd, 'x');
  برائے (i = 0; i < (حجم(uint64) * 2); i++, x <<= 4)
    putc(fd, digits[x >> (حجم(uint64) * 8 - 4)]);
}

// Print to the given fd. Only understands %d, %x, %p, %c, %s.
خالی
vprintf(صحیح fd, ثابت حرف *fmt, va_list ap)
{
  حرف *s;
  صحیح c0, c1, c2, i, state;

  state = 0;
  برائے (i = 0; fmt[i]; i++) {
    c0 = fmt[i] & 0xff;
    اگر (state == 0) {
      اگر (c0 == '%') {
        state = '%';
      } ورنہ {
        putc(fd, c0);
      }
    } ورنہ اگر (state == '%') {
      c1 = c2 = 0;
      اگر (c0)
        c1 = fmt[i + 1] & 0xff;
      اگر (c1)
        c2 = fmt[i + 2] & 0xff;
      اگر (c0 == 'd') {
        printint(fd, va_arg(ap, صحیح), 10, 1);
      } ورنہ اگر (c0 == 'l' && c1 == 'd') {
        printint(fd, va_arg(ap, uint64), 10, 1);
        i += 1;
      } ورنہ اگر (c0 == 'l' && c1 == 'l' && c2 == 'd') {
        printint(fd, va_arg(ap, uint64), 10, 1);
        i += 2;
      } ورنہ اگر (c0 == 'u') {
        printint(fd, va_arg(ap, uint32), 10, 0);
      } ورنہ اگر (c0 == 'l' && c1 == 'u') {
        printint(fd, va_arg(ap, uint64), 10, 0);
        i += 1;
      } ورنہ اگر (c0 == 'l' && c1 == 'l' && c2 == 'u') {
        printint(fd, va_arg(ap, uint64), 10, 0);
        i += 2;
      } ورنہ اگر (c0 == 'x') {
        printint(fd, va_arg(ap, uint32), 16, 0);
      } ورنہ اگر (c0 == 'l' && c1 == 'x') {
        printint(fd, va_arg(ap, uint64), 16, 0);
        i += 1;
      } ورنہ اگر (c0 == 'l' && c1 == 'l' && c2 == 'x') {
        printint(fd, va_arg(ap, uint64), 16, 0);
        i += 2;
      } ورنہ اگر (c0 == 'p') {
        printptr(fd, va_arg(ap, uint64));
      } ورنہ اگر (c0 == 'c') {
        putc(fd, va_arg(ap, uint32));
      } ورنہ اگر (c0 == 's') {
        اگر ((s = va_arg(ap, حرف *)) == 0)
          s = "(null)";
        برائے (; *s; s++)
          putc(fd, *s);
      } ورنہ اگر (c0 == '%') {
        putc(fd, '%');
      } ورنہ {
        // Unknown % sequence.  Print it to draw attention.
        putc(fd, '%');
        putc(fd, c0);
      }

      state = 0;
    }
  }
}

خالی
fprintf(صحیح fd, ثابت حرف *fmt, ...)
{
  va_list ap;

  va_start(ap, fmt);
  vprintf(fd, fmt, ap);
}

خالی
لکھو(ثابت حرف *fmt, ...)
{
  va_list ap;

  va_start(ap, fmt);
  vprintf(1, fmt, ap);
}
