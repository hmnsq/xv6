#അന്തര്ഭാവ "kernel/types.h"
#അന്തര്ഭാവ "kernel/stat.h"
#അന്തര്ഭാവ "user/user.h"

#അന്തര്ഭാവ <stdarg.h>

സ്ഥിതം അക്ഷരം digits[] = "0123456789ABCDEF";

സ്ഥിതം ശൂന്യം
putc(പൂർണ്ണസംഖ്യ fd, അക്ഷരം c)
{
  ലിഖ(fd, &c, 1);
}

സ്ഥിതം ശൂന്യം
printint(പൂർണ്ണസംഖ്യ fd, long long xx, പൂർണ്ണസംഖ്യ base, പൂർണ്ണസംഖ്യ sgn)
{
  അക്ഷരം buf[20];
  പൂർണ്ണസംഖ്യ i, neg;
  unsigned long long x;

  neg = 0;
  എങ്കിൽ (sgn && xx < 0) {
    neg = 1;
    x = -xx;
  } അല്ലെങ്കിൽ {
    x = xx;
  }

  i = 0;
  ചെയ്യുക {
    buf[i++] = digits[x % base];
  } ആകുമ്പോൾ ((x /= base) != 0);
  എങ്കിൽ (neg)
    buf[i++] = '-';

  ആകുമ്പോൾ (--i >= 0)
    putc(fd, buf[i]);
}

സ്ഥിതം ശൂന്യം
printptr(പൂർണ്ണസംഖ്യ fd, uint64 x)
{
  പൂർണ്ണസംഖ്യ i;
  putc(fd, '0');
  putc(fd, 'x');
  വേണ്ടി (i = 0; i < (വലുപ്പം(uint64) * 2); i++, x <<= 4)
    putc(fd, digits[x >> (വലുപ്പം(uint64) * 8 - 4)]);
}

// Print to the given fd. Only understands %d, %x, %p, %c, %s.
ശൂന്യം
vprintf(പൂർണ്ണസംഖ്യ fd, സ്ഥിരം അക്ഷരം *fmt, va_list ap)
{
  അക്ഷരം *s;
  പൂർണ്ണസംഖ്യ c0, c1, c2, i, state;

  state = 0;
  വേണ്ടി (i = 0; fmt[i]; i++) {
    c0 = fmt[i] & 0xff;
    എങ്കിൽ (state == 0) {
      എങ്കിൽ (c0 == '%') {
        state = '%';
      } അല്ലെങ്കിൽ {
        putc(fd, c0);
      }
    } അല്ലെങ്കിൽ എങ്കിൽ (state == '%') {
      c1 = c2 = 0;
      എങ്കിൽ (c0)
        c1 = fmt[i + 1] & 0xff;
      എങ്കിൽ (c1)
        c2 = fmt[i + 2] & 0xff;
      എങ്കിൽ (c0 == 'd') {
        printint(fd, va_arg(ap, പൂർണ്ണസംഖ്യ), 10, 1);
      } അല്ലെങ്കിൽ എങ്കിൽ (c0 == 'l' && c1 == 'd') {
        printint(fd, va_arg(ap, uint64), 10, 1);
        i += 1;
      } അല്ലെങ്കിൽ എങ്കിൽ (c0 == 'l' && c1 == 'l' && c2 == 'd') {
        printint(fd, va_arg(ap, uint64), 10, 1);
        i += 2;
      } അല്ലെങ്കിൽ എങ്കിൽ (c0 == 'u') {
        printint(fd, va_arg(ap, uint32), 10, 0);
      } അല്ലെങ്കിൽ എങ്കിൽ (c0 == 'l' && c1 == 'u') {
        printint(fd, va_arg(ap, uint64), 10, 0);
        i += 1;
      } അല്ലെങ്കിൽ എങ്കിൽ (c0 == 'l' && c1 == 'l' && c2 == 'u') {
        printint(fd, va_arg(ap, uint64), 10, 0);
        i += 2;
      } അല്ലെങ്കിൽ എങ്കിൽ (c0 == 'x') {
        printint(fd, va_arg(ap, uint32), 16, 0);
      } അല്ലെങ്കിൽ എങ്കിൽ (c0 == 'l' && c1 == 'x') {
        printint(fd, va_arg(ap, uint64), 16, 0);
        i += 1;
      } അല്ലെങ്കിൽ എങ്കിൽ (c0 == 'l' && c1 == 'l' && c2 == 'x') {
        printint(fd, va_arg(ap, uint64), 16, 0);
        i += 2;
      } അല്ലെങ്കിൽ എങ്കിൽ (c0 == 'p') {
        printptr(fd, va_arg(ap, uint64));
      } അല്ലെങ്കിൽ എങ്കിൽ (c0 == 'c') {
        putc(fd, va_arg(ap, uint32));
      } അല്ലെങ്കിൽ എങ്കിൽ (c0 == 's') {
        എങ്കിൽ ((s = va_arg(ap, അക്ഷരം *)) == 0)
          s = "(null)";
        വേണ്ടി (; *s; s++)
          putc(fd, *s);
      } അല്ലെങ്കിൽ എങ്കിൽ (c0 == '%') {
        putc(fd, '%');
      } അല്ലെങ്കിൽ {
        // Unknown % sequence.  Print it to draw attention.
        putc(fd, '%');
        putc(fd, c0);
      }

      state = 0;
    }
  }
}

ശൂന്യം
fprintf(പൂർണ്ണസംഖ്യ fd, സ്ഥിരം അക്ഷരം *fmt, ...)
{
  va_list ap;

  va_start(ap, fmt);
  vprintf(fd, fmt, ap);
}

ശൂന്യം
printf(സ്ഥിരം അക്ഷരം *fmt, ...)
{
  va_list ap;

  va_start(ap, fmt);
  vprintf(1, fmt, ap);
}
