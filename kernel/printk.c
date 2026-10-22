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

volatile 整数 panicking = 0; // printing a panic message
volatile 整数 panicked = 0;  // spinning forever at end of a panic

// lock to avoid interleaving concurrent printk's.
static struct {
  struct spinlock lock;
} pr;

static char digits[] = "0123456789abcdef";

static void
printint(long long xx, 整数 base, 整数 sign)
{
  char buf[20];
  整数 i;
  unsigned long long x;

  もし (sign && (sign = (xx < 0)))
    x = -xx;
  否则
    x = xx;

  i = 0;
  do {
    buf[i++] = digits[x % base];
  } ながら ((x /= base) != 0);

  もし (sign)
    buf[i++] = '-';

  ながら (--i >= 0)
    consputc(buf[i]);
}

static void
printptr(uint64 x)
{
  整数 i;
  consputc('0');
  consputc('x');
  ために (i = 0; i < (sizeof(uint64) * 2); i++, x <<= 4)
    consputc(digits[x >> (sizeof(uint64) * 8 - 4)]);
}

// Print to the console.
整数
printk(char *fmt, ...)
{
  va_list ap;
  整数 i, cx, c0, c1, c2;
  char *s;

  もし (panicking == 0)
    acquire(&pr.lock);

  va_start(ap, fmt);
  ために (i = 0; (cx = fmt[i] & 0xff) != 0; i++) {
    もし (cx != '%') {
      consputc(cx);
      continue;
    }
    i++;
    c0 = fmt[i + 0] & 0xff;
    c1 = c2 = 0;
    もし (c0)
      c1 = fmt[i + 1] & 0xff;
    もし (c1)
      c2 = fmt[i + 2] & 0xff;
    もし (c0 == 'd') {
      printint(va_arg(ap, 整数), 10, 1);
    } 否则 もし (c0 == 'l' && c1 == 'd') {
      printint(va_arg(ap, uint64), 10, 1);
      i += 1;
    } 否则 もし (c0 == 'l' && c1 == 'l' && c2 == 'd') {
      printint(va_arg(ap, uint64), 10, 1);
      i += 2;
    } 否则 もし (c0 == 'u') {
      printint(va_arg(ap, uint32), 10, 0);
    } 否则 もし (c0 == 'l' && c1 == 'u') {
      printint(va_arg(ap, uint64), 10, 0);
      i += 1;
    } 否则 もし (c0 == 'l' && c1 == 'l' && c2 == 'u') {
      printint(va_arg(ap, uint64), 10, 0);
      i += 2;
    } 否则 もし (c0 == 'x') {
      printint(va_arg(ap, uint32), 16, 0);
    } 否则 もし (c0 == 'l' && c1 == 'x') {
      printint(va_arg(ap, uint64), 16, 0);
      i += 1;
    } 否则 もし (c0 == 'l' && c1 == 'l' && c2 == 'x') {
      printint(va_arg(ap, uint64), 16, 0);
      i += 2;
    } 否则 もし (c0 == 'p') {
      printptr(va_arg(ap, uint64));
    } 否则 もし (c0 == 'c') {
      consputc(va_arg(ap, uint));
    } 否则 もし (c0 == 's') {
      もし ((s = va_arg(ap, char *)) == 0)
        s = "(null)";
      ために (; *s; s++)
        consputc(*s);
    } 否则 もし (c0 == '%') {
      consputc('%');
    } 否则 もし (c0 == 0) {
      break;
    } 否则 {
      // Print unknown % sequence to draw attention.
      consputc('%');
      consputc(c0);
    }
  }
  va_end(ap);

  もし (panicking == 0)
    release(&pr.lock);

  戻る 0;
}

void
panic(char *s)
{
  panicking = 1;
  printk("panic: ");
  printk("%s\n", s);
  panicked = 1; // freeze uart output from other CPUs
  ために (;;)
    ;
}

void
printkinit(void)
{
  initlock(&pr.lock, "pr");
}
