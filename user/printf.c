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
printint(पूर्णाङ्क fd, दीर्घ दीर्घ xx, पूर्णाङ्क आधारः, पूर्णाङ्क sgn)
{
  वर्ण buf[20];
  पूर्णाङ्क i, neg;
  अचिह्नित दीर्घ दीर्घ x;

  neg = 0;
  यदि (sgn && xx < 0) {
    neg = 1;
    x = -xx;
  } अन्यथा {
    x = xx;
  }

  i = 0;
  कुरु {
    buf[i++] = digits[x % आधारः];
  } यावत् ((x /= आधारः) != 0);
  यदि (neg)
    buf[i++] = '-';

  यावत् (--i >= 0)
    putc(fd, buf[i]);
}

स्थिर शून्य
printptr(पूर्णाङ्क fd, uint64 x)
{
  पूर्णाङ्क i;
  putc(fd, '0');
  putc(fd, 'x');
  प्रति (i = 0; i < (परिमाण(uint64) * 2); i++, x <<= 4)
    putc(fd, digits[x >> (परिमाण(uint64) * 8 - 4)]);
}

// Print to the given fd. Only understands %d, %x, %p, %c, %s.
शून्य
vprintf(पूर्णाङ्क fd, ध्रुव वर्ण *fmt, va_list ap)
{
  वर्ण *s;
  पूर्णाङ्क c0, c1, c2, i, state;

  state = 0;
  प्रति (i = 0; fmt[i]; i++) {
    c0 = fmt[i] & 0xff;
    यदि (state == 0) {
      यदि (c0 == '%') {
        state = '%';
      } अन्यथा {
        putc(fd, c0);
      }
    } अन्यथा यदि (state == '%') {
      c1 = c2 = 0;
      यदि (c0)
        c1 = fmt[i + 1] & 0xff;
      यदि (c1)
        c2 = fmt[i + 2] & 0xff;
      यदि (c0 == 'd') {
        printint(fd, va_arg(ap, पूर्णाङ्क), 10, 1);
      } अन्यथा यदि (c0 == 'l' && c1 == 'd') {
        printint(fd, va_arg(ap, uint64), 10, 1);
        i += 1;
      } अन्यथा यदि (c0 == 'l' && c1 == 'l' && c2 == 'd') {
        printint(fd, va_arg(ap, uint64), 10, 1);
        i += 2;
      } अन्यथा यदि (c0 == 'u') {
        printint(fd, va_arg(ap, uint32), 10, 0);
      } अन्यथा यदि (c0 == 'l' && c1 == 'u') {
        printint(fd, va_arg(ap, uint64), 10, 0);
        i += 1;
      } अन्यथा यदि (c0 == 'l' && c1 == 'l' && c2 == 'u') {
        printint(fd, va_arg(ap, uint64), 10, 0);
        i += 2;
      } अन्यथा यदि (c0 == 'x') {
        printint(fd, va_arg(ap, uint32), 16, 0);
      } अन्यथा यदि (c0 == 'l' && c1 == 'x') {
        printint(fd, va_arg(ap, uint64), 16, 0);
        i += 1;
      } अन्यथा यदि (c0 == 'l' && c1 == 'l' && c2 == 'x') {
        printint(fd, va_arg(ap, uint64), 16, 0);
        i += 2;
      } अन्यथा यदि (c0 == 'p') {
        printptr(fd, va_arg(ap, uint64));
      } अन्यथा यदि (c0 == 'c') {
        putc(fd, va_arg(ap, uint32));
      } अन्यथा यदि (c0 == 's') {
        यदि ((s = va_arg(ap, वर्ण *)) == 0)
          s = "(null)";
        प्रति (; *s; s++)
          putc(fd, *s);
      } अन्यथा यदि (c0 == '%') {
        putc(fd, '%');
      } अन्यथा {
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
प्रारूप_लिख(ध्रुव वर्ण *fmt, ...)
{
  va_list ap;

  va_start(ap, fmt);
  vprintf(1, fmt, ap);
}
