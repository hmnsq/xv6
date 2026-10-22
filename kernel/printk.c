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

ca_ncala ကိန်းပြည့် panicking = 0; // printing a panic message
ca_ncala ကိန်းပြည့် panicked = 0;  // spinning forever at end of a panic

// lock to avoid interleaving concurrent printk's.
static sa_nracanaa {
  sa_nracanaa spinlock lock;
} pr;

static var_na digits[] = "0123456789abcdef";

static void
printint(long long xx, ကိန်းပြည့် base, ကိန်းပြည့် sign)
{
  var_na buf[20];
  ကိန်းပြည့် i;
  unsigned long long x;

  အကယ်၍ (sign && (sign = (xx < 0)))
    x = -xx;
  မဟုတ်ပါက
    x = xx;

  i = 0;
  do {
    buf[i++] = digits[x % base];
  } နေစဉ် ((x /= base) != 0);

  အကယ်၍ (sign)
    buf[i++] = '-';

  နေစဉ် (--i >= 0)
    consputc(buf[i]);
}

static void
printptr(uint64 x)
{
  ကိန်းပြည့် i;
  consputc('0');
  consputc('x');
  အတွက် (i = 0; i < (parimaa_na(uint64) * 2); i++, x <<= 4)
    consputc(digits[x >> (parimaa_na(uint64) * 8 - 4)]);
}

// Print to the console.
ကိန်းပြည့်
printk(var_na *fmt, ...)
{
  va_list ap;
  ကိန်းပြည့် i, cx, c0, c1, c2;
  var_na *s;

  အကယ်၍ (panicking == 0)
    acquire(&pr.lock);

  va_start(ap, fmt);
  အတွက် (i = 0; (cx = fmt[i] & 0xff) != 0; i++) {
    အကယ်၍ (cx != '%') {
      consputc(cx);
      continue;
    }
    i++;
    c0 = fmt[i + 0] & 0xff;
    c1 = c2 = 0;
    အကယ်၍ (c0)
      c1 = fmt[i + 1] & 0xff;
    အကယ်၍ (c1)
      c2 = fmt[i + 2] & 0xff;
    အကယ်၍ (c0 == 'd') {
      printint(va_arg(ap, ကိန်းပြည့်), 10, 1);
    } မဟုတ်ပါက အကယ်၍ (c0 == 'l' && c1 == 'd') {
      printint(va_arg(ap, uint64), 10, 1);
      i += 1;
    } မဟုတ်ပါက အကယ်၍ (c0 == 'l' && c1 == 'l' && c2 == 'd') {
      printint(va_arg(ap, uint64), 10, 1);
      i += 2;
    } မဟုတ်ပါက အကယ်၍ (c0 == 'u') {
      printint(va_arg(ap, uint32), 10, 0);
    } မဟုတ်ပါက အကယ်၍ (c0 == 'l' && c1 == 'u') {
      printint(va_arg(ap, uint64), 10, 0);
      i += 1;
    } မဟုတ်ပါက အကယ်၍ (c0 == 'l' && c1 == 'l' && c2 == 'u') {
      printint(va_arg(ap, uint64), 10, 0);
      i += 2;
    } မဟုတ်ပါက အကယ်၍ (c0 == 'x') {
      printint(va_arg(ap, uint32), 16, 0);
    } မဟုတ်ပါက အကယ်၍ (c0 == 'l' && c1 == 'x') {
      printint(va_arg(ap, uint64), 16, 0);
      i += 1;
    } မဟုတ်ပါက အကယ်၍ (c0 == 'l' && c1 == 'l' && c2 == 'x') {
      printint(va_arg(ap, uint64), 16, 0);
      i += 2;
    } မဟုတ်ပါက အကယ်၍ (c0 == 'p') {
      printptr(va_arg(ap, uint64));
    } မဟုတ်ပါက အကယ်၍ (c0 == 'c') {
      consputc(va_arg(ap, uint));
    } မဟုတ်ပါက အကယ်၍ (c0 == 's') {
      အကယ်၍ ((s = va_arg(ap, var_na *)) == 0)
        s = "(null)";
      အတွက် (; *s; s++)
        consputc(*s);
    } မဟုတ်ပါက အကယ်၍ (c0 == '%') {
      consputc('%');
    } မဟုတ်ပါက အကယ်၍ (c0 == 0) {
      bha_nga;
    } မဟုတ်ပါက {
      // Print unknown % sequence to draw attention.
      consputc('%');
      consputc(c0);
    }
  }
  va_end(ap);

  အကယ်၍ (panicking == 0)
    release(&pr.lock);

  ပြန်ပေး 0;
}

void
panic(var_na *s)
{
  panicking = 1;
  printk("panic: ");
  printk("%s\n", s);
  panicked = 1; // freeze uart output from other CPUs
  အတွက် (;;)
    ;
}

void
printkinit(void)
{
  initlock(&pr.lock, "pr");
}
