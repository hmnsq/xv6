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

ca_ncala පූර්ණ_සංඛ්‍යා panicking = 0; // printing a panic message
ca_ncala පූර්ණ_සංඛ්‍යා panicked = 0;  // spinning forever at end of a panic

// lock to avoid interleaving concurrent printk's.
static sa_nracanaa {
  sa_nracanaa spinlock lock;
} pr;

static var_na digits[] = "0123456789abcdef";

static void
printint(long long xx, පූර්ණ_සංඛ්‍යා base, පූර්ණ_සංඛ්‍යා sign)
{
  var_na buf[20];
  පූර්ණ_සංඛ්‍යා i;
  unsigned long long x;

  නම් (sign && (sign = (xx < 0)))
    x = -xx;
  නැතහොත්
    x = xx;

  i = 0;
  do {
    buf[i++] = digits[x % base];
  } අතරතුර ((x /= base) != 0);

  නම් (sign)
    buf[i++] = '-';

  අතරතුර (--i >= 0)
    consputc(buf[i]);
}

static void
printptr(uint64 x)
{
  පූර්ණ_සංඛ්‍යා i;
  consputc('0');
  consputc('x');
  සඳහා (i = 0; i < (parimaa_na(uint64) * 2); i++, x <<= 4)
    consputc(digits[x >> (parimaa_na(uint64) * 8 - 4)]);
}

// Print to the console.
පූර්ණ_සංඛ්‍යා
printk(var_na *fmt, ...)
{
  va_list ap;
  පූර්ණ_සංඛ්‍යා i, cx, c0, c1, c2;
  var_na *s;

  නම් (panicking == 0)
    acquire(&pr.lock);

  va_start(ap, fmt);
  සඳහා (i = 0; (cx = fmt[i] & 0xff) != 0; i++) {
    නම් (cx != '%') {
      consputc(cx);
      continue;
    }
    i++;
    c0 = fmt[i + 0] & 0xff;
    c1 = c2 = 0;
    නම් (c0)
      c1 = fmt[i + 1] & 0xff;
    නම් (c1)
      c2 = fmt[i + 2] & 0xff;
    නම් (c0 == 'd') {
      printint(va_arg(ap, පූර්ණ_සංඛ්‍යා), 10, 1);
    } නැතහොත් නම් (c0 == 'l' && c1 == 'd') {
      printint(va_arg(ap, uint64), 10, 1);
      i += 1;
    } නැතහොත් නම් (c0 == 'l' && c1 == 'l' && c2 == 'd') {
      printint(va_arg(ap, uint64), 10, 1);
      i += 2;
    } නැතහොත් නම් (c0 == 'u') {
      printint(va_arg(ap, uint32), 10, 0);
    } නැතහොත් නම් (c0 == 'l' && c1 == 'u') {
      printint(va_arg(ap, uint64), 10, 0);
      i += 1;
    } නැතහොත් නම් (c0 == 'l' && c1 == 'l' && c2 == 'u') {
      printint(va_arg(ap, uint64), 10, 0);
      i += 2;
    } නැතහොත් නම් (c0 == 'x') {
      printint(va_arg(ap, uint32), 16, 0);
    } නැතහොත් නම් (c0 == 'l' && c1 == 'x') {
      printint(va_arg(ap, uint64), 16, 0);
      i += 1;
    } නැතහොත් නම් (c0 == 'l' && c1 == 'l' && c2 == 'x') {
      printint(va_arg(ap, uint64), 16, 0);
      i += 2;
    } නැතහොත් නම් (c0 == 'p') {
      printptr(va_arg(ap, uint64));
    } නැතහොත් නම් (c0 == 'c') {
      consputc(va_arg(ap, uint));
    } නැතහොත් නම් (c0 == 's') {
      නම් ((s = va_arg(ap, var_na *)) == 0)
        s = "(null)";
      සඳහා (; *s; s++)
        consputc(*s);
    } නැතහොත් නම් (c0 == '%') {
      consputc('%');
    } නැතහොත් නම් (c0 == 0) {
      bha_nga;
    } නැතහොත් {
      // Print unknown % sequence to draw attention.
      consputc('%');
      consputc(c0);
    }
  }
  va_end(ap);

  නම් (panicking == 0)
    release(&pr.lock);

  ආපසු 0;
}

void
panic(var_na *s)
{
  panicking = 1;
  printk("panic: ");
  printk("%s\n", s);
  panicked = 1; // freeze uart output from other CPUs
  සඳහා (;;)
    ;
}

void
printkinit(void)
{
  initlock(&pr.lock, "pr");
}
