//
// formatted console output -- printk, panic.
//

#ଅନ୍ତର୍ଭାଵ <stdarg.h>

#ଅନ୍ତର୍ଭାଵ "types.h"
#ଅନ୍ତର୍ଭାଵ "param.h"
#ଅନ୍ତର୍ଭାଵ "spinlock.h"
#ଅନ୍ତର୍ଭାଵ "sleeplock.h"
#ଅନ୍ତର୍ଭାଵ "fs.h"
#ଅନ୍ତର୍ଭାଵ "file.h"
#ଅନ୍ତର୍ଭାଵ "memlayout.h"
#ଅନ୍ତର୍ଭାଵ "riscv.h"
#ଅନ୍ତର୍ଭାଵ "defs.h"
#ଅନ୍ତର୍ଭାଵ "proc.h"

ଅସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା panicking = 0; // printing a panic message
ଅସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା panicked = 0;  // spinning forever at end of a panic

// lock to avoid interleaving concurrent printk's.
ସ୍ଥିର ସଂରଚନା {
  ସଂରଚନା spinlock lock;
} pr;

ସ୍ଥିର ଅକ୍ଷର digits[] = "0123456789abcdef";

ସ୍ଥିର ଶୂନ୍ୟ
printint(long long xx, ପୂର୍ଣ୍ଣସଂଖ୍ୟା base, ପୂର୍ଣ୍ଣସଂଖ୍ୟା sign)
{
  ଅକ୍ଷର buf[20];
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
  unsigned long long x;

  ଯଦି (sign && (sign = (xx < 0)))
    x = -xx;
  ଅନ୍ୟଥା
    x = xx;

  i = 0;
  କର {
    buf[i++] = digits[x % base];
  } ଯେତେବେଳେ ((x /= base) != 0);

  ଯଦି (sign)
    buf[i++] = '-';

  ଯେତେବେଳେ (--i >= 0)
    consputc(buf[i]);
}

ସ୍ଥିର ଶୂନ୍ୟ
printptr(uint64 x)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
  consputc('0');
  consputc('x');
  ପାଇଁ (i = 0; i < (ଆକାର(uint64) * 2); i++, x <<= 4)
    consputc(digits[x >> (ଆକାର(uint64) * 8 - 4)]);
}

// Print to the console.
ପୂର୍ଣ୍ଣସଂଖ୍ୟା
printk(ଅକ୍ଷର *fmt, ...)
{
  va_list ap;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା i, cx, c0, c1, c2;
  ଅକ୍ଷର *s;

  ଯଦି (panicking == 0)
    acquire(&pr.lock);

  va_start(ap, fmt);
  ପାଇଁ (i = 0; (cx = fmt[i] & 0xff) != 0; i++) {
    ଯଦି (cx != '%') {
      consputc(cx);
      ଜାରି;
    }
    i++;
    c0 = fmt[i + 0] & 0xff;
    c1 = c2 = 0;
    ଯଦି (c0)
      c1 = fmt[i + 1] & 0xff;
    ଯଦି (c1)
      c2 = fmt[i + 2] & 0xff;
    ଯଦି (c0 == 'd') {
      printint(va_arg(ap, ପୂର୍ଣ୍ଣସଂଖ୍ୟା), 10, 1);
    } ଅନ୍ୟଥା ଯଦି (c0 == 'l' && c1 == 'd') {
      printint(va_arg(ap, uint64), 10, 1);
      i += 1;
    } ଅନ୍ୟଥା ଯଦି (c0 == 'l' && c1 == 'l' && c2 == 'd') {
      printint(va_arg(ap, uint64), 10, 1);
      i += 2;
    } ଅନ୍ୟଥା ଯଦି (c0 == 'u') {
      printint(va_arg(ap, uint32), 10, 0);
    } ଅନ୍ୟଥା ଯଦି (c0 == 'l' && c1 == 'u') {
      printint(va_arg(ap, uint64), 10, 0);
      i += 1;
    } ଅନ୍ୟଥା ଯଦି (c0 == 'l' && c1 == 'l' && c2 == 'u') {
      printint(va_arg(ap, uint64), 10, 0);
      i += 2;
    } ଅନ୍ୟଥା ଯଦି (c0 == 'x') {
      printint(va_arg(ap, uint32), 16, 0);
    } ଅନ୍ୟଥା ଯଦି (c0 == 'l' && c1 == 'x') {
      printint(va_arg(ap, uint64), 16, 0);
      i += 1;
    } ଅନ୍ୟଥା ଯଦି (c0 == 'l' && c1 == 'l' && c2 == 'x') {
      printint(va_arg(ap, uint64), 16, 0);
      i += 2;
    } ଅନ୍ୟଥା ଯଦି (c0 == 'p') {
      printptr(va_arg(ap, uint64));
    } ଅନ୍ୟଥା ଯଦି (c0 == 'c') {
      consputc(va_arg(ap, uint));
    } ଅନ୍ୟଥା ଯଦି (c0 == 's') {
      ଯଦି ((s = va_arg(ap, ଅକ୍ଷର *)) == 0)
        s = "(null)";
      ପାଇଁ (; *s; s++)
        consputc(*s);
    } ଅନ୍ୟଥା ଯଦି (c0 == '%') {
      consputc('%');
    } ଅନ୍ୟଥା ଯଦି (c0 == 0) {
      ଭାଙ୍ଗ;
    } ଅନ୍ୟଥା {
      // Print unknown % sequence to draw attention.
      consputc('%');
      consputc(c0);
    }
  }
  va_end(ap);

  ଯଦି (panicking == 0)
    release(&pr.lock);

  ଫେରନ୍ତୁ 0;
}

ଶୂନ୍ୟ
panic(ଅକ୍ଷର *s)
{
  panicking = 1;
  printk("panic: ");
  printk("%s\n", s);
  panicked = 1; // freeze uart output from other CPUs
  ପାଇଁ (;;)
    ;
}

ଶୂନ୍ୟ
printkinit(ଶୂନ୍ୟ)
{
  initlock(&pr.lock, "pr");
}
