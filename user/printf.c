#అన్తర్భావ "kernel/types.h"
#అన్తర్భావ "kernel/stat.h"
#అన్తర్భావ "user/user.h"

#అన్తర్భావ <stdarg.h>

స్థిర అక్షరం digits[] = "0123456789ABCDEF";

స్థిర శూన్య
putc(పూర్ణాంకం fd, అక్షరం c)
{
  లిఖ(fd, &c, 1);
}

స్థిర శూన్య
printint(పూర్ణాంకం fd, దీర్ఘ దీర్ఘ xx, పూర్ణాంకం పునాది, పూర్ణాంకం sgn)
{
  అక్షరం buf[20];
  పూర్ణాంకం i, neg;
  గుర్తులేని దీర్ఘ దీర్ఘ x;

  neg = 0;
  ఒకవేళ (sgn && xx < 0) {
    neg = 1;
    x = -xx;
  } లేకపోతే {
    x = xx;
  }

  i = 0;
  చేయి {
    buf[i++] = digits[x % పునాది];
  } అయితే ((x /= పునాది) != 0);
  ఒకవేళ (neg)
    buf[i++] = '-';

  అయితే (--i >= 0)
    putc(fd, buf[i]);
}

స్థిర శూన్య
printptr(పూర్ణాంకం fd, uint64 x)
{
  పూర్ణాంకం i;
  putc(fd, '0');
  putc(fd, 'x');
  కొరకు (i = 0; i < (పరిమాణం(uint64) * 2); i++, x <<= 4)
    putc(fd, digits[x >> (పరిమాణం(uint64) * 8 - 4)]);
}

// Print to the given fd. Only understands %d, %x, %p, %c, %s.
శూన్య
vprintf(పూర్ణాంకం fd, స్థిరరాశి అక్షరం *fmt, va_list ap)
{
  అక్షరం *s;
  పూర్ణాంకం c0, c1, c2, i, state;

  state = 0;
  కొరకు (i = 0; fmt[i]; i++) {
    c0 = fmt[i] & 0xff;
    ఒకవేళ (state == 0) {
      ఒకవేళ (c0 == '%') {
        state = '%';
      } లేకపోతే {
        putc(fd, c0);
      }
    } లేకపోతే ఒకవేళ (state == '%') {
      c1 = c2 = 0;
      ఒకవేళ (c0)
        c1 = fmt[i + 1] & 0xff;
      ఒకవేళ (c1)
        c2 = fmt[i + 2] & 0xff;
      ఒకవేళ (c0 == 'd') {
        printint(fd, va_arg(ap, పూర్ణాంకం), 10, 1);
      } లేకపోతే ఒకవేళ (c0 == 'l' && c1 == 'd') {
        printint(fd, va_arg(ap, uint64), 10, 1);
        i += 1;
      } లేకపోతే ఒకవేళ (c0 == 'l' && c1 == 'l' && c2 == 'd') {
        printint(fd, va_arg(ap, uint64), 10, 1);
        i += 2;
      } లేకపోతే ఒకవేళ (c0 == 'u') {
        printint(fd, va_arg(ap, uint32), 10, 0);
      } లేకపోతే ఒకవేళ (c0 == 'l' && c1 == 'u') {
        printint(fd, va_arg(ap, uint64), 10, 0);
        i += 1;
      } లేకపోతే ఒకవేళ (c0 == 'l' && c1 == 'l' && c2 == 'u') {
        printint(fd, va_arg(ap, uint64), 10, 0);
        i += 2;
      } లేకపోతే ఒకవేళ (c0 == 'x') {
        printint(fd, va_arg(ap, uint32), 16, 0);
      } లేకపోతే ఒకవేళ (c0 == 'l' && c1 == 'x') {
        printint(fd, va_arg(ap, uint64), 16, 0);
        i += 1;
      } లేకపోతే ఒకవేళ (c0 == 'l' && c1 == 'l' && c2 == 'x') {
        printint(fd, va_arg(ap, uint64), 16, 0);
        i += 2;
      } లేకపోతే ఒకవేళ (c0 == 'p') {
        printptr(fd, va_arg(ap, uint64));
      } లేకపోతే ఒకవేళ (c0 == 'c') {
        putc(fd, va_arg(ap, uint32));
      } లేకపోతే ఒకవేళ (c0 == 's') {
        ఒకవేళ ((s = va_arg(ap, అక్షరం *)) == 0)
          s = "(null)";
        కొరకు (; *s; s++)
          putc(fd, *s);
      } లేకపోతే ఒకవేళ (c0 == '%') {
        putc(fd, '%');
      } లేకపోతే {
        // Unknown % sequence.  Print it to draw attention.
        putc(fd, '%');
        putc(fd, c0);
      }

      state = 0;
    }
  }
}

శూన్య
fprintf(పూర్ణాంకం fd, స్థిరరాశి అక్షరం *fmt, ...)
{
  va_list ap;

  va_start(ap, fmt);
  vprintf(fd, fmt, ap);
}

శూన్య
రాయి(స్థిరరాశి అక్షరం *fmt, ...)
{
  va_list ap;

  va_start(ap, fmt);
  vprintf(1, fmt, ap);
}
