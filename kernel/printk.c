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

volatile 정수 panicking = 0; // printing a panic message
volatile 정수 panicked = 0;  // spinning forever at end of a panic

// lock to avoid interleaving concurrent printk's.
static struct {
  struct spinlock lock;
} pr;

static char digits[] = "0123456789abcdef";

static void
printint(long long xx, 정수 base, 정수 sign)
{
  char buf[20];
  정수 i;
  unsigned long long x;

  만약 (sign && (sign = (xx < 0)))
    x = -xx;
  아니면
    x = xx;

  i = 0;
  do {
    buf[i++] = digits[x % base];
  } 동안 ((x /= base) != 0);

  만약 (sign)
    buf[i++] = '-';

  동안 (--i >= 0)
    consputc(buf[i]);
}

static void
printptr(uint64 x)
{
  정수 i;
  consputc('0');
  consputc('x');
  위해 (i = 0; i < (sizeof(uint64) * 2); i++, x <<= 4)
    consputc(digits[x >> (sizeof(uint64) * 8 - 4)]);
}

// Print to the console.
정수
printk(char *fmt, ...)
{
  va_list ap;
  정수 i, cx, c0, c1, c2;
  char *s;

  만약 (panicking == 0)
    acquire(&pr.lock);

  va_start(ap, fmt);
  위해 (i = 0; (cx = fmt[i] & 0xff) != 0; i++) {
    만약 (cx != '%') {
      consputc(cx);
      continue;
    }
    i++;
    c0 = fmt[i + 0] & 0xff;
    c1 = c2 = 0;
    만약 (c0)
      c1 = fmt[i + 1] & 0xff;
    만약 (c1)
      c2 = fmt[i + 2] & 0xff;
    만약 (c0 == 'd') {
      printint(va_arg(ap, 정수), 10, 1);
    } 아니면 만약 (c0 == 'l' && c1 == 'd') {
      printint(va_arg(ap, uint64), 10, 1);
      i += 1;
    } 아니면 만약 (c0 == 'l' && c1 == 'l' && c2 == 'd') {
      printint(va_arg(ap, uint64), 10, 1);
      i += 2;
    } 아니면 만약 (c0 == 'u') {
      printint(va_arg(ap, uint32), 10, 0);
    } 아니면 만약 (c0 == 'l' && c1 == 'u') {
      printint(va_arg(ap, uint64), 10, 0);
      i += 1;
    } 아니면 만약 (c0 == 'l' && c1 == 'l' && c2 == 'u') {
      printint(va_arg(ap, uint64), 10, 0);
      i += 2;
    } 아니면 만약 (c0 == 'x') {
      printint(va_arg(ap, uint32), 16, 0);
    } 아니면 만약 (c0 == 'l' && c1 == 'x') {
      printint(va_arg(ap, uint64), 16, 0);
      i += 1;
    } 아니면 만약 (c0 == 'l' && c1 == 'l' && c2 == 'x') {
      printint(va_arg(ap, uint64), 16, 0);
      i += 2;
    } 아니면 만약 (c0 == 'p') {
      printptr(va_arg(ap, uint64));
    } 아니면 만약 (c0 == 'c') {
      consputc(va_arg(ap, uint));
    } 아니면 만약 (c0 == 's') {
      만약 ((s = va_arg(ap, char *)) == 0)
        s = "(null)";
      위해 (; *s; s++)
        consputc(*s);
    } 아니면 만약 (c0 == '%') {
      consputc('%');
    } 아니면 만약 (c0 == 0) {
      break;
    } 아니면 {
      // Print unknown % sequence to draw attention.
      consputc('%');
      consputc(c0);
    }
  }
  va_end(ap);

  만약 (panicking == 0)
    release(&pr.lock);

  반환 0;
}

void
panic(char *s)
{
  panicking = 1;
  printk("panic: ");
  printk("%s\n", s);
  panicked = 1; // freeze uart output from other CPUs
  위해 (;;)
    ;
}

void
printkinit(void)
{
  initlock(&pr.lock, "pr");
}
