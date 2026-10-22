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

volatile số_nguyên panicking = 0; // printing a panic message
volatile số_nguyên panicked = 0;  // spinning forever at end of a panic

// lock to avoid interleaving concurrent printk's.
static struct {
  struct spinlock lock;
} pr;

static char digits[] = "0123456789abcdef";

static void
printint(long long xx, số_nguyên base, số_nguyên sign)
{
  char buf[20];
  số_nguyên i;
  unsigned long long x;

  nếu (sign && (sign = (xx < 0)))
    x = -xx;
  không_thì
    x = xx;

  i = 0;
  do {
    buf[i++] = digits[x % base];
  } trong_khi ((x /= base) != 0);

  nếu (sign)
    buf[i++] = '-';

  trong_khi (--i >= 0)
    consputc(buf[i]);
}

static void
printptr(uint64 x)
{
  số_nguyên i;
  consputc('0');
  consputc('x');
  cho (i = 0; i < (sizeof(uint64) * 2); i++, x <<= 4)
    consputc(digits[x >> (sizeof(uint64) * 8 - 4)]);
}

// Print to the console.
số_nguyên
printk(char *fmt, ...)
{
  va_list ap;
  số_nguyên i, cx, c0, c1, c2;
  char *s;

  nếu (panicking == 0)
    acquire(&pr.lock);

  va_start(ap, fmt);
  cho (i = 0; (cx = fmt[i] & 0xff) != 0; i++) {
    nếu (cx != '%') {
      consputc(cx);
      continue;
    }
    i++;
    c0 = fmt[i + 0] & 0xff;
    c1 = c2 = 0;
    nếu (c0)
      c1 = fmt[i + 1] & 0xff;
    nếu (c1)
      c2 = fmt[i + 2] & 0xff;
    nếu (c0 == 'd') {
      printint(va_arg(ap, số_nguyên), 10, 1);
    } không_thì nếu (c0 == 'l' && c1 == 'd') {
      printint(va_arg(ap, uint64), 10, 1);
      i += 1;
    } không_thì nếu (c0 == 'l' && c1 == 'l' && c2 == 'd') {
      printint(va_arg(ap, uint64), 10, 1);
      i += 2;
    } không_thì nếu (c0 == 'u') {
      printint(va_arg(ap, uint32), 10, 0);
    } không_thì nếu (c0 == 'l' && c1 == 'u') {
      printint(va_arg(ap, uint64), 10, 0);
      i += 1;
    } không_thì nếu (c0 == 'l' && c1 == 'l' && c2 == 'u') {
      printint(va_arg(ap, uint64), 10, 0);
      i += 2;
    } không_thì nếu (c0 == 'x') {
      printint(va_arg(ap, uint32), 16, 0);
    } không_thì nếu (c0 == 'l' && c1 == 'x') {
      printint(va_arg(ap, uint64), 16, 0);
      i += 1;
    } không_thì nếu (c0 == 'l' && c1 == 'l' && c2 == 'x') {
      printint(va_arg(ap, uint64), 16, 0);
      i += 2;
    } không_thì nếu (c0 == 'p') {
      printptr(va_arg(ap, uint64));
    } không_thì nếu (c0 == 'c') {
      consputc(va_arg(ap, uint));
    } không_thì nếu (c0 == 's') {
      nếu ((s = va_arg(ap, char *)) == 0)
        s = "(null)";
      cho (; *s; s++)
        consputc(*s);
    } không_thì nếu (c0 == '%') {
      consputc('%');
    } không_thì nếu (c0 == 0) {
      break;
    } không_thì {
      // Print unknown % sequence to draw attention.
      consputc('%');
      consputc(c0);
    }
  }
  va_end(ap);

  nếu (panicking == 0)
    release(&pr.lock);

  trả_về 0;
}

void
panic(char *s)
{
  panicking = 1;
  printk("panic: ");
  printk("%s\n", s);
  panicked = 1; // freeze uart output from other CPUs
  cho (;;)
    ;
}

void
printkinit(void)
{
  initlock(&pr.lock, "pr");
}
