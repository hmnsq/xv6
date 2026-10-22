//
// formatted console output -- printk, panic.
//

#شامل <stdarg.h>

#شامل "types.h"
#شامل "param.h"
#شامل "spinlock.h"
#شامل "sleeplock.h"
#شامل "fs.h"
#شامل "file.h"
#شامل "memlayout.h"
#شامل "riscv.h"
#شامل "defs.h"
#شامل "proc.h"

متلون صحیح panicking = 0; // printing a panic message
متلون صحیح panicked = 0;  // spinning forever at end of a panic

// lock to avoid interleaving concurrent printk's.
ساکن ساخت {
  ساخت spinlock تالا;
} pr;

ساکن حرف digits[] = "0123456789abcdef";

ساکن خالی
printint(طویل طویل xx, صحیح بنیاد, صحیح sign)
{
  حرف buf[20];
  صحیح i;
  غیرعلامتی طویل طویل x;

  اگر (sign && (sign = (xx < 0)))
    x = -xx;
  ورنہ
    x = xx;

  i = 0;
  کرو {
    buf[i++] = digits[x % بنیاد];
  } جب ((x /= بنیاد) != 0);

  اگر (sign)
    buf[i++] = '-';

  جب (--i >= 0)
    consputc(buf[i]);
}

ساکن خالی
printptr(uint64 x)
{
  صحیح i;
  consputc('0');
  consputc('x');
  برائے (i = 0; i < (حجم(uint64) * 2); i++, x <<= 4)
    consputc(digits[x >> (حجم(uint64) * 8 - 4)]);
}

// Print to the console.
صحیح
printk(حرف *fmt, ...)
{
  va_list ap;
  صحیح i, cx, c0, c1, c2;
  حرف *s;

  اگر (panicking == 0)
    acquire(&pr.تالا);

  va_start(ap, fmt);
  برائے (i = 0; (cx = fmt[i] & 0xff) != 0; i++) {
    اگر (cx != '%') {
      consputc(cx);
      جاری;
    }
    i++;
    c0 = fmt[i + 0] & 0xff;
    c1 = c2 = 0;
    اگر (c0)
      c1 = fmt[i + 1] & 0xff;
    اگر (c1)
      c2 = fmt[i + 2] & 0xff;
    اگر (c0 == 'd') {
      printint(va_arg(ap, صحیح), 10, 1);
    } ورنہ اگر (c0 == 'l' && c1 == 'd') {
      printint(va_arg(ap, uint64), 10, 1);
      i += 1;
    } ورنہ اگر (c0 == 'l' && c1 == 'l' && c2 == 'd') {
      printint(va_arg(ap, uint64), 10, 1);
      i += 2;
    } ورنہ اگر (c0 == 'u') {
      printint(va_arg(ap, uint32), 10, 0);
    } ورنہ اگر (c0 == 'l' && c1 == 'u') {
      printint(va_arg(ap, uint64), 10, 0);
      i += 1;
    } ورنہ اگر (c0 == 'l' && c1 == 'l' && c2 == 'u') {
      printint(va_arg(ap, uint64), 10, 0);
      i += 2;
    } ورنہ اگر (c0 == 'x') {
      printint(va_arg(ap, uint32), 16, 0);
    } ورنہ اگر (c0 == 'l' && c1 == 'x') {
      printint(va_arg(ap, uint64), 16, 0);
      i += 1;
    } ورنہ اگر (c0 == 'l' && c1 == 'l' && c2 == 'x') {
      printint(va_arg(ap, uint64), 16, 0);
      i += 2;
    } ورنہ اگر (c0 == 'p') {
      printptr(va_arg(ap, uint64));
    } ورنہ اگر (c0 == 'c') {
      consputc(va_arg(ap, uint));
    } ورنہ اگر (c0 == 's') {
      اگر ((s = va_arg(ap, حرف *)) == 0)
        s = "(null)";
      برائے (; *s; s++)
        consputc(*s);
    } ورنہ اگر (c0 == '%') {
      consputc('%');
    } ورنہ اگر (c0 == 0) {
      توڑو;
    } ورنہ {
      // Print unknown % sequence to draw attention.
      consputc('%');
      consputc(c0);
    }
  }
  va_end(ap);

  اگر (panicking == 0)
    release(&pr.تالا);

  واپس 0;
}

خالی
panic(حرف *s)
{
  panicking = 1;
  printk("panic: ");
  printk("%s\n", s);
  panicked = 1; // freeze uart output from other CPUs
  برائے (;;)
    ;
}

خالی
printkinit(خالی)
{
  initlock(&pr.تالا, "pr");
}
