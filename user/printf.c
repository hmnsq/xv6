#অন্তর্ভাৱ "kernel/types.h"
#অন্তর্ভাৱ "kernel/stat.h"
#অন্তর্ভাৱ "user/user.h"

#অন্তর্ভাৱ <stdarg.h>

স্থির ৱর্ণ digits[] = "0123456789ABCDEF";

স্থির শূন্য
putc(পূর্ণাঙ্ক fd, ৱর্ণ c)
{
  লিখ(fd, &c, 1);
}

স্থির শূন্য
printint(পূর্ণাঙ্ক fd, long long xx, পূর্ণাঙ্ক base, পূর্ণাঙ্ক sgn)
{
  ৱর্ণ buf[20];
  পূর্ণাঙ্ক i, neg;
  unsigned long long x;

  neg = 0;
  যদি (sgn && xx < 0) {
    neg = 1;
    x = -xx;
  } অন্যথা {
    x = xx;
  }

  i = 0;
  কৰা {
    buf[i++] = digits[x % base];
  } যেতিয়ালৈ ((x /= base) != 0);
  যদি (neg)
    buf[i++] = '-';

  যেতিয়ালৈ (--i >= 0)
    putc(fd, buf[i]);
}

স্থির শূন্য
printptr(পূর্ণাঙ্ক fd, uint64 x)
{
  পূর্ণাঙ্ক i;
  putc(fd, '0');
  putc(fd, 'x');
  বাবে (i = 0; i < (পরিমাণ(uint64) * 2); i++, x <<= 4)
    putc(fd, digits[x >> (পরিমাণ(uint64) * 8 - 4)]);
}

// Print to the given fd. Only understands %d, %x, %p, %c, %s.
শূন্য
vprintf(পূর্ণাঙ্ক fd, ধ্রুৱ ৱর্ণ *fmt, va_list ap)
{
  ৱর্ণ *s;
  পূর্ণাঙ্ক c0, c1, c2, i, state;

  state = 0;
  বাবে (i = 0; fmt[i]; i++) {
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
        printint(fd, va_arg(ap, পূর্ণাঙ্ক), 10, 1);
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
        যদি ((s = va_arg(ap, ৱর্ণ *)) == 0)
          s = "(null)";
        বাবে (; *s; s++)
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
fprintf(পূর্ণাঙ্ক fd, ধ্রুৱ ৱর্ণ *fmt, ...)
{
  va_list ap;

  va_start(ap, fmt);
  vprintf(fd, fmt, ap);
}

শূন্য
printf(ধ্রুৱ ৱর্ণ *fmt, ...)
{
  va_list ap;

  va_start(ap, fmt);
  vprintf(1, fmt, ap);
}
