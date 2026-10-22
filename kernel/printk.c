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

ca_ncala จำนวนเต็ม panicking = 0; // printing a panic message
ca_ncala จำนวนเต็ม panicked = 0;  // spinning forever at end of a panic

// lock to avoid interleaving concurrent printk's.
static sa_nracanaa {
  sa_nracanaa spinlock lock;
} pr;

static var_na digits[] = "0123456789abcdef";

static void
printint(long long xx, จำนวนเต็ม base, จำนวนเต็ม sign)
{
  var_na buf[20];
  จำนวนเต็ม i;
  unsigned long long x;

  ถ้า (sign && (sign = (xx < 0)))
    x = -xx;
  มิฉะนั้น
    x = xx;

  i = 0;
  do {
    buf[i++] = digits[x % base];
  } ขณะ ((x /= base) != 0);

  ถ้า (sign)
    buf[i++] = '-';

  ขณะ (--i >= 0)
    consputc(buf[i]);
}

static void
printptr(uint64 x)
{
  จำนวนเต็ม i;
  consputc('0');
  consputc('x');
  สำหรับ (i = 0; i < (parimaa_na(uint64) * 2); i++, x <<= 4)
    consputc(digits[x >> (parimaa_na(uint64) * 8 - 4)]);
}

// Print to the console.
จำนวนเต็ม
printk(var_na *fmt, ...)
{
  va_list ap;
  จำนวนเต็ม i, cx, c0, c1, c2;
  var_na *s;

  ถ้า (panicking == 0)
    acquire(&pr.lock);

  va_start(ap, fmt);
  สำหรับ (i = 0; (cx = fmt[i] & 0xff) != 0; i++) {
    ถ้า (cx != '%') {
      consputc(cx);
      continue;
    }
    i++;
    c0 = fmt[i + 0] & 0xff;
    c1 = c2 = 0;
    ถ้า (c0)
      c1 = fmt[i + 1] & 0xff;
    ถ้า (c1)
      c2 = fmt[i + 2] & 0xff;
    ถ้า (c0 == 'd') {
      printint(va_arg(ap, จำนวนเต็ม), 10, 1);
    } มิฉะนั้น ถ้า (c0 == 'l' && c1 == 'd') {
      printint(va_arg(ap, uint64), 10, 1);
      i += 1;
    } มิฉะนั้น ถ้า (c0 == 'l' && c1 == 'l' && c2 == 'd') {
      printint(va_arg(ap, uint64), 10, 1);
      i += 2;
    } มิฉะนั้น ถ้า (c0 == 'u') {
      printint(va_arg(ap, uint32), 10, 0);
    } มิฉะนั้น ถ้า (c0 == 'l' && c1 == 'u') {
      printint(va_arg(ap, uint64), 10, 0);
      i += 1;
    } มิฉะนั้น ถ้า (c0 == 'l' && c1 == 'l' && c2 == 'u') {
      printint(va_arg(ap, uint64), 10, 0);
      i += 2;
    } มิฉะนั้น ถ้า (c0 == 'x') {
      printint(va_arg(ap, uint32), 16, 0);
    } มิฉะนั้น ถ้า (c0 == 'l' && c1 == 'x') {
      printint(va_arg(ap, uint64), 16, 0);
      i += 1;
    } มิฉะนั้น ถ้า (c0 == 'l' && c1 == 'l' && c2 == 'x') {
      printint(va_arg(ap, uint64), 16, 0);
      i += 2;
    } มิฉะนั้น ถ้า (c0 == 'p') {
      printptr(va_arg(ap, uint64));
    } มิฉะนั้น ถ้า (c0 == 'c') {
      consputc(va_arg(ap, uint));
    } มิฉะนั้น ถ้า (c0 == 's') {
      ถ้า ((s = va_arg(ap, var_na *)) == 0)
        s = "(null)";
      สำหรับ (; *s; s++)
        consputc(*s);
    } มิฉะนั้น ถ้า (c0 == '%') {
      consputc('%');
    } มิฉะนั้น ถ้า (c0 == 0) {
      bha_nga;
    } มิฉะนั้น {
      // Print unknown % sequence to draw attention.
      consputc('%');
      consputc(c0);
    }
  }
  va_end(ap);

  ถ้า (panicking == 0)
    release(&pr.lock);

  คืน 0;
}

void
panic(var_na *s)
{
  panicking = 1;
  printk("panic: ");
  printk("%s\n", s);
  panicked = 1; // freeze uart output from other CPUs
  สำหรับ (;;)
    ;
}

void
printkinit(void)
{
  initlock(&pr.lock, "pr");
}
