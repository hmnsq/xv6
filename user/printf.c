#ଅନ୍ତର୍ଭାଵ "kernel/types.h"
#ଅନ୍ତର୍ଭାଵ "kernel/stat.h"
#ଅନ୍ତର୍ଭାଵ "user/user.h"

#ଅନ୍ତର୍ଭାଵ <stdarg.h>

ସ୍ଥିର ଅକ୍ଷର digits[] = "0123456789ABCDEF";

ସ୍ଥିର ଶୂନ୍ୟ
putc(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ଅକ୍ଷର c)
{
  ଲିଖ(fd, &c, 1);
}

ସ୍ଥିର ଶୂନ୍ୟ
printint(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, long long xx, ପୂର୍ଣ୍ଣସଂଖ୍ୟା base, ପୂର୍ଣ୍ଣସଂଖ୍ୟା sgn)
{
  ଅକ୍ଷର buf[20];
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା i, neg;
  unsigned long long x;

  neg = 0;
  ଯଦି (sgn && xx < 0) {
    neg = 1;
    x = -xx;
  } ଅନ୍ୟଥା {
    x = xx;
  }

  i = 0;
  କର {
    buf[i++] = digits[x % base];
  } ଯେତେବେଳେ ((x /= base) != 0);
  ଯଦି (neg)
    buf[i++] = '-';

  ଯେତେବେଳେ (--i >= 0)
    putc(fd, buf[i]);
}

ସ୍ଥିର ଶୂନ୍ୟ
printptr(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, uint64 x)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
  putc(fd, '0');
  putc(fd, 'x');
  ପାଇଁ (i = 0; i < (ଆକାର(uint64) * 2); i++, x <<= 4)
    putc(fd, digits[x >> (ଆକାର(uint64) * 8 - 4)]);
}

// Print to the given fd. Only understands %d, %x, %p, %c, %s.
ଶୂନ୍ୟ
vprintf(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ଅଚଳ ଅକ୍ଷର *fmt, va_list ap)
{
  ଅକ୍ଷର *s;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା c0, c1, c2, i, state;

  state = 0;
  ପାଇଁ (i = 0; fmt[i]; i++) {
    c0 = fmt[i] & 0xff;
    ଯଦି (state == 0) {
      ଯଦି (c0 == '%') {
        state = '%';
      } ଅନ୍ୟଥା {
        putc(fd, c0);
      }
    } ଅନ୍ୟଥା ଯଦି (state == '%') {
      c1 = c2 = 0;
      ଯଦି (c0)
        c1 = fmt[i + 1] & 0xff;
      ଯଦି (c1)
        c2 = fmt[i + 2] & 0xff;
      ଯଦି (c0 == 'd') {
        printint(fd, va_arg(ap, ପୂର୍ଣ୍ଣସଂଖ୍ୟା), 10, 1);
      } ଅନ୍ୟଥା ଯଦି (c0 == 'l' && c1 == 'd') {
        printint(fd, va_arg(ap, uint64), 10, 1);
        i += 1;
      } ଅନ୍ୟଥା ଯଦି (c0 == 'l' && c1 == 'l' && c2 == 'd') {
        printint(fd, va_arg(ap, uint64), 10, 1);
        i += 2;
      } ଅନ୍ୟଥା ଯଦି (c0 == 'u') {
        printint(fd, va_arg(ap, uint32), 10, 0);
      } ଅନ୍ୟଥା ଯଦି (c0 == 'l' && c1 == 'u') {
        printint(fd, va_arg(ap, uint64), 10, 0);
        i += 1;
      } ଅନ୍ୟଥା ଯଦି (c0 == 'l' && c1 == 'l' && c2 == 'u') {
        printint(fd, va_arg(ap, uint64), 10, 0);
        i += 2;
      } ଅନ୍ୟଥା ଯଦି (c0 == 'x') {
        printint(fd, va_arg(ap, uint32), 16, 0);
      } ଅନ୍ୟଥା ଯଦି (c0 == 'l' && c1 == 'x') {
        printint(fd, va_arg(ap, uint64), 16, 0);
        i += 1;
      } ଅନ୍ୟଥା ଯଦି (c0 == 'l' && c1 == 'l' && c2 == 'x') {
        printint(fd, va_arg(ap, uint64), 16, 0);
        i += 2;
      } ଅନ୍ୟଥା ଯଦି (c0 == 'p') {
        printptr(fd, va_arg(ap, uint64));
      } ଅନ୍ୟଥା ଯଦି (c0 == 'c') {
        putc(fd, va_arg(ap, uint32));
      } ଅନ୍ୟଥା ଯଦି (c0 == 's') {
        ଯଦି ((s = va_arg(ap, ଅକ୍ଷର *)) == 0)
          s = "(null)";
        ପାଇଁ (; *s; s++)
          putc(fd, *s);
      } ଅନ୍ୟଥା ଯଦି (c0 == '%') {
        putc(fd, '%');
      } ଅନ୍ୟଥା {
        // Unknown % sequence.  Print it to draw attention.
        putc(fd, '%');
        putc(fd, c0);
      }

      state = 0;
    }
  }
}

ଶୂନ୍ୟ
fprintf(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ଅଚଳ ଅକ୍ଷର *fmt, ...)
{
  va_list ap;

  va_start(ap, fmt);
  vprintf(fd, fmt, ap);
}

ଶୂନ୍ୟ
printf(ଅଚଳ ଅକ୍ଷର *fmt, ...)
{
  va_list ap;

  va_start(ap, fmt);
  vprintf(1, fmt, ap);
}
