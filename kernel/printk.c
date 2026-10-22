//
// formatted console output -- printk, panic.
//

#ਅਨ੍ਤਰ੍ਭਾਵ <stdarg.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "types.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "param.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "spinlock.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "sleeplock.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "fs.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "file.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "memlayout.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "riscv.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "defs.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "proc.h"

ਅਸਥਿਰ ਪੂਰਨ panicking = 0; // printing a panic message
ਅਸਥਿਰ ਪੂਰਨ panicked = 0;  // spinning forever at end of a panic

// lock to avoid interleaving concurrent printk's.
ਸਥਿਰ ਬਣਤਰ {
  ਬਣਤਰ spinlock ਤਾਲਾ;
} pr;

ਸਥਿਰ ਅੱਖਰ digits[] = "0123456789abcdef";

ਸਥਿਰ ਖਾਲੀ
printint(ਲੰਮਾ ਲੰਮਾ xx, ਪੂਰਨ ਅਧਾਰ, ਪੂਰਨ sign)
{
  ਅੱਖਰ buf[20];
  ਪੂਰਨ i;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਲੰਮਾ x;

  ਜੇ (sign && (sign = (xx < 0)))
    x = -xx;
  ਨਹੀਂਤਾਂ
    x = xx;

  i = 0;
  ਕਰੋ {
    buf[i++] = digits[x % ਅਧਾਰ];
  } ਜਦੋਂ ((x /= ਅਧਾਰ) != 0);

  ਜੇ (sign)
    buf[i++] = '-';

  ਜਦੋਂ (--i >= 0)
    consputc(buf[i]);
}

ਸਥਿਰ ਖਾਲੀ
printptr(uint64 x)
{
  ਪੂਰਨ i;
  consputc('0');
  consputc('x');
  ਲਈ (i = 0; i < (ਆਕਾਰ(uint64) * 2); i++, x <<= 4)
    consputc(digits[x >> (ਆਕਾਰ(uint64) * 8 - 4)]);
}

// Print to the console.
ਪੂਰਨ
printk(ਅੱਖਰ *fmt, ...)
{
  va_list ap;
  ਪੂਰਨ i, cx, c0, c1, c2;
  ਅੱਖਰ *s;

  ਜੇ (panicking == 0)
    acquire(&pr.ਤਾਲਾ);

  va_start(ap, fmt);
  ਲਈ (i = 0; (cx = fmt[i] & 0xff) != 0; i++) {
    ਜੇ (cx != '%') {
      consputc(cx);
      ਜਾਰੀ;
    }
    i++;
    c0 = fmt[i + 0] & 0xff;
    c1 = c2 = 0;
    ਜੇ (c0)
      c1 = fmt[i + 1] & 0xff;
    ਜੇ (c1)
      c2 = fmt[i + 2] & 0xff;
    ਜੇ (c0 == 'd') {
      printint(va_arg(ap, ਪੂਰਨ), 10, 1);
    } ਨਹੀਂਤਾਂ ਜੇ (c0 == 'l' && c1 == 'd') {
      printint(va_arg(ap, uint64), 10, 1);
      i += 1;
    } ਨਹੀਂਤਾਂ ਜੇ (c0 == 'l' && c1 == 'l' && c2 == 'd') {
      printint(va_arg(ap, uint64), 10, 1);
      i += 2;
    } ਨਹੀਂਤਾਂ ਜੇ (c0 == 'u') {
      printint(va_arg(ap, uint32), 10, 0);
    } ਨਹੀਂਤਾਂ ਜੇ (c0 == 'l' && c1 == 'u') {
      printint(va_arg(ap, uint64), 10, 0);
      i += 1;
    } ਨਹੀਂਤਾਂ ਜੇ (c0 == 'l' && c1 == 'l' && c2 == 'u') {
      printint(va_arg(ap, uint64), 10, 0);
      i += 2;
    } ਨਹੀਂਤਾਂ ਜੇ (c0 == 'x') {
      printint(va_arg(ap, uint32), 16, 0);
    } ਨਹੀਂਤਾਂ ਜੇ (c0 == 'l' && c1 == 'x') {
      printint(va_arg(ap, uint64), 16, 0);
      i += 1;
    } ਨਹੀਂਤਾਂ ਜੇ (c0 == 'l' && c1 == 'l' && c2 == 'x') {
      printint(va_arg(ap, uint64), 16, 0);
      i += 2;
    } ਨਹੀਂਤਾਂ ਜੇ (c0 == 'p') {
      printptr(va_arg(ap, uint64));
    } ਨਹੀਂਤਾਂ ਜੇ (c0 == 'c') {
      consputc(va_arg(ap, uint));
    } ਨਹੀਂਤਾਂ ਜੇ (c0 == 's') {
      ਜੇ ((s = va_arg(ap, ਅੱਖਰ *)) == 0)
        s = "(null)";
      ਲਈ (; *s; s++)
        consputc(*s);
    } ਨਹੀਂਤਾਂ ਜੇ (c0 == '%') {
      consputc('%');
    } ਨਹੀਂਤਾਂ ਜੇ (c0 == 0) {
      ਰੋਕੋ;
    } ਨਹੀਂਤਾਂ {
      // Print unknown % sequence to draw attention.
      consputc('%');
      consputc(c0);
    }
  }
  va_end(ap);

  ਜੇ (panicking == 0)
    release(&pr.ਤਾਲਾ);

  ਵਾਪਸ 0;
}

ਖਾਲੀ
panic(ਅੱਖਰ *s)
{
  panicking = 1;
  printk("panic: ");
  printk("%s\n", s);
  panicked = 1; // freeze uart output from other CPUs
  ਲਈ (;;)
    ;
}

ਖਾਲੀ
printkinit(ਖਾਲੀ)
{
  initlock(&pr.ਤਾਲਾ, "pr");
}
