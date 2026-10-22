#ಅನ್ತರ್ಭಾವ "kernel/types.h"
#ಅನ್ತರ್ಭಾವ "kernel/stat.h"
#ಅನ್ತರ್ಭಾವ "user/user.h"

#ಅನ್ತರ್ಭಾವ <stdarg.h>

ಸ್ಥಿರ ವರ್ಣ digits[] = "0123456789ABCDEF";

ಸ್ಥಿರ ಶೂನ್ಯ
putc(ಪೂರ್ಣಾಙ್ಕ fd, ವರ್ಣ c)
{
  ಲಿಖ(fd, &c, 1);
}

ಸ್ಥಿರ ಶೂನ್ಯ
printint(ಪೂರ್ಣಾಙ್ಕ fd, long long xx, ಪೂರ್ಣಾಙ್ಕ base, ಪೂರ್ಣಾಙ್ಕ sgn)
{
  ವರ್ಣ buf[20];
  ಪೂರ್ಣಾಙ್ಕ i, neg;
  unsigned long long x;

  neg = 0;
  ಯದಿ (sgn && xx < 0) {
    neg = 1;
    x = -xx;
  } ಅನ್ಯಥಾ {
    x = xx;
  }

  i = 0;
  ಕುರು {
    buf[i++] = digits[x % base];
  } ಯಾವತ್ ((x /= base) != 0);
  ಯದಿ (neg)
    buf[i++] = '-';

  ಯಾವತ್ (--i >= 0)
    putc(fd, buf[i]);
}

ಸ್ಥಿರ ಶೂನ್ಯ
printptr(ಪೂರ್ಣಾಙ್ಕ fd, uint64 x)
{
  ಪೂರ್ಣಾಙ್ಕ i;
  putc(fd, '0');
  putc(fd, 'x');
  ಪ್ರತಿ (i = 0; i < (ಪರಿಮಾಣ(uint64) * 2); i++, x <<= 4)
    putc(fd, digits[x >> (ಪರಿಮಾಣ(uint64) * 8 - 4)]);
}

// Print to the given fd. Only understands %d, %x, %p, %c, %s.
ಶೂನ್ಯ
vprintf(ಪೂರ್ಣಾಙ್ಕ fd, ಧ್ರುವ ವರ್ಣ *fmt, va_list ap)
{
  ವರ್ಣ *s;
  ಪೂರ್ಣಾಙ್ಕ c0, c1, c2, i, state;

  state = 0;
  ಪ್ರತಿ (i = 0; fmt[i]; i++) {
    c0 = fmt[i] & 0xff;
    ಯದಿ (state == 0) {
      ಯದಿ (c0 == '%') {
        state = '%';
      } ಅನ್ಯಥಾ {
        putc(fd, c0);
      }
    } ಅನ್ಯಥಾ ಯದಿ (state == '%') {
      c1 = c2 = 0;
      ಯದಿ (c0)
        c1 = fmt[i + 1] & 0xff;
      ಯದಿ (c1)
        c2 = fmt[i + 2] & 0xff;
      ಯದಿ (c0 == 'd') {
        printint(fd, va_arg(ap, ಪೂರ್ಣಾಙ್ಕ), 10, 1);
      } ಅನ್ಯಥಾ ಯದಿ (c0 == 'l' && c1 == 'd') {
        printint(fd, va_arg(ap, uint64), 10, 1);
        i += 1;
      } ಅನ್ಯಥಾ ಯದಿ (c0 == 'l' && c1 == 'l' && c2 == 'd') {
        printint(fd, va_arg(ap, uint64), 10, 1);
        i += 2;
      } ಅನ್ಯಥಾ ಯದಿ (c0 == 'u') {
        printint(fd, va_arg(ap, uint32), 10, 0);
      } ಅನ್ಯಥಾ ಯದಿ (c0 == 'l' && c1 == 'u') {
        printint(fd, va_arg(ap, uint64), 10, 0);
        i += 1;
      } ಅನ್ಯಥಾ ಯದಿ (c0 == 'l' && c1 == 'l' && c2 == 'u') {
        printint(fd, va_arg(ap, uint64), 10, 0);
        i += 2;
      } ಅನ್ಯಥಾ ಯದಿ (c0 == 'x') {
        printint(fd, va_arg(ap, uint32), 16, 0);
      } ಅನ್ಯಥಾ ಯದಿ (c0 == 'l' && c1 == 'x') {
        printint(fd, va_arg(ap, uint64), 16, 0);
        i += 1;
      } ಅನ್ಯಥಾ ಯದಿ (c0 == 'l' && c1 == 'l' && c2 == 'x') {
        printint(fd, va_arg(ap, uint64), 16, 0);
        i += 2;
      } ಅನ್ಯಥಾ ಯದಿ (c0 == 'p') {
        printptr(fd, va_arg(ap, uint64));
      } ಅನ್ಯಥಾ ಯದಿ (c0 == 'c') {
        putc(fd, va_arg(ap, uint32));
      } ಅನ್ಯಥಾ ಯದಿ (c0 == 's') {
        ಯದಿ ((s = va_arg(ap, ವರ್ಣ *)) == 0)
          s = "(null)";
        ಪ್ರತಿ (; *s; s++)
          putc(fd, *s);
      } ಅನ್ಯಥಾ ಯದಿ (c0 == '%') {
        putc(fd, '%');
      } ಅನ್ಯಥಾ {
        // Unknown % sequence.  Print it to draw attention.
        putc(fd, '%');
        putc(fd, c0);
      }

      state = 0;
    }
  }
}

ಶೂನ್ಯ
fprintf(ಪೂರ್ಣಾಙ್ಕ fd, ಧ್ರುವ ವರ್ಣ *fmt, ...)
{
  va_list ap;

  va_start(ap, fmt);
  vprintf(fd, fmt, ap);
}

ಶೂನ್ಯ
printf(ಧ್ರುವ ವರ್ಣ *fmt, ...)
{
  va_list ap;

  va_start(ap, fmt);
  vprintf(1, fmt, ap);
}
