#அந்தர்பாவ "kernel/types.h"
#அந்தர்பாவ "kernel/stat.h"
#அந்தர்பாவ "user/user.h"

#அந்தர்பாவ <stdarg.h>

நிலையான எழுத்து digits[] = "0123456789ABCDEF";

நிலையான வெறுமை
putc(முழு fd, எழுத்து c)
{
  லிக(fd, &c, 1);
}

நிலையான வெறுமை
printint(முழு fd, long long xx, முழு base, முழு sgn)
{
  எழுத்து buf[20];
  முழு i, neg;
  unsigned long long x;

  neg = 0;
  எனில் (sgn && xx < 0) {
    neg = 1;
    x = -xx;
  } இல்லையெனில் {
    x = xx;
  }

  i = 0;
  செய் {
    buf[i++] = digits[x % base];
  } போது ((x /= base) != 0);
  எனில் (neg)
    buf[i++] = '-';

  போது (--i >= 0)
    putc(fd, buf[i]);
}

நிலையான வெறுமை
printptr(முழு fd, uint64 x)
{
  முழு i;
  putc(fd, '0');
  putc(fd, 'x');
  க்கு (i = 0; i < (அளவு(uint64) * 2); i++, x <<= 4)
    putc(fd, digits[x >> (அளவு(uint64) * 8 - 4)]);
}

// Print to the given fd. Only understands %d, %x, %p, %c, %s.
வெறுமை
vprintf(முழு fd, மாறிலி எழுத்து *fmt, va_list ap)
{
  எழுத்து *s;
  முழு c0, c1, c2, i, state;

  state = 0;
  க்கு (i = 0; fmt[i]; i++) {
    c0 = fmt[i] & 0xff;
    எனில் (state == 0) {
      எனில் (c0 == '%') {
        state = '%';
      } இல்லையெனில் {
        putc(fd, c0);
      }
    } இல்லையெனில் எனில் (state == '%') {
      c1 = c2 = 0;
      எனில் (c0)
        c1 = fmt[i + 1] & 0xff;
      எனில் (c1)
        c2 = fmt[i + 2] & 0xff;
      எனில் (c0 == 'd') {
        printint(fd, va_arg(ap, முழு), 10, 1);
      } இல்லையெனில் எனில் (c0 == 'l' && c1 == 'd') {
        printint(fd, va_arg(ap, uint64), 10, 1);
        i += 1;
      } இல்லையெனில் எனில் (c0 == 'l' && c1 == 'l' && c2 == 'd') {
        printint(fd, va_arg(ap, uint64), 10, 1);
        i += 2;
      } இல்லையெனில் எனில் (c0 == 'u') {
        printint(fd, va_arg(ap, uint32), 10, 0);
      } இல்லையெனில் எனில் (c0 == 'l' && c1 == 'u') {
        printint(fd, va_arg(ap, uint64), 10, 0);
        i += 1;
      } இல்லையெனில் எனில் (c0 == 'l' && c1 == 'l' && c2 == 'u') {
        printint(fd, va_arg(ap, uint64), 10, 0);
        i += 2;
      } இல்லையெனில் எனில் (c0 == 'x') {
        printint(fd, va_arg(ap, uint32), 16, 0);
      } இல்லையெனில் எனில் (c0 == 'l' && c1 == 'x') {
        printint(fd, va_arg(ap, uint64), 16, 0);
        i += 1;
      } இல்லையெனில் எனில் (c0 == 'l' && c1 == 'l' && c2 == 'x') {
        printint(fd, va_arg(ap, uint64), 16, 0);
        i += 2;
      } இல்லையெனில் எனில் (c0 == 'p') {
        printptr(fd, va_arg(ap, uint64));
      } இல்லையெனில் எனில் (c0 == 'c') {
        putc(fd, va_arg(ap, uint32));
      } இல்லையெனில் எனில் (c0 == 's') {
        எனில் ((s = va_arg(ap, எழுத்து *)) == 0)
          s = "(null)";
        க்கு (; *s; s++)
          putc(fd, *s);
      } இல்லையெனில் எனில் (c0 == '%') {
        putc(fd, '%');
      } இல்லையெனில் {
        // Unknown % sequence.  Print it to draw attention.
        putc(fd, '%');
        putc(fd, c0);
      }

      state = 0;
    }
  }
}

வெறுமை
fprintf(முழு fd, மாறிலி எழுத்து *fmt, ...)
{
  va_list ap;

  va_start(ap, fmt);
  vprintf(fd, fmt, ap);
}

வெறுமை
printf(மாறிலி எழுத்து *fmt, ...)
{
  va_list ap;

  va_start(ap, fmt);
  vprintf(1, fmt, ap);
}
