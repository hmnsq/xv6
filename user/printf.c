#अन्तर्भाव "kernel/types.h"
#अन्तर्भाव "kernel/stat.h"
#अन्तर्भाव "user/user.h"

#अन्तर्भाव <stdarg.h>

स्थिर वर्ण digits[] = "0123456789ABCDEF";

स्थिर शून्य
putc(पूर्णाङ्क fd, वर्ण c)
{
  लिख(fd, &c, 1);
}

स्थिर शून्य
printint(पूर्णाङ्क fd, long long xx, पूर्णाङ्क base, पूर्णाङ्क sgn)
{
  वर्ण buf[20];
  पूर्णाङ्क i, neg;
  unsigned long long x;

  neg = 0;
  सचे (sgn && xx < 0) {
    neg = 1;
    x = -xx;
  } अञ्ञथा {
    x = xx;
  }

  i = 0;
  करोहि {
    buf[i++] = digits[x % base];
  } याव ((x /= base) != 0);
  सचे (neg)
    buf[i++] = '-';

  याव (--i >= 0)
    putc(fd, buf[i]);
}

स्थिर शून्य
printptr(पूर्णाङ्क fd, uint64 x)
{
  पूर्णाङ्क i;
  putc(fd, '0');
  putc(fd, 'x');
  अत्थाय (i = 0; i < (परिमाण(uint64) * 2); i++, x <<= 4)
    putc(fd, digits[x >> (परिमाण(uint64) * 8 - 4)]);
}

// Print to the given fd. Only understands %d, %x, %p, %c, %s.
शून्य
vprintf(पूर्णाङ्क fd, ध्रुव वर्ण *fmt, va_list ap)
{
  वर्ण *s;
  पूर्णाङ्क c0, c1, c2, i, state;

  state = 0;
  अत्थाय (i = 0; fmt[i]; i++) {
    c0 = fmt[i] & 0xff;
    सचे (state == 0) {
      सचे (c0 == '%') {
        state = '%';
      } अञ्ञथा {
        putc(fd, c0);
      }
    } अञ्ञथा सचे (state == '%') {
      c1 = c2 = 0;
      सचे (c0)
        c1 = fmt[i + 1] & 0xff;
      सचे (c1)
        c2 = fmt[i + 2] & 0xff;
      सचे (c0 == 'd') {
        printint(fd, va_arg(ap, पूर्णाङ्क), 10, 1);
      } अञ्ञथा सचे (c0 == 'l' && c1 == 'd') {
        printint(fd, va_arg(ap, uint64), 10, 1);
        i += 1;
      } अञ्ञथा सचे (c0 == 'l' && c1 == 'l' && c2 == 'd') {
        printint(fd, va_arg(ap, uint64), 10, 1);
        i += 2;
      } अञ्ञथा सचे (c0 == 'u') {
        printint(fd, va_arg(ap, uint32), 10, 0);
      } अञ्ञथा सचे (c0 == 'l' && c1 == 'u') {
        printint(fd, va_arg(ap, uint64), 10, 0);
        i += 1;
      } अञ्ञथा सचे (c0 == 'l' && c1 == 'l' && c2 == 'u') {
        printint(fd, va_arg(ap, uint64), 10, 0);
        i += 2;
      } अञ्ञथा सचे (c0 == 'x') {
        printint(fd, va_arg(ap, uint32), 16, 0);
      } अञ्ञथा सचे (c0 == 'l' && c1 == 'x') {
        printint(fd, va_arg(ap, uint64), 16, 0);
        i += 1;
      } अञ्ञथा सचे (c0 == 'l' && c1 == 'l' && c2 == 'x') {
        printint(fd, va_arg(ap, uint64), 16, 0);
        i += 2;
      } अञ्ञथा सचे (c0 == 'p') {
        printptr(fd, va_arg(ap, uint64));
      } अञ्ञथा सचे (c0 == 'c') {
        putc(fd, va_arg(ap, uint32));
      } अञ्ञथा सचे (c0 == 's') {
        सचे ((s = va_arg(ap, वर्ण *)) == 0)
          s = "(null)";
        अत्थाय (; *s; s++)
          putc(fd, *s);
      } अञ्ञथा सचे (c0 == '%') {
        putc(fd, '%');
      } अञ्ञथा {
        // Unknown % sequence.  Print it to draw attention.
        putc(fd, '%');
        putc(fd, c0);
      }

      state = 0;
    }
  }
}

शून्य
fprintf(पूर्णाङ्क fd, ध्रुव वर्ण *fmt, ...)
{
  va_list ap;

  va_start(ap, fmt);
  vprintf(fd, fmt, ap);
}

शून्य
printf(ध्रुव वर्ण *fmt, ...)
{
  va_list ap;

  va_start(ap, fmt);
  vprintf(1, fmt, ap);
}
