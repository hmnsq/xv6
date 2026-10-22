//
// formatted console output -- printk, panic.
//

#include <stdarg.h>

#include "types.h"
#include "param.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "fs.h"
#include "file.h"
#include "memlayout.h"
#include "riscv.h"
#include "defs.h"
#include "proc.h"

ca_ncala ཧྲིལ panicking = 0; // printing a panic message
ca_ncala ཧྲིལ panicked = 0;  // spinning forever at end of a panic

// lock to avoid interleaving concurrent printk's.
static sa_nracanaa {
  sa_nracanaa spinlock lock;
} pr;

static var_na digits[] = "0123456789abcdef";

static void
printint(long long xx, ཧྲིལ base, ཧྲིལ sign)
{
  var_na buf[20];
  ཧྲིལ i;
  unsigned long long x;

  གལ་ཏེ (sign && (sign = (xx < 0)))
    x = -xx;
  དེ་མིན
    x = xx;

  i = 0;
  do {
    buf[i++] = digits[x % base];
  } སྐབས ((x /= base) != 0);

  གལ་ཏེ (sign)
    buf[i++] = '-';

  སྐབས (--i >= 0)
    consputc(buf[i]);
}

static void
printptr(uint64 x)
{
  ཧྲིལ i;
  consputc('0');
  consputc('x');
  ཆེད་དུ (i = 0; i < (parimaa_na(uint64) * 2); i++, x <<= 4)
    consputc(digits[x >> (parimaa_na(uint64) * 8 - 4)]);
}

// Print to the console.
ཧྲིལ
printk(var_na *fmt, ...)
{
  va_list ap;
  ཧྲིལ i, cx, c0, c1, c2;
  var_na *s;

  གལ་ཏེ (panicking == 0)
    acquire(&pr.lock);

  va_start(ap, fmt);
  ཆེད་དུ (i = 0; (cx = fmt[i] & 0xff) != 0; i++) {
    གལ་ཏེ (cx != '%') {
      consputc(cx);
      continue;
    }
    i++;
    c0 = fmt[i + 0] & 0xff;
    c1 = c2 = 0;
    གལ་ཏེ (c0)
      c1 = fmt[i + 1] & 0xff;
    གལ་ཏེ (c1)
      c2 = fmt[i + 2] & 0xff;
    གལ་ཏེ (c0 == 'd') {
      printint(va_arg(ap, ཧྲིལ), 10, 1);
    } དེ་མིན གལ་ཏེ (c0 == 'l' && c1 == 'd') {
      printint(va_arg(ap, uint64), 10, 1);
      i += 1;
    } དེ་མིན གལ་ཏེ (c0 == 'l' && c1 == 'l' && c2 == 'd') {
      printint(va_arg(ap, uint64), 10, 1);
      i += 2;
    } དེ་མིན གལ་ཏེ (c0 == 'u') {
      printint(va_arg(ap, uint32), 10, 0);
    } དེ་མིན གལ་ཏེ (c0 == 'l' && c1 == 'u') {
      printint(va_arg(ap, uint64), 10, 0);
      i += 1;
    } དེ་མིན གལ་ཏེ (c0 == 'l' && c1 == 'l' && c2 == 'u') {
      printint(va_arg(ap, uint64), 10, 0);
      i += 2;
    } དེ་མིན གལ་ཏེ (c0 == 'x') {
      printint(va_arg(ap, uint32), 16, 0);
    } དེ་མིན གལ་ཏེ (c0 == 'l' && c1 == 'x') {
      printint(va_arg(ap, uint64), 16, 0);
      i += 1;
    } དེ་མིན གལ་ཏེ (c0 == 'l' && c1 == 'l' && c2 == 'x') {
      printint(va_arg(ap, uint64), 16, 0);
      i += 2;
    } དེ་མིན གལ་ཏེ (c0 == 'p') {
      printptr(va_arg(ap, uint64));
    } དེ་མིན གལ་ཏེ (c0 == 'c') {
      consputc(va_arg(ap, uint));
    } དེ་མིན གལ་ཏེ (c0 == 's') {
      གལ་ཏེ ((s = va_arg(ap, var_na *)) == 0)
        s = "(null)";
      ཆེད་དུ (; *s; s++)
        consputc(*s);
    } དེ་མིན གལ་ཏེ (c0 == '%') {
      consputc('%');
    } དེ་མིན གལ་ཏེ (c0 == 0) {
      bha_nga;
    } དེ་མིན {
      // Print unknown % sequence to draw attention.
      consputc('%');
      consputc(c0);
    }
  }
  va_end(ap);

  གལ་ཏེ (panicking == 0)
    release(&pr.lock);

  ཕྱིར 0;
}

void
panic(var_na *s)
{
  panicking = 1;
  printk("panic: ");
  printk("%s\n", s);
  panicked = 1; // freeze uart output from other CPUs
  ཆེད་དུ (;;)
    ;
}

void
printkinit(void)
{
  initlock(&pr.lock, "pr");
}
