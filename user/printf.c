#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/types.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/stat.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "user/user.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <stdarg.h>

ਸਥਿਰ ਅੱਖਰ digits[] = "0123456789ABCDEF";

ਸਥਿਰ ਖਾਲੀ
putc(ਪੂਰਨ fd, ਅੱਖਰ c)
{
  ਲਿਖ(fd, &c, 1);
}

ਸਥਿਰ ਖਾਲੀ
printint(ਪੂਰਨ fd, ਲੰਮਾ ਲੰਮਾ xx, ਪੂਰਨ ਅਧਾਰ, ਪੂਰਨ sgn)
{
  ਅੱਖਰ buf[20];
  ਪੂਰਨ i, neg;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਲੰਮਾ x;

  neg = 0;
  ਜੇ (sgn && xx < 0) {
    neg = 1;
    x = -xx;
  } ਨਹੀਂਤਾਂ {
    x = xx;
  }

  i = 0;
  ਕਰੋ {
    buf[i++] = digits[x % ਅਧਾਰ];
  } ਜਦੋਂ ((x /= ਅਧਾਰ) != 0);
  ਜੇ (neg)
    buf[i++] = '-';

  ਜਦੋਂ (--i >= 0)
    putc(fd, buf[i]);
}

ਸਥਿਰ ਖਾਲੀ
printptr(ਪੂਰਨ fd, uint64 x)
{
  ਪੂਰਨ i;
  putc(fd, '0');
  putc(fd, 'x');
  ਲਈ (i = 0; i < (ਆਕਾਰ(uint64) * 2); i++, x <<= 4)
    putc(fd, digits[x >> (ਆਕਾਰ(uint64) * 8 - 4)]);
}

// Print to the given fd. Only understands %d, %x, %p, %c, %s.
ਖਾਲੀ
vprintf(ਪੂਰਨ fd, ਅਚਲ ਅੱਖਰ *fmt, va_list ap)
{
  ਅੱਖਰ *s;
  ਪੂਰਨ c0, c1, c2, i, state;

  state = 0;
  ਲਈ (i = 0; fmt[i]; i++) {
    c0 = fmt[i] & 0xff;
    ਜੇ (state == 0) {
      ਜੇ (c0 == '%') {
        state = '%';
      } ਨਹੀਂਤਾਂ {
        putc(fd, c0);
      }
    } ਨਹੀਂਤਾਂ ਜੇ (state == '%') {
      c1 = c2 = 0;
      ਜੇ (c0)
        c1 = fmt[i + 1] & 0xff;
      ਜੇ (c1)
        c2 = fmt[i + 2] & 0xff;
      ਜੇ (c0 == 'd') {
        printint(fd, va_arg(ap, ਪੂਰਨ), 10, 1);
      } ਨਹੀਂਤਾਂ ਜੇ (c0 == 'l' && c1 == 'd') {
        printint(fd, va_arg(ap, uint64), 10, 1);
        i += 1;
      } ਨਹੀਂਤਾਂ ਜੇ (c0 == 'l' && c1 == 'l' && c2 == 'd') {
        printint(fd, va_arg(ap, uint64), 10, 1);
        i += 2;
      } ਨਹੀਂਤਾਂ ਜੇ (c0 == 'u') {
        printint(fd, va_arg(ap, uint32), 10, 0);
      } ਨਹੀਂਤਾਂ ਜੇ (c0 == 'l' && c1 == 'u') {
        printint(fd, va_arg(ap, uint64), 10, 0);
        i += 1;
      } ਨਹੀਂਤਾਂ ਜੇ (c0 == 'l' && c1 == 'l' && c2 == 'u') {
        printint(fd, va_arg(ap, uint64), 10, 0);
        i += 2;
      } ਨਹੀਂਤਾਂ ਜੇ (c0 == 'x') {
        printint(fd, va_arg(ap, uint32), 16, 0);
      } ਨਹੀਂਤਾਂ ਜੇ (c0 == 'l' && c1 == 'x') {
        printint(fd, va_arg(ap, uint64), 16, 0);
        i += 1;
      } ਨਹੀਂਤਾਂ ਜੇ (c0 == 'l' && c1 == 'l' && c2 == 'x') {
        printint(fd, va_arg(ap, uint64), 16, 0);
        i += 2;
      } ਨਹੀਂਤਾਂ ਜੇ (c0 == 'p') {
        printptr(fd, va_arg(ap, uint64));
      } ਨਹੀਂਤਾਂ ਜੇ (c0 == 'c') {
        putc(fd, va_arg(ap, uint32));
      } ਨਹੀਂਤਾਂ ਜੇ (c0 == 's') {
        ਜੇ ((s = va_arg(ap, ਅੱਖਰ *)) == 0)
          s = "(null)";
        ਲਈ (; *s; s++)
          putc(fd, *s);
      } ਨਹੀਂਤਾਂ ਜੇ (c0 == '%') {
        putc(fd, '%');
      } ਨਹੀਂਤਾਂ {
        // Unknown % sequence.  Print it to draw attention.
        putc(fd, '%');
        putc(fd, c0);
      }

      state = 0;
    }
  }
}

ਖਾਲੀ
fprintf(ਪੂਰਨ fd, ਅਚਲ ਅੱਖਰ *fmt, ...)
{
  va_list ap;

  va_start(ap, fmt);
  vprintf(fd, fmt, ap);
}

ਖਾਲੀ
ਲਿਖੋ(ਅਚਲ ਅੱਖਰ *fmt, ...)
{
  va_list ap;

  va_start(ap, fmt);
  vprintf(1, fmt, ap);
}
