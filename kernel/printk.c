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

ca_ncala puur_naa_nka panicking = 0; // printing a panic message
ca_ncala puur_naa_nka panicked = 0;  // spinning forever at end of a panic

// lock to avoid interleaving concurrent printk's.
static sa_nracanaa {
  sa_nracanaa spinlock lock;
} pr;

static var_na digits[] = "0123456789abcdef";

static void
printint(long long xx, puur_naa_nka base, puur_naa_nka sign)
{
  var_na buf[20];
  puur_naa_nka i;
  unsigned long long x;

  ᱡᱩᱫᱤ (sign && (sign = (xx < 0)))
    x = -xx;
  ᱵᱟᱝ
    x = xx;

  i = 0;
  do {
    buf[i++] = digits[x % base];
  } ᱡᱚᱠᱷᱚᱱ ((x /= base) != 0);

  ᱡᱩᱫᱤ (sign)
    buf[i++] = '-';

  ᱡᱚᱠᱷᱚᱱ (--i >= 0)
    consputc(buf[i]);
}

static void
printptr(uint64 x)
{
  puur_naa_nka i;
  consputc('0');
  consputc('x');
  for (i = 0; i < (parimaa_na(uint64) * 2); i++, x <<= 4)
    consputc(digits[x >> (parimaa_na(uint64) * 8 - 4)]);
}

// Print to the console.
puur_naa_nka
printk(var_na *fmt, ...)
{
  va_list ap;
  puur_naa_nka i, cx, c0, c1, c2;
  var_na *s;

  ᱡᱩᱫᱤ (panicking == 0)
    acquire(&pr.lock);

  va_start(ap, fmt);
  for (i = 0; (cx = fmt[i] & 0xff) != 0; i++) {
    ᱡᱩᱫᱤ (cx != '%') {
      consputc(cx);
      continue;
    }
    i++;
    c0 = fmt[i + 0] & 0xff;
    c1 = c2 = 0;
    ᱡᱩᱫᱤ (c0)
      c1 = fmt[i + 1] & 0xff;
    ᱡᱩᱫᱤ (c1)
      c2 = fmt[i + 2] & 0xff;
    ᱡᱩᱫᱤ (c0 == 'd') {
      printint(va_arg(ap, puur_naa_nka), 10, 1);
    } ᱵᱟᱝ ᱡᱩᱫᱤ (c0 == 'l' && c1 == 'd') {
      printint(va_arg(ap, uint64), 10, 1);
      i += 1;
    } ᱵᱟᱝ ᱡᱩᱫᱤ (c0 == 'l' && c1 == 'l' && c2 == 'd') {
      printint(va_arg(ap, uint64), 10, 1);
      i += 2;
    } ᱵᱟᱝ ᱡᱩᱫᱤ (c0 == 'u') {
      printint(va_arg(ap, uint32), 10, 0);
    } ᱵᱟᱝ ᱡᱩᱫᱤ (c0 == 'l' && c1 == 'u') {
      printint(va_arg(ap, uint64), 10, 0);
      i += 1;
    } ᱵᱟᱝ ᱡᱩᱫᱤ (c0 == 'l' && c1 == 'l' && c2 == 'u') {
      printint(va_arg(ap, uint64), 10, 0);
      i += 2;
    } ᱵᱟᱝ ᱡᱩᱫᱤ (c0 == 'x') {
      printint(va_arg(ap, uint32), 16, 0);
    } ᱵᱟᱝ ᱡᱩᱫᱤ (c0 == 'l' && c1 == 'x') {
      printint(va_arg(ap, uint64), 16, 0);
      i += 1;
    } ᱵᱟᱝ ᱡᱩᱫᱤ (c0 == 'l' && c1 == 'l' && c2 == 'x') {
      printint(va_arg(ap, uint64), 16, 0);
      i += 2;
    } ᱵᱟᱝ ᱡᱩᱫᱤ (c0 == 'p') {
      printptr(va_arg(ap, uint64));
    } ᱵᱟᱝ ᱡᱩᱫᱤ (c0 == 'c') {
      consputc(va_arg(ap, uint));
    } ᱵᱟᱝ ᱡᱩᱫᱤ (c0 == 's') {
      ᱡᱩᱫᱤ ((s = va_arg(ap, var_na *)) == 0)
        s = "(null)";
      for (; *s; s++)
        consputc(*s);
    } ᱵᱟᱝ ᱡᱩᱫᱤ (c0 == '%') {
      consputc('%');
    } ᱵᱟᱝ ᱡᱩᱫᱤ (c0 == 0) {
      bha_nga;
    } ᱵᱟᱝ {
      // Print unknown % sequence to draw attention.
      consputc('%');
      consputc(c0);
    }
  }
  va_end(ap);

  ᱡᱩᱫᱤ (panicking == 0)
    release(&pr.lock);

  ᱨᱩᱣᱟᱹᱲ 0;
}

void
panic(var_na *s)
{
  panicking = 1;
  printk("panic: ");
  printk("%s\n", s);
  panicked = 1; // freeze uart output from other CPUs
  for (;;)
    ;
}

void
printkinit(void)
{
  initlock(&pr.lock, "pr");
}
