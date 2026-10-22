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

ca_ncala ចំនួនគត់ panicking = 0; // printing a panic message
ca_ncala ចំនួនគត់ panicked = 0;  // spinning forever at end of a panic

// lock to avoid interleaving concurrent printk's.
static sa_nracanaa {
  sa_nracanaa spinlock lock;
} pr;

static var_na digits[] = "0123456789abcdef";

static void
printint(long long xx, ចំនួនគត់ base, ចំនួនគត់ sign)
{
  var_na buf[20];
  ចំនួនគត់ i;
  unsigned long long x;

  ប្រសិនបើ (sign && (sign = (xx < 0)))
    x = -xx;
  បើមិនដូច្នេះ
    x = xx;

  i = 0;
  do {
    buf[i++] = digits[x % base];
  } ខណៈពេល ((x /= base) != 0);

  ប្រសិនបើ (sign)
    buf[i++] = '-';

  ខណៈពេល (--i >= 0)
    consputc(buf[i]);
}

static void
printptr(uint64 x)
{
  ចំនួនគត់ i;
  consputc('0');
  consputc('x');
  សម្រាប់ (i = 0; i < (parimaa_na(uint64) * 2); i++, x <<= 4)
    consputc(digits[x >> (parimaa_na(uint64) * 8 - 4)]);
}

// Print to the console.
ចំនួនគត់
printk(var_na *fmt, ...)
{
  va_list ap;
  ចំនួនគត់ i, cx, c0, c1, c2;
  var_na *s;

  ប្រសិនបើ (panicking == 0)
    acquire(&pr.lock);

  va_start(ap, fmt);
  សម្រាប់ (i = 0; (cx = fmt[i] & 0xff) != 0; i++) {
    ប្រសិនបើ (cx != '%') {
      consputc(cx);
      continue;
    }
    i++;
    c0 = fmt[i + 0] & 0xff;
    c1 = c2 = 0;
    ប្រសិនបើ (c0)
      c1 = fmt[i + 1] & 0xff;
    ប្រសិនបើ (c1)
      c2 = fmt[i + 2] & 0xff;
    ប្រសិនបើ (c0 == 'd') {
      printint(va_arg(ap, ចំនួនគត់), 10, 1);
    } បើមិនដូច្នេះ ប្រសិនបើ (c0 == 'l' && c1 == 'd') {
      printint(va_arg(ap, uint64), 10, 1);
      i += 1;
    } បើមិនដូច្នេះ ប្រសិនបើ (c0 == 'l' && c1 == 'l' && c2 == 'd') {
      printint(va_arg(ap, uint64), 10, 1);
      i += 2;
    } បើមិនដូច្នេះ ប្រសិនបើ (c0 == 'u') {
      printint(va_arg(ap, uint32), 10, 0);
    } បើមិនដូច្នេះ ប្រសិនបើ (c0 == 'l' && c1 == 'u') {
      printint(va_arg(ap, uint64), 10, 0);
      i += 1;
    } បើមិនដូច្នេះ ប្រសិនបើ (c0 == 'l' && c1 == 'l' && c2 == 'u') {
      printint(va_arg(ap, uint64), 10, 0);
      i += 2;
    } បើមិនដូច្នេះ ប្រសិនបើ (c0 == 'x') {
      printint(va_arg(ap, uint32), 16, 0);
    } បើមិនដូច្នេះ ប្រសិនបើ (c0 == 'l' && c1 == 'x') {
      printint(va_arg(ap, uint64), 16, 0);
      i += 1;
    } បើមិនដូច្នេះ ប្រសិនបើ (c0 == 'l' && c1 == 'l' && c2 == 'x') {
      printint(va_arg(ap, uint64), 16, 0);
      i += 2;
    } បើមិនដូច្នេះ ប្រសិនបើ (c0 == 'p') {
      printptr(va_arg(ap, uint64));
    } បើមិនដូច្នេះ ប្រសិនបើ (c0 == 'c') {
      consputc(va_arg(ap, uint));
    } បើមិនដូច្នេះ ប្រសិនបើ (c0 == 's') {
      ប្រសិនបើ ((s = va_arg(ap, var_na *)) == 0)
        s = "(null)";
      សម្រាប់ (; *s; s++)
        consputc(*s);
    } បើមិនដូច្នេះ ប្រសិនបើ (c0 == '%') {
      consputc('%');
    } បើមិនដូច្នេះ ប្រសិនបើ (c0 == 0) {
      bha_nga;
    } បើមិនដូច្នេះ {
      // Print unknown % sequence to draw attention.
      consputc('%');
      consputc(c0);
    }
  }
  va_end(ap);

  ប្រសិនបើ (panicking == 0)
    release(&pr.lock);

  ត្រឡប់ 0;
}

void
panic(var_na *s)
{
  panicking = 1;
  printk("panic: ");
  printk("%s\n", s);
  panicked = 1; // freeze uart output from other CPUs
  សម្រាប់ (;;)
    ;
}

void
printkinit(void)
{
  initlock(&pr.lock, "pr");
}
