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

ca_ncala ຈຳນວນເຕັມ panicking = 0; // printing a panic message
ca_ncala ຈຳນວນເຕັມ panicked = 0;  // spinning forever at end of a panic

// lock to avoid interleaving concurrent printk's.
static sa_nracanaa {
  sa_nracanaa spinlock lock;
} pr;

static var_na digits[] = "0123456789abcdef";

static void
printint(long long xx, ຈຳນວນເຕັມ base, ຈຳນວນເຕັມ sign)
{
  var_na buf[20];
  ຈຳນວນເຕັມ i;
  unsigned long long x;

  ຖ້າ (sign && (sign = (xx < 0)))
    x = -xx;
  ບໍ່ດັ່ງນັ້ນ
    x = xx;

  i = 0;
  do {
    buf[i++] = digits[x % base];
  } ໃນຂະນະທີ່ ((x /= base) != 0);

  ຖ້າ (sign)
    buf[i++] = '-';

  ໃນຂະນະທີ່ (--i >= 0)
    consputc(buf[i]);
}

static void
printptr(uint64 x)
{
  ຈຳນວນເຕັມ i;
  consputc('0');
  consputc('x');
  ສຳລັບ (i = 0; i < (parimaa_na(uint64) * 2); i++, x <<= 4)
    consputc(digits[x >> (parimaa_na(uint64) * 8 - 4)]);
}

// Print to the console.
ຈຳນວນເຕັມ
printk(var_na *fmt, ...)
{
  va_list ap;
  ຈຳນວນເຕັມ i, cx, c0, c1, c2;
  var_na *s;

  ຖ້າ (panicking == 0)
    acquire(&pr.lock);

  va_start(ap, fmt);
  ສຳລັບ (i = 0; (cx = fmt[i] & 0xff) != 0; i++) {
    ຖ້າ (cx != '%') {
      consputc(cx);
      continue;
    }
    i++;
    c0 = fmt[i + 0] & 0xff;
    c1 = c2 = 0;
    ຖ້າ (c0)
      c1 = fmt[i + 1] & 0xff;
    ຖ້າ (c1)
      c2 = fmt[i + 2] & 0xff;
    ຖ້າ (c0 == 'd') {
      printint(va_arg(ap, ຈຳນວນເຕັມ), 10, 1);
    } ບໍ່ດັ່ງນັ້ນ ຖ້າ (c0 == 'l' && c1 == 'd') {
      printint(va_arg(ap, uint64), 10, 1);
      i += 1;
    } ບໍ່ດັ່ງນັ້ນ ຖ້າ (c0 == 'l' && c1 == 'l' && c2 == 'd') {
      printint(va_arg(ap, uint64), 10, 1);
      i += 2;
    } ບໍ່ດັ່ງນັ້ນ ຖ້າ (c0 == 'u') {
      printint(va_arg(ap, uint32), 10, 0);
    } ບໍ່ດັ່ງນັ້ນ ຖ້າ (c0 == 'l' && c1 == 'u') {
      printint(va_arg(ap, uint64), 10, 0);
      i += 1;
    } ບໍ່ດັ່ງນັ້ນ ຖ້າ (c0 == 'l' && c1 == 'l' && c2 == 'u') {
      printint(va_arg(ap, uint64), 10, 0);
      i += 2;
    } ບໍ່ດັ່ງນັ້ນ ຖ້າ (c0 == 'x') {
      printint(va_arg(ap, uint32), 16, 0);
    } ບໍ່ດັ່ງນັ້ນ ຖ້າ (c0 == 'l' && c1 == 'x') {
      printint(va_arg(ap, uint64), 16, 0);
      i += 1;
    } ບໍ່ດັ່ງນັ້ນ ຖ້າ (c0 == 'l' && c1 == 'l' && c2 == 'x') {
      printint(va_arg(ap, uint64), 16, 0);
      i += 2;
    } ບໍ່ດັ່ງນັ້ນ ຖ້າ (c0 == 'p') {
      printptr(va_arg(ap, uint64));
    } ບໍ່ດັ່ງນັ້ນ ຖ້າ (c0 == 'c') {
      consputc(va_arg(ap, uint));
    } ບໍ່ດັ່ງນັ້ນ ຖ້າ (c0 == 's') {
      ຖ້າ ((s = va_arg(ap, var_na *)) == 0)
        s = "(null)";
      ສຳລັບ (; *s; s++)
        consputc(*s);
    } ບໍ່ດັ່ງນັ້ນ ຖ້າ (c0 == '%') {
      consputc('%');
    } ບໍ່ດັ່ງນັ້ນ ຖ້າ (c0 == 0) {
      bha_nga;
    } ບໍ່ດັ່ງນັ້ນ {
      // Print unknown % sequence to draw attention.
      consputc('%');
      consputc(c0);
    }
  }
  va_end(ap);

  ຖ້າ (panicking == 0)
    release(&pr.lock);

  ສົ່ງຄືນ 0;
}

void
panic(var_na *s)
{
  panicking = 1;
  printk("panic: ");
  printk("%s\n", s);
  panicked = 1; // freeze uart output from other CPUs
  ສຳລັບ (;;)
    ;
}

void
printkinit(void)
{
  initlock(&pr.lock, "pr");
}
