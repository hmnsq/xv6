#অন্তর্ভাব "kernel/types.h"
#অন্তর্ভাব "kernel/stat.h"
#অন্তর্ভাব "user/user.h"

#অন্তর্ভাব <stdarg.h>

স্থির অক্ষর digits[] = "0123456789ABCDEF";

স্থির শূন্য
putc(পূর্ণসংখ্যা fd, অক্ষর c)
{
  লিখ(fd, &c, 1);
}

স্থির শূন্য
printint(পূর্ণসংখ্যা fd, long long xx, পূর্ণসংখ্যা base, পূর্ণসংখ্যা sgn)
{
  অক্ষর buf[20];
  পূর্ণসংখ্যা i, neg;
  unsigned long long x;

  neg = 0;
  যদি (sgn && xx < 0) {
    neg = 1;
    x = -xx;
  } অন্যথা {
    x = xx;
  }

  i = 0;
  করো {
    buf[i++] = digits[x % base];
  } যখন ((x /= base) != 0);
  যদি (neg)
    buf[i++] = '-';

  যখন (--i >= 0)
    putc(fd, buf[i]);
}

স্থির শূন্য
printptr(পূর্ণসংখ্যা fd, uint64 x)
{
  পূর্ণসংখ্যা i;
  putc(fd, '0');
  putc(fd, 'x');
  জন্য (i = 0; i < (আকার(uint64) * 2); i++, x <<= 4)
    putc(fd, digits[x >> (আকার(uint64) * 8 - 4)]);
}

// Print to the given fd. Only understands %d, %x, %p, %c, %s.
শূন্য
vprintf(পূর্ণসংখ্যা fd, ধ্রুবক অক্ষর *fmt, va_list ap)
{
  অক্ষর *s;
  পূর্ণসংখ্যা c0, c1, c2, i, state;

  state = 0;
  জন্য (i = 0; fmt[i]; i++) {
    c0 = fmt[i] & 0xff;
    যদি (state == 0) {
      যদি (c0 == '%') {
        state = '%';
      } অন্যথা {
        putc(fd, c0);
      }
    } অন্যথা যদি (state == '%') {
      c1 = c2 = 0;
      যদি (c0)
        c1 = fmt[i + 1] & 0xff;
      যদি (c1)
        c2 = fmt[i + 2] & 0xff;
      যদি (c0 == 'd') {
        printint(fd, va_arg(ap, পূর্ণসংখ্যা), 10, 1);
      } অন্যথা যদি (c0 == 'l' && c1 == 'd') {
        printint(fd, va_arg(ap, uint64), 10, 1);
        i += 1;
      } অন্যথা যদি (c0 == 'l' && c1 == 'l' && c2 == 'd') {
        printint(fd, va_arg(ap, uint64), 10, 1);
        i += 2;
      } অন্যথা যদি (c0 == 'u') {
        printint(fd, va_arg(ap, uint32), 10, 0);
      } অন্যথা যদি (c0 == 'l' && c1 == 'u') {
        printint(fd, va_arg(ap, uint64), 10, 0);
        i += 1;
      } অন্যথা যদি (c0 == 'l' && c1 == 'l' && c2 == 'u') {
        printint(fd, va_arg(ap, uint64), 10, 0);
        i += 2;
      } অন্যথা যদি (c0 == 'x') {
        printint(fd, va_arg(ap, uint32), 16, 0);
      } অন্যথা যদি (c0 == 'l' && c1 == 'x') {
        printint(fd, va_arg(ap, uint64), 16, 0);
        i += 1;
      } অন্যথা যদি (c0 == 'l' && c1 == 'l' && c2 == 'x') {
        printint(fd, va_arg(ap, uint64), 16, 0);
        i += 2;
      } অন্যথা যদি (c0 == 'p') {
        printptr(fd, va_arg(ap, uint64));
      } অন্যথা যদি (c0 == 'c') {
        putc(fd, va_arg(ap, uint32));
      } অন্যথা যদি (c0 == 's') {
        যদি ((s = va_arg(ap, অক্ষর *)) == 0)
          s = "(null)";
        জন্য (; *s; s++)
          putc(fd, *s);
      } অন্যথা যদি (c0 == '%') {
        putc(fd, '%');
      } অন্যথা {
        // Unknown % sequence.  Print it to draw attention.
        putc(fd, '%');
        putc(fd, c0);
      }

      state = 0;
    }
  }
}

শূন্য
fprintf(পূর্ণসংখ্যা fd, ধ্রুবক অক্ষর *fmt, ...)
{
  va_list ap;

  va_start(ap, fmt);
  vprintf(fd, fmt, ap);
}

শূন্য
printf(ধ্রুবক অক্ষর *fmt, ...)
{
  va_list ap;

  va_start(ap, fmt);
  vprintf(1, fmt, ap);
}
