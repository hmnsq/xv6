//
// formatted console output -- printk, panic.
//

#ضمن <stdarg.h>

#ضمن "types.h"
#ضمن "param.h"
#ضمن "spinlock.h"
#ضمن "sleeplock.h"
#ضمن "fs.h"
#ضمن "file.h"
#ضمن "memlayout.h"
#ضمن "riscv.h"
#ضمن "defs.h"
#ضمن "proc.h"

متطاير صحيح panicking = 0; // printing a panic message
متطاير صحيح panicked = 0;  // spinning forever at end of a panic

// lock to avoid interleaving concurrent printk's.
ساكن بنية {
  بنية spinlock جندرا;
} pr;

ساكن حرف digits[] = "0123456789abcdef";

ساكن فراغ
printint(لما لما xx, صحيح ادھار, صحيح sign)
{
  حرف buf[20];
  صحيح i;
  بنانشانی لما لما x;

  إذا (sign && (sign = (xx < 0)))
    x = -xx;
  وإلا
    x = xx;

  i = 0;
  افعل {
    buf[i++] = digits[x % ادھار];
  } طالما ((x /= ادھار) != 0);

  إذا (sign)
    buf[i++] = '-';

  طالما (--i >= 0)
    consputc(buf[i]);
}

ساكن فراغ
printptr(uint64 x)
{
  صحيح i;
  consputc('0');
  consputc('x');
  لكل (i = 0; i < (حجم(uint64) * 2); i++, x <<= 4)
    consputc(digits[x >> (حجم(uint64) * 8 - 4)]);
}

// Print to the console.
صحيح
printk(حرف *fmt, ...)
{
  va_list ap;
  صحيح i, cx, c0, c1, c2;
  حرف *s;

  إذا (panicking == 0)
    acquire(&pr.جندرا);

  va_start(ap, fmt);
  لكل (i = 0; (cx = fmt[i] & 0xff) != 0; i++) {
    إذا (cx != '%') {
      consputc(cx);
      استمر;
    }
    i++;
    c0 = fmt[i + 0] & 0xff;
    c1 = c2 = 0;
    إذا (c0)
      c1 = fmt[i + 1] & 0xff;
    إذا (c1)
      c2 = fmt[i + 2] & 0xff;
    إذا (c0 == 'd') {
      printint(va_arg(ap, صحيح), 10, 1);
    } وإلا إذا (c0 == 'l' && c1 == 'd') {
      printint(va_arg(ap, uint64), 10, 1);
      i += 1;
    } وإلا إذا (c0 == 'l' && c1 == 'l' && c2 == 'd') {
      printint(va_arg(ap, uint64), 10, 1);
      i += 2;
    } وإلا إذا (c0 == 'u') {
      printint(va_arg(ap, uint32), 10, 0);
    } وإلا إذا (c0 == 'l' && c1 == 'u') {
      printint(va_arg(ap, uint64), 10, 0);
      i += 1;
    } وإلا إذا (c0 == 'l' && c1 == 'l' && c2 == 'u') {
      printint(va_arg(ap, uint64), 10, 0);
      i += 2;
    } وإلا إذا (c0 == 'x') {
      printint(va_arg(ap, uint32), 16, 0);
    } وإلا إذا (c0 == 'l' && c1 == 'x') {
      printint(va_arg(ap, uint64), 16, 0);
      i += 1;
    } وإلا إذا (c0 == 'l' && c1 == 'l' && c2 == 'x') {
      printint(va_arg(ap, uint64), 16, 0);
      i += 2;
    } وإلا إذا (c0 == 'p') {
      printptr(va_arg(ap, uint64));
    } وإلا إذا (c0 == 'c') {
      consputc(va_arg(ap, uint));
    } وإلا إذا (c0 == 's') {
      إذا ((s = va_arg(ap, حرف *)) == 0)
        s = "(null)";
      لكل (; *s; s++)
        consputc(*s);
    } وإلا إذا (c0 == '%') {
      consputc('%');
    } وإلا إذا (c0 == 0) {
      اكسر;
    } وإلا {
      // Print unknown % sequence to draw attention.
      consputc('%');
      consputc(c0);
    }
  }
  va_end(ap);

  إذا (panicking == 0)
    release(&pr.جندرا);

  أرجع 0;
}

فراغ
panic(حرف *s)
{
  panicking = 1;
  printk("panic: ");
  printk("%s\n", s);
  panicked = 1; // freeze uart output from other CPUs
  لكل (;;)
    ;
}

فراغ
printkinit(فراغ)
{
  initlock(&pr.جندرا, "pr");
}
