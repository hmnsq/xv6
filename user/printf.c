#ಅನ್ತರ್ಭಾವ "kernel/types.h"
#ಅನ್ತರ್ಭಾವ "kernel/stat.h"
#ಅನ್ತರ್ಭಾವ "user/user.h"

#ಅನ್ತರ್ಭಾವ <stdarg.h>

ಸ್ಥಿರ ಅಕ್ಷರ digits[] = "0123456789ABCDEF";

ಸ್ಥಿರ ಶೂನ್ಯ
putc(ಪೂರ್ಣಾಂಕ fd, ಅಕ್ಷರ c)
{
  ಲಿಖ(fd, &c, 1);
}

ಸ್ಥಿರ ಶೂನ್ಯ
printint(ಪೂರ್ಣಾಂಕ fd, ದೀರ್ಘ ದೀರ್ಘ xx, ಪೂರ್ಣಾಂಕ ತಳ, ಪೂರ್ಣಾಂಕ sgn)
{
  ಅಕ್ಷರ buf[20];
  ಪೂರ್ಣಾಂಕ i, neg;
  ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ದೀರ್ಘ x;

  neg = 0;
  ಆದರೆ (sgn && xx < 0) {
    neg = 1;
    x = -xx;
  } ಇಲ್ಲದಿದ್ದರೆ {
    x = xx;
  }

  i = 0;
  ಮಾಡು {
    buf[i++] = digits[x % ತಳ];
  } ಆಗ ((x /= ತಳ) != 0);
  ಆದರೆ (neg)
    buf[i++] = '-';

  ಆಗ (--i >= 0)
    putc(fd, buf[i]);
}

ಸ್ಥಿರ ಶೂನ್ಯ
printptr(ಪೂರ್ಣಾಂಕ fd, uint64 x)
{
  ಪೂರ್ಣಾಂಕ i;
  putc(fd, '0');
  putc(fd, 'x');
  ಗಾಗಿ (i = 0; i < (ಗಾತ್ರ(uint64) * 2); i++, x <<= 4)
    putc(fd, digits[x >> (ಗಾತ್ರ(uint64) * 8 - 4)]);
}

// Print to the given fd. Only understands %d, %x, %p, %c, %s.
ಶೂನ್ಯ
vprintf(ಪೂರ್ಣಾಂಕ fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fmt, va_list ap)
{
  ಅಕ್ಷರ *s;
  ಪೂರ್ಣಾಂಕ c0, c1, c2, i, state;

  state = 0;
  ಗಾಗಿ (i = 0; fmt[i]; i++) {
    c0 = fmt[i] & 0xff;
    ಆದರೆ (state == 0) {
      ಆದರೆ (c0 == '%') {
        state = '%';
      } ಇಲ್ಲದಿದ್ದರೆ {
        putc(fd, c0);
      }
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (state == '%') {
      c1 = c2 = 0;
      ಆದರೆ (c0)
        c1 = fmt[i + 1] & 0xff;
      ಆದರೆ (c1)
        c2 = fmt[i + 2] & 0xff;
      ಆದರೆ (c0 == 'd') {
        printint(fd, va_arg(ap, ಪೂರ್ಣಾಂಕ), 10, 1);
      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c0 == 'l' && c1 == 'd') {
        printint(fd, va_arg(ap, uint64), 10, 1);
        i += 1;
      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c0 == 'l' && c1 == 'l' && c2 == 'd') {
        printint(fd, va_arg(ap, uint64), 10, 1);
        i += 2;
      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c0 == 'u') {
        printint(fd, va_arg(ap, uint32), 10, 0);
      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c0 == 'l' && c1 == 'u') {
        printint(fd, va_arg(ap, uint64), 10, 0);
        i += 1;
      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c0 == 'l' && c1 == 'l' && c2 == 'u') {
        printint(fd, va_arg(ap, uint64), 10, 0);
        i += 2;
      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c0 == 'x') {
        printint(fd, va_arg(ap, uint32), 16, 0);
      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c0 == 'l' && c1 == 'x') {
        printint(fd, va_arg(ap, uint64), 16, 0);
        i += 1;
      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c0 == 'l' && c1 == 'l' && c2 == 'x') {
        printint(fd, va_arg(ap, uint64), 16, 0);
        i += 2;
      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c0 == 'p') {
        printptr(fd, va_arg(ap, uint64));
      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c0 == 'c') {
        putc(fd, va_arg(ap, uint32));
      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c0 == 's') {
        ಆದರೆ ((s = va_arg(ap, ಅಕ್ಷರ *)) == 0)
          s = "(null)";
        ಗಾಗಿ (; *s; s++)
          putc(fd, *s);
      } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c0 == '%') {
        putc(fd, '%');
      } ಇಲ್ಲದಿದ್ದರೆ {
        // Unknown % sequence.  Print it to draw attention.
        putc(fd, '%');
        putc(fd, c0);
      }

      state = 0;
    }
  }
}

ಶೂನ್ಯ
fprintf(ಪೂರ್ಣಾಂಕ fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fmt, ...)
{
  va_list ap;

  va_start(ap, fmt);
  vprintf(fd, fmt, ap);
}

ಶೂನ್ಯ
ಬರೆ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fmt, ...)
{
  va_list ap;

  va_start(ap, fmt);
  vprintf(1, fmt, ap);
}
