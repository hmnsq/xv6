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

volatile цяло panicking = 0; // printing a panic message
volatile цяло panicked = 0;  // spinning forever at end of a panic

// lock to avoid interleaving concurrent printk's.
static struct {
  struct spinlock lock;
} pr;

static char digits[] = "0123456789abcdef";

static void
printint(long long xx, цяло base, цяло sign)
{
  char buf[20];
  цяло i;
  unsigned long long x;

  ако (sign && (sign = (xx < 0)))
    x = -xx;
  иначе
    x = xx;

  i = 0;
  do {
    buf[i++] = digits[x % base];
  } докато ((x /= base) != 0);

  ако (sign)
    buf[i++] = '-';

  докато (--i >= 0)
    consputc(buf[i]);
}

static void
printptr(uint64 x)
{
  цяло i;
  consputc('0');
  consputc('x');
  за (i = 0; i < (sizeof(uint64) * 2); i++, x <<= 4)
    consputc(digits[x >> (sizeof(uint64) * 8 - 4)]);
}

// Print to the console.
цяло
printk(char *fmt, ...)
{
  va_list ap;
  цяло i, cx, c0, c1, c2;
  char *s;

  ако (panicking == 0)
    acquire(&pr.lock);

  va_start(ap, fmt);
  за (i = 0; (cx = fmt[i] & 0xff) != 0; i++) {
    ако (cx != '%') {
      consputc(cx);
      continue;
    }
    i++;
    c0 = fmt[i + 0] & 0xff;
    c1 = c2 = 0;
    ако (c0)
      c1 = fmt[i + 1] & 0xff;
    ако (c1)
      c2 = fmt[i + 2] & 0xff;
    ако (c0 == 'd') {
      printint(va_arg(ap, цяло), 10, 1);
    } иначе ако (c0 == 'l' && c1 == 'd') {
      printint(va_arg(ap, uint64), 10, 1);
      i += 1;
    } иначе ако (c0 == 'l' && c1 == 'l' && c2 == 'd') {
      printint(va_arg(ap, uint64), 10, 1);
      i += 2;
    } иначе ако (c0 == 'u') {
      printint(va_arg(ap, uint32), 10, 0);
    } иначе ако (c0 == 'l' && c1 == 'u') {
      printint(va_arg(ap, uint64), 10, 0);
      i += 1;
    } иначе ако (c0 == 'l' && c1 == 'l' && c2 == 'u') {
      printint(va_arg(ap, uint64), 10, 0);
      i += 2;
    } иначе ако (c0 == 'x') {
      printint(va_arg(ap, uint32), 16, 0);
    } иначе ако (c0 == 'l' && c1 == 'x') {
      printint(va_arg(ap, uint64), 16, 0);
      i += 1;
    } иначе ако (c0 == 'l' && c1 == 'l' && c2 == 'x') {
      printint(va_arg(ap, uint64), 16, 0);
      i += 2;
    } иначе ако (c0 == 'p') {
      printptr(va_arg(ap, uint64));
    } иначе ако (c0 == 'c') {
      consputc(va_arg(ap, uint));
    } иначе ако (c0 == 's') {
      ако ((s = va_arg(ap, char *)) == 0)
        s = "(null)";
      за (; *s; s++)
        consputc(*s);
    } иначе ако (c0 == '%') {
      consputc('%');
    } иначе ако (c0 == 0) {
      break;
    } иначе {
      // Print unknown % sequence to draw attention.
      consputc('%');
      consputc(c0);
    }
  }
  va_end(ap);

  ако (panicking == 0)
    release(&pr.lock);

  върни 0;
}

void
panic(char *s)
{
  panicking = 1;
  printk("panic: ");
  printk("%s\n", s);
  panicked = 1; // freeze uart output from other CPUs
  за (;;)
    ;
}

void
printkinit(void)
{
  initlock(&pr.lock, "pr");
}
