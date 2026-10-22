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

চঞ্চল পূর্ণাঙ্ক panicking = 0; // printing a panic message
চঞ্চল পূর্ণাঙ্ক panicked = 0;  // spinning forever at end of a panic

// lock to avoid interleaving concurrent printk's.
স্থির সংরচনা {
  সংরচনা spinlock lock;
} pr;

স্থির বর্ণ digits[] = "0123456789abcdef";

স্থির শূন্য
printint(long long xx, পূর্ণাঙ্ক base, পূর্ণাঙ্ক sign)
{
  বর্ণ buf[20];
  পূর্ণাঙ্ক i;
  unsigned long long x;

  যদি (sign && (sign = (xx < 0)))
    x = -xx;
  অন্যথা
    x = xx;

  i = 0;
  কুরু {
    buf[i++] = digits[x % base];
  } যাবত্ ((x /= base) != 0);

  যদি (sign)
    buf[i++] = '-';

  যাবত্ (--i >= 0)
    consputc(buf[i]);
}

স্থির শূন্য
printptr(uint64 x)
{
  পূর্ণাঙ্ক i;
  consputc('0');
  consputc('x');
  প্রতি (i = 0; i < (পরিমাণ(uint64) * 2); i++, x <<= 4)
    consputc(digits[x >> (পরিমাণ(uint64) * 8 - 4)]);
}

// Print to the console.
পূর্ণাঙ্ক
printk(বর্ণ *fmt, ...)
{
  va_list ap;
  পূর্ণাঙ্ক i, cx, c0, c1, c2;
  বর্ণ *s;

  যদি (panicking == 0)
    acquire(&pr.lock);

  va_start(ap, fmt);
  প্রতি (i = 0; (cx = fmt[i] & 0xff) != 0; i++) {
    যদি (cx != '%') {
      consputc(cx);
      অনুবর্ত;
    }
    i++;
    c0 = fmt[i + 0] & 0xff;
    c1 = c2 = 0;
    যদি (c0)
      c1 = fmt[i + 1] & 0xff;
    যদি (c1)
      c2 = fmt[i + 2] & 0xff;
    যদি (c0 == 'd') {
      printint(va_arg(ap, পূর্ণাঙ্ক), 10, 1);
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
      যদি ((s = va_arg(ap, বর্ণ *)) == 0)
        s = "(null)";
      প্রতি (; *s; s++)
        consputc(*s);
    } অন্যথা যদি (c0 == '%') {
      consputc('%');
    } অন্যথা যদি (c0 == 0) {
      ভঙ্গ;
    } অন্যথা {
      // Print unknown % sequence to draw attention.
      consputc('%');
      consputc(c0);
    }
  }
  va_end(ap);

  যদি (panicking == 0)
    release(&pr.lock);

  প্রত্যাগম 0;
}

শূন্য
panic(বর্ণ *s)
{
  panicking = 1;
  printk("panic: ");
  printk("%s\n", s);
  panicked = 1; // freeze uart output from other CPUs
  প্রতি (;;)
    ;
}

শূন্য
printkinit(শূন্য)
{
  initlock(&pr.lock, "pr");
}
