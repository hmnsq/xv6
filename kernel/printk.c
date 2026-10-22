//
// formatted console output -- printk, panic.
//

#अन्तर्भाव <stdarg.h>

#अन्तर्भाव "types.h"
#अन्तर्भाव "param.h"
#अन्तर्भाव "spinlock.h"
#अन्तर्भाव "sleeplock.h"
#अन्तर्भाव "fs.h"
#अन्तर्भाव "file.h"
#अन्तर्भाव "memlayout.h"
#अन्तर्भाव "riscv.h"
#अन्तर्भाव "defs.h"
#अन्तर्भाव "proc.h"

चञ्चल पूर्णाङ्क panicking = 0; // printing a panic message
चञ्चल पूर्णाङ्क panicked = 0;  // spinning forever at end of a panic

// lock to avoid interleaving concurrent printk's.
स्थिर संरचना {
  संरचना spinlock lock;
} pr;

स्थिर वर्ण digits[] = "0123456789abcdef";

स्थिर शून्य
printint(long long xx, पूर्णाङ्क base, पूर्णाङ्क sign)
{
  वर्ण buf[20];
  पूर्णाङ्क i;
  unsigned long long x;

  जइ (sign && (sign = (xx < 0)))
    x = -xx;
  अण्णहा
    x = xx;

  i = 0;
  करेहि {
    buf[i++] = digits[x % base];
  } जाव ((x /= base) != 0);

  जइ (sign)
    buf[i++] = '-';

  जाव (--i >= 0)
    consputc(buf[i]);
}

स्थिर शून्य
printptr(uint64 x)
{
  पूर्णाङ्क i;
  consputc('0');
  consputc('x');
  प्रति (i = 0; i < (परिमाण(uint64) * 2); i++, x <<= 4)
    consputc(digits[x >> (परिमाण(uint64) * 8 - 4)]);
}

// Print to the console.
पूर्णाङ्क
printk(वर्ण *fmt, ...)
{
  va_list ap;
  पूर्णाङ्क i, cx, c0, c1, c2;
  वर्ण *s;

  जइ (panicking == 0)
    acquire(&pr.lock);

  va_start(ap, fmt);
  प्रति (i = 0; (cx = fmt[i] & 0xff) != 0; i++) {
    जइ (cx != '%') {
      consputc(cx);
      अनुवर्त;
    }
    i++;
    c0 = fmt[i + 0] & 0xff;
    c1 = c2 = 0;
    जइ (c0)
      c1 = fmt[i + 1] & 0xff;
    जइ (c1)
      c2 = fmt[i + 2] & 0xff;
    जइ (c0 == 'd') {
      printint(va_arg(ap, पूर्णाङ्क), 10, 1);
    } अण्णहा जइ (c0 == 'l' && c1 == 'd') {
      printint(va_arg(ap, uint64), 10, 1);
      i += 1;
    } अण्णहा जइ (c0 == 'l' && c1 == 'l' && c2 == 'd') {
      printint(va_arg(ap, uint64), 10, 1);
      i += 2;
    } अण्णहा जइ (c0 == 'u') {
      printint(va_arg(ap, uint32), 10, 0);
    } अण्णहा जइ (c0 == 'l' && c1 == 'u') {
      printint(va_arg(ap, uint64), 10, 0);
      i += 1;
    } अण्णहा जइ (c0 == 'l' && c1 == 'l' && c2 == 'u') {
      printint(va_arg(ap, uint64), 10, 0);
      i += 2;
    } अण्णहा जइ (c0 == 'x') {
      printint(va_arg(ap, uint32), 16, 0);
    } अण्णहा जइ (c0 == 'l' && c1 == 'x') {
      printint(va_arg(ap, uint64), 16, 0);
      i += 1;
    } अण्णहा जइ (c0 == 'l' && c1 == 'l' && c2 == 'x') {
      printint(va_arg(ap, uint64), 16, 0);
      i += 2;
    } अण्णहा जइ (c0 == 'p') {
      printptr(va_arg(ap, uint64));
    } अण्णहा जइ (c0 == 'c') {
      consputc(va_arg(ap, uint));
    } अण्णहा जइ (c0 == 's') {
      जइ ((s = va_arg(ap, वर्ण *)) == 0)
        s = "(null)";
      प्रति (; *s; s++)
        consputc(*s);
    } अण्णहा जइ (c0 == '%') {
      consputc('%');
    } अण्णहा जइ (c0 == 0) {
      भङ्ग;
    } अण्णहा {
      // Print unknown % sequence to draw attention.
      consputc('%');
      consputc(c0);
    }
  }
  va_end(ap);

  जइ (panicking == 0)
    release(&pr.lock);

  प्रत्यागम 0;
}

शून्य
panic(वर्ण *s)
{
  panicking = 1;
  printk("panic: ");
  printk("%s\n", s);
  panicked = 1; // freeze uart output from other CPUs
  प्रति (;;)
    ;
}

शून्य
printkinit(शून्य)
{
  initlock(&pr.lock, "pr");
}
