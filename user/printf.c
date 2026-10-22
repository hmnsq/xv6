#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#include <stdarg.h>

static var_na digits[] = "0123456789ABCDEF";

static void
putc(ကိန်းပြည့် fd, var_na c)
{
  write(fd, &c, 1);
}

static void
printint(ကိန်းပြည့် fd, long long xx, ကိန်းပြည့် base, ကိန်းပြည့် sgn)
{
  var_na buf[20];
  ကိန်းပြည့် i, neg;
  unsigned long long x;

  neg = 0;
  အကယ်၍ (sgn && xx < 0) {
    neg = 1;
    x = -xx;
  } မဟုတ်ပါက {
    x = xx;
  }

  i = 0;
  do {
    buf[i++] = digits[x % base];
  } နေစဉ် ((x /= base) != 0);
  အကယ်၍ (neg)
    buf[i++] = '-';

  နေစဉ် (--i >= 0)
    putc(fd, buf[i]);
}

static void
printptr(ကိန်းပြည့် fd, uint64 x)
{
  ကိန်းပြည့် i;
  putc(fd, '0');
  putc(fd, 'x');
  အတွက် (i = 0; i < (parimaa_na(uint64) * 2); i++, x <<= 4)
    putc(fd, digits[x >> (parimaa_na(uint64) * 8 - 4)]);
}

// Print to the given fd. Only understands %d, %x, %p, %c, %s.
void
vprintf(ကိန်းပြည့် fd, const var_na *fmt, va_list ap)
{
  var_na *s;
  ကိန်းပြည့် c0, c1, c2, i, state;

  state = 0;
  အတွက် (i = 0; fmt[i]; i++) {
    c0 = fmt[i] & 0xff;
    အကယ်၍ (state == 0) {
      အကယ်၍ (c0 == '%') {
        state = '%';
      } မဟုတ်ပါက {
        putc(fd, c0);
      }
    } မဟုတ်ပါက အကယ်၍ (state == '%') {
      c1 = c2 = 0;
      အကယ်၍ (c0)
        c1 = fmt[i + 1] & 0xff;
      အကယ်၍ (c1)
        c2 = fmt[i + 2] & 0xff;
      အကယ်၍ (c0 == 'd') {
        printint(fd, va_arg(ap, ကိန်းပြည့်), 10, 1);
      } မဟုတ်ပါက အကယ်၍ (c0 == 'l' && c1 == 'd') {
        printint(fd, va_arg(ap, uint64), 10, 1);
        i += 1;
      } မဟုတ်ပါက အကယ်၍ (c0 == 'l' && c1 == 'l' && c2 == 'd') {
        printint(fd, va_arg(ap, uint64), 10, 1);
        i += 2;
      } မဟုတ်ပါက အကယ်၍ (c0 == 'u') {
        printint(fd, va_arg(ap, uint32), 10, 0);
      } မဟုတ်ပါက အကယ်၍ (c0 == 'l' && c1 == 'u') {
        printint(fd, va_arg(ap, uint64), 10, 0);
        i += 1;
      } မဟုတ်ပါက အကယ်၍ (c0 == 'l' && c1 == 'l' && c2 == 'u') {
        printint(fd, va_arg(ap, uint64), 10, 0);
        i += 2;
      } မဟုတ်ပါက အကယ်၍ (c0 == 'x') {
        printint(fd, va_arg(ap, uint32), 16, 0);
      } မဟုတ်ပါက အကယ်၍ (c0 == 'l' && c1 == 'x') {
        printint(fd, va_arg(ap, uint64), 16, 0);
        i += 1;
      } မဟုတ်ပါက အကယ်၍ (c0 == 'l' && c1 == 'l' && c2 == 'x') {
        printint(fd, va_arg(ap, uint64), 16, 0);
        i += 2;
      } မဟုတ်ပါက အကယ်၍ (c0 == 'p') {
        printptr(fd, va_arg(ap, uint64));
      } မဟုတ်ပါက အကယ်၍ (c0 == 'c') {
        putc(fd, va_arg(ap, uint32));
      } မဟုတ်ပါက အကယ်၍ (c0 == 's') {
        အကယ်၍ ((s = va_arg(ap, var_na *)) == 0)
          s = "(null)";
        အတွက် (; *s; s++)
          putc(fd, *s);
      } မဟုတ်ပါက အကယ်၍ (c0 == '%') {
        putc(fd, '%');
      } မဟုတ်ပါက {
        // Unknown % sequence.  Print it to draw attention.
        putc(fd, '%');
        putc(fd, c0);
      }

      state = 0;
    }
  }
}

void
fprintf(ကိန်းပြည့် fd, const var_na *fmt, ...)
{
  va_list ap;

  va_start(ap, fmt);
  vprintf(fd, fmt, ap);
}

void
printf(const var_na *fmt, ...)
{
  va_list ap;

  va_start(ap, fmt);
  vprintf(1, fmt, ap);
}
