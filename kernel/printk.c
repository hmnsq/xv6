//
// formatted console output -- printk, panic.
//

#অন্তর্ভাব <stdarg.h>

#অন্তর্ভাব "types.h"
#অন্তর্ভাব "param.h"
#অন্তর্ভাব "spinlock.h"
#অন্তর্ভাব "sleeplock.h"
#অন্তর্ভাব "fs.h"
#অন্তর্ভাব "file.h"
#অন্তর্ভাব "memlayout.h"
#অন্তর্ভাব "riscv.h"
#অন্তর্ভাব "defs.h"
#অন্তর্ভাব "proc.h"

অস্থির পূর্ণসংখ্যা panicking = 0; // printing a panic message
অস্থির পূর্ণসংখ্যা panicked = 0;  // spinning forever at end of a panic

// lock to avoid interleaving concurrent printk's.
স্থির কাঠামো {
  কাঠামো spinlock lock;
} pr;

স্থির অক্ষর digits[] = "0123456789abcdef";

স্থির শূন্য
printint(long long xx, পূর্ণসংখ্যা base, পূর্ণসংখ্যা sign)
{
  অক্ষর buf[20];
  পূর্ণসংখ্যা i;
  unsigned long long x;

  যদি (sign && (sign = (xx < 0)))
    x = -xx;
  অন্যথা
    x = xx;

  i = 0;
  করো {
    buf[i++] = digits[x % base];
  } যখন ((x /= base) != 0);

  যদি (sign)
    buf[i++] = '-';

  যখন (--i >= 0)
    consputc(buf[i]);
}

স্থির শূন্য
printptr(uint64 x)
{
  পূর্ণসংখ্যা i;
  consputc('0');
  consputc('x');
  জন্য (i = 0; i < (আকার(uint64) * 2); i++, x <<= 4)
    consputc(digits[x >> (আকার(uint64) * 8 - 4)]);
}

// Print to the console.
পূর্ণসংখ্যা
printk(অক্ষর *fmt, ...)
{
  va_list ap;
  পূর্ণসংখ্যা i, cx, c0, c1, c2;
  অক্ষর *s;

  যদি (panicking == 0)
    acquire(&pr.lock);

  va_start(ap, fmt);
  জন্য (i = 0; (cx = fmt[i] & 0xff) != 0; i++) {
    যদি (cx != '%') {
      consputc(cx);
      চালু;
    }
    i++;
    c0 = fmt[i + 0] & 0xff;
    c1 = c2 = 0;
    যদি (c0)
      c1 = fmt[i + 1] & 0xff;
    যদি (c1)
      c2 = fmt[i + 2] & 0xff;
    যদি (c0 == 'd') {
      printint(va_arg(ap, পূর্ণসংখ্যা), 10, 1);
    } অন্যথা যদি (c0 == 'l' && c1 == 'd') {
      printint(va_arg(ap, uint64), 10, 1);
      i += 1;
    } অন্যথা যদি (c0 == 'l' && c1 == 'l' && c2 == 'd') {
      printint(va_arg(ap, uint64), 10, 1);
      i += 2;
    } অন্যথা যদি (c0 == 'u') {
      printint(va_arg(ap, uint32), 10, 0);
    } অন্যথা যদি (c0 == 'l' && c1 == 'u') {
      printint(va_arg(ap, uint64), 10, 0);
      i += 1;
    } অন্যথা যদি (c0 == 'l' && c1 == 'l' && c2 == 'u') {
      printint(va_arg(ap, uint64), 10, 0);
      i += 2;
    } অন্যথা যদি (c0 == 'x') {
      printint(va_arg(ap, uint32), 16, 0);
    } অন্যথা যদি (c0 == 'l' && c1 == 'x') {
      printint(va_arg(ap, uint64), 16, 0);
      i += 1;
    } অন্যথা যদি (c0 == 'l' && c1 == 'l' && c2 == 'x') {
      printint(va_arg(ap, uint64), 16, 0);
      i += 2;
    } অন্যথা যদি (c0 == 'p') {
      printptr(va_arg(ap, uint64));
    } অন্যথা যদি (c0 == 'c') {
      consputc(va_arg(ap, uint));
    } অন্যথা যদি (c0 == 's') {
      যদি ((s = va_arg(ap, অক্ষর *)) == 0)
        s = "(null)";
      জন্য (; *s; s++)
        consputc(*s);
    } অন্যথা যদি (c0 == '%') {
      consputc('%');
    } অন্যথা যদি (c0 == 0) {
      বিরতি;
    } অন্যথা {
      // Print unknown % sequence to draw attention.
      consputc('%');
      consputc(c0);
    }
  }
  va_end(ap);

  যদি (panicking == 0)
    release(&pr.lock);

  ফেরত 0;
}

শূন্য
panic(অক্ষর *s)
{
  panicking = 1;
  printk("panic: ");
  printk("%s\n", s);
  panicked = 1; // freeze uart output from other CPUs
  জন্য (;;)
    ;
}

শূন্য
printkinit(শূন্য)
{
  initlock(&pr.lock, "pr");
}
