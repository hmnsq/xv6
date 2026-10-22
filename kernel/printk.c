//
// formatted console output -- printk, panic.
//

#అన్తర్భావ <stdarg.h>

#అన్తర్భావ "types.h"
#అన్తర్భావ "param.h"
#అన్తర్భావ "spinlock.h"
#అన్తర్భావ "sleeplock.h"
#అన్తర్భావ "fs.h"
#అన్తర్భావ "file.h"
#అన్తర్భావ "memlayout.h"
#అన్తర్భావ "riscv.h"
#అన్తర్భావ "defs.h"
#అన్తర్భావ "proc.h"

అస్థిర పూర్ణాంకం panicking = 0; // printing a panic message
అస్థిర పూర్ణాంకం panicked = 0;  // spinning forever at end of a panic

// lock to avoid interleaving concurrent printk's.
స్థిర నిర్మాణం {
  నిర్మాణం spinlock తాళం;
} pr;

స్థిర అక్షరం digits[] = "0123456789abcdef";

స్థిర శూన్య
printint(దీర్ఘ దీర్ఘ xx, పూర్ణాంకం పునాది, పూర్ణాంకం sign)
{
  అక్షరం buf[20];
  పూర్ణాంకం i;
  గుర్తులేని దీర్ఘ దీర్ఘ x;

  ఒకవేళ (sign && (sign = (xx < 0)))
    x = -xx;
  లేకపోతే
    x = xx;

  i = 0;
  చేయి {
    buf[i++] = digits[x % పునాది];
  } అయితే ((x /= పునాది) != 0);

  ఒకవేళ (sign)
    buf[i++] = '-';

  అయితే (--i >= 0)
    consputc(buf[i]);
}

స్థిర శూన్య
printptr(uint64 x)
{
  పూర్ణాంకం i;
  consputc('0');
  consputc('x');
  కొరకు (i = 0; i < (పరిమాణం(uint64) * 2); i++, x <<= 4)
    consputc(digits[x >> (పరిమాణం(uint64) * 8 - 4)]);
}

// Print to the console.
పూర్ణాంకం
printk(అక్షరం *fmt, ...)
{
  va_list ap;
  పూర్ణాంకం i, cx, c0, c1, c2;
  అక్షరం *s;

  ఒకవేళ (panicking == 0)
    acquire(&pr.తాళం);

  va_start(ap, fmt);
  కొరకు (i = 0; (cx = fmt[i] & 0xff) != 0; i++) {
    ఒకవేళ (cx != '%') {
      consputc(cx);
      కొనసాగు;
    }
    i++;
    c0 = fmt[i + 0] & 0xff;
    c1 = c2 = 0;
    ఒకవేళ (c0)
      c1 = fmt[i + 1] & 0xff;
    ఒకవేళ (c1)
      c2 = fmt[i + 2] & 0xff;
    ఒకవేళ (c0 == 'd') {
      printint(va_arg(ap, పూర్ణాంకం), 10, 1);
    } లేకపోతే ఒకవేళ (c0 == 'l' && c1 == 'd') {
      printint(va_arg(ap, uint64), 10, 1);
      i += 1;
    } లేకపోతే ఒకవేళ (c0 == 'l' && c1 == 'l' && c2 == 'd') {
      printint(va_arg(ap, uint64), 10, 1);
      i += 2;
    } లేకపోతే ఒకవేళ (c0 == 'u') {
      printint(va_arg(ap, uint32), 10, 0);
    } లేకపోతే ఒకవేళ (c0 == 'l' && c1 == 'u') {
      printint(va_arg(ap, uint64), 10, 0);
      i += 1;
    } లేకపోతే ఒకవేళ (c0 == 'l' && c1 == 'l' && c2 == 'u') {
      printint(va_arg(ap, uint64), 10, 0);
      i += 2;
    } లేకపోతే ఒకవేళ (c0 == 'x') {
      printint(va_arg(ap, uint32), 16, 0);
    } లేకపోతే ఒకవేళ (c0 == 'l' && c1 == 'x') {
      printint(va_arg(ap, uint64), 16, 0);
      i += 1;
    } లేకపోతే ఒకవేళ (c0 == 'l' && c1 == 'l' && c2 == 'x') {
      printint(va_arg(ap, uint64), 16, 0);
      i += 2;
    } లేకపోతే ఒకవేళ (c0 == 'p') {
      printptr(va_arg(ap, uint64));
    } లేకపోతే ఒకవేళ (c0 == 'c') {
      consputc(va_arg(ap, uint));
    } లేకపోతే ఒకవేళ (c0 == 's') {
      ఒకవేళ ((s = va_arg(ap, అక్షరం *)) == 0)
        s = "(null)";
      కొరకు (; *s; s++)
        consputc(*s);
    } లేకపోతే ఒకవేళ (c0 == '%') {
      consputc('%');
    } లేకపోతే ఒకవేళ (c0 == 0) {
      ఆపు;
    } లేకపోతే {
      // Print unknown % sequence to draw attention.
      consputc('%');
      consputc(c0);
    }
  }
  va_end(ap);

  ఒకవేళ (panicking == 0)
    release(&pr.తాళం);

  తిరిగి 0;
}

శూన్య
panic(అక్షరం *s)
{
  panicking = 1;
  printk("panic: ");
  printk("%s\n", s);
  panicked = 1; // freeze uart output from other CPUs
  కొరకు (;;)
    ;
}

శూన్య
printkinit(శూన్య)
{
  initlock(&pr.తాళం, "pr");
}
