#અન્તર્ભાવ "kernel/types.h"
#અન્તર્ભાવ "kernel/stat.h"
#અન્તર્ભાવ "user/user.h"

#અન્તર્ભાવ <stdarg.h>

સ્થિર અક્ષર digits[] = "0123456789ABCDEF";

સ્થિર શૂન્ય
putc(પૂર્ણાંક fd, અક્ષર c)
{
  લિખ(fd, &c, 1);
}

સ્થિર શૂન્ય
printint(પૂર્ણાંક fd, long long xx, પૂર્ણાંક base, પૂર્ણાંક sgn)
{
  અક્ષર buf[20];
  પૂર્ણાંક i, neg;
  unsigned long long x;

  neg = 0;
  જો (sgn && xx < 0) {
    neg = 1;
    x = -xx;
  } નહીંતર {
    x = xx;
  }

  i = 0;
  કરો {
    buf[i++] = digits[x % base];
  } જ્યારે ((x /= base) != 0);
  જો (neg)
    buf[i++] = '-';

  જ્યારે (--i >= 0)
    putc(fd, buf[i]);
}

સ્થિર શૂન્ય
printptr(પૂર્ણાંક fd, uint64 x)
{
  પૂર્ણાંક i;
  putc(fd, '0');
  putc(fd, 'x');
  માટે (i = 0; i < (કદ(uint64) * 2); i++, x <<= 4)
    putc(fd, digits[x >> (કદ(uint64) * 8 - 4)]);
}

// Print to the given fd. Only understands %d, %x, %p, %c, %s.
શૂન્ય
vprintf(પૂર્ણાંક fd, અચલ અક્ષર *fmt, va_list ap)
{
  અક્ષર *s;
  પૂર્ણાંક c0, c1, c2, i, state;

  state = 0;
  માટે (i = 0; fmt[i]; i++) {
    c0 = fmt[i] & 0xff;
    જો (state == 0) {
      જો (c0 == '%') {
        state = '%';
      } નહીંતર {
        putc(fd, c0);
      }
    } નહીંતર જો (state == '%') {
      c1 = c2 = 0;
      જો (c0)
        c1 = fmt[i + 1] & 0xff;
      જો (c1)
        c2 = fmt[i + 2] & 0xff;
      જો (c0 == 'd') {
        printint(fd, va_arg(ap, પૂર્ણાંક), 10, 1);
      } નહીંતર જો (c0 == 'l' && c1 == 'd') {
        printint(fd, va_arg(ap, uint64), 10, 1);
        i += 1;
      } નહીંતર જો (c0 == 'l' && c1 == 'l' && c2 == 'd') {
        printint(fd, va_arg(ap, uint64), 10, 1);
        i += 2;
      } નહીંતર જો (c0 == 'u') {
        printint(fd, va_arg(ap, uint32), 10, 0);
      } નહીંતર જો (c0 == 'l' && c1 == 'u') {
        printint(fd, va_arg(ap, uint64), 10, 0);
        i += 1;
      } નહીંતર જો (c0 == 'l' && c1 == 'l' && c2 == 'u') {
        printint(fd, va_arg(ap, uint64), 10, 0);
        i += 2;
      } નહીંતર જો (c0 == 'x') {
        printint(fd, va_arg(ap, uint32), 16, 0);
      } નહીંતર જો (c0 == 'l' && c1 == 'x') {
        printint(fd, va_arg(ap, uint64), 16, 0);
        i += 1;
      } નહીંતર જો (c0 == 'l' && c1 == 'l' && c2 == 'x') {
        printint(fd, va_arg(ap, uint64), 16, 0);
        i += 2;
      } નહીંતર જો (c0 == 'p') {
        printptr(fd, va_arg(ap, uint64));
      } નહીંતર જો (c0 == 'c') {
        putc(fd, va_arg(ap, uint32));
      } નહીંતર જો (c0 == 's') {
        જો ((s = va_arg(ap, અક્ષર *)) == 0)
          s = "(null)";
        માટે (; *s; s++)
          putc(fd, *s);
      } નહીંતર જો (c0 == '%') {
        putc(fd, '%');
      } નહીંતર {
        // Unknown % sequence.  Print it to draw attention.
        putc(fd, '%');
        putc(fd, c0);
      }

      state = 0;
    }
  }
}

શૂન્ય
fprintf(પૂર્ણાંક fd, અચલ અક્ષર *fmt, ...)
{
  va_list ap;

  va_start(ap, fmt);
  vprintf(fd, fmt, ap);
}

શૂન્ય
printf(અચલ અક્ષર *fmt, ...)
{
  va_list ap;

  va_start(ap, fmt);
  vprintf(1, fmt, ap);
}
