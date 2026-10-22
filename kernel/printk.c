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

volatile ціле panicking = 0; // printing a panic message
volatile ціле panicked = 0;  // spinning forever at end of a panic

// lock to avoid interleaving concurrent printk's.
static struct {
  struct spinlock lock;
} pr;

static char digits[] = "0123456789abcdef";

static void
printint(long long xx, ціле base, ціле sign)
{
  char buf[20];
  ціле i;
  unsigned long long x;

  якщо (sign && (sign = (xx < 0)))
    x = -xx;
  інакше
    x = xx;

  i = 0;
  do {
    buf[i++] = digits[x % base];
  } поки ((x /= base) != 0);

  якщо (sign)
    buf[i++] = '-';

  поки (--i >= 0)
    consputc(buf[i]);
}

static void
printptr(uint64 x)
{
  ціле i;
  consputc('0');
  consputc('x');
  для (i = 0; i < (sizeof(uint64) * 2); i++, x <<= 4)
    consputc(digits[x >> (sizeof(uint64) * 8 - 4)]);
}

// Print to the console.
ціле
printk(char *fmt, ...)
{
  va_list ap;
  ціле i, cx, c0, c1, c2;
  char *s;

  якщо (panicking == 0)
    acquire(&pr.lock);

  va_start(ap, fmt);
  для (i = 0; (cx = fmt[i] & 0xff) != 0; i++) {
    якщо (cx != '%') {
      consputc(cx);
      continue;
    }
    i++;
    c0 = fmt[i + 0] & 0xff;
    c1 = c2 = 0;
    якщо (c0)
      c1 = fmt[i + 1] & 0xff;
    якщо (c1)
      c2 = fmt[i + 2] & 0xff;
    якщо (c0 == 'd') {
      printint(va_arg(ap, ціле), 10, 1);
    } інакше якщо (c0 == 'l' && c1 == 'd') {
      printint(va_arg(ap, uint64), 10, 1);
      i += 1;
    } інакше якщо (c0 == 'l' && c1 == 'l' && c2 == 'd') {
      printint(va_arg(ap, uint64), 10, 1);
      i += 2;
    } інакше якщо (c0 == 'u') {
      printint(va_arg(ap, uint32), 10, 0);
    } інакше якщо (c0 == 'l' && c1 == 'u') {
      printint(va_arg(ap, uint64), 10, 0);
      i += 1;
    } інакше якщо (c0 == 'l' && c1 == 'l' && c2 == 'u') {
      printint(va_arg(ap, uint64), 10, 0);
      i += 2;
    } інакше якщо (c0 == 'x') {
      printint(va_arg(ap, uint32), 16, 0);
    } інакше якщо (c0 == 'l' && c1 == 'x') {
      printint(va_arg(ap, uint64), 16, 0);
      i += 1;
    } інакше якщо (c0 == 'l' && c1 == 'l' && c2 == 'x') {
      printint(va_arg(ap, uint64), 16, 0);
      i += 2;
    } інакше якщо (c0 == 'p') {
      printptr(va_arg(ap, uint64));
    } інакше якщо (c0 == 'c') {
      consputc(va_arg(ap, uint));
    } інакше якщо (c0 == 's') {
      якщо ((s = va_arg(ap, char *)) == 0)
        s = "(null)";
      для (; *s; s++)
        consputc(*s);
    } інакше якщо (c0 == '%') {
      consputc('%');
    } інакше якщо (c0 == 0) {
      break;
    } інакше {
      // Print unknown % sequence to draw attention.
      consputc('%');
      consputc(c0);
    }
  }
  va_end(ap);

  якщо (panicking == 0)
    release(&pr.lock);

  повернути 0;
}

void
panic(char *s)
{
  panicking = 1;
  printk("panic: ");
  printk("%s\n", s);
  panicked = 1; // freeze uart output from other CPUs
  для (;;)
    ;
}

void
printkinit(void)
{
  initlock(&pr.lock, "pr");
}
