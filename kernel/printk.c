//
// formatted console output -- printk, panic.
//

#ಅನ್ತರ್ಭಾವ <stdarg.h>

#ಅನ್ತರ್ಭಾವ "types.h"
#ಅನ್ತರ್ಭಾವ "param.h"
#ಅನ್ತರ್ಭಾವ "spinlock.h"
#ಅನ್ತರ್ಭಾವ "sleeplock.h"
#ಅನ್ತರ್ಭಾವ "fs.h"
#ಅನ್ತರ್ಭಾವ "file.h"
#ಅನ್ತರ್ಭಾವ "memlayout.h"
#ಅನ್ತರ್ಭಾವ "riscv.h"
#ಅನ್ತರ್ಭಾವ "defs.h"
#ಅನ್ತರ್ಭಾವ "proc.h"

ಚಞ್ಚಲ ಪೂರ್ಣಾಙ್ಕ panicking = 0; // printing a panic message
ಚಞ್ಚಲ ಪೂರ್ಣಾಙ್ಕ panicked = 0;  // spinning forever at end of a panic

// lock to avoid interleaving concurrent printk's.
ಸ್ಥಿರ ಸಂರಚನಾ {
  ಸಂರಚನಾ spinlock lock;
} pr;

ಸ್ಥಿರ ವರ್ಣ digits[] = "0123456789abcdef";

ಸ್ಥಿರ ಶೂನ್ಯ
printint(long long xx, ಪೂರ್ಣಾಙ್ಕ base, ಪೂರ್ಣಾಙ್ಕ sign)
{
  ವರ್ಣ buf[20];
  ಪೂರ್ಣಾಙ್ಕ i;
  unsigned long long x;

  ಯದಿ (sign && (sign = (xx < 0)))
    x = -xx;
  ಅನ್ಯಥಾ
    x = xx;

  i = 0;
  ಕುರು {
    buf[i++] = digits[x % base];
  } ಯಾವತ್ ((x /= base) != 0);

  ಯದಿ (sign)
    buf[i++] = '-';

  ಯಾವತ್ (--i >= 0)
    consputc(buf[i]);
}

ಸ್ಥಿರ ಶೂನ್ಯ
printptr(uint64 x)
{
  ಪೂರ್ಣಾಙ್ಕ i;
  consputc('0');
  consputc('x');
  ಪ್ರತಿ (i = 0; i < (ಪರಿಮಾಣ(uint64) * 2); i++, x <<= 4)
    consputc(digits[x >> (ಪರಿಮಾಣ(uint64) * 8 - 4)]);
}

// Print to the console.
ಪೂರ್ಣಾಙ್ಕ
printk(ವರ್ಣ *fmt, ...)
{
  va_list ap;
  ಪೂರ್ಣಾಙ್ಕ i, cx, c0, c1, c2;
  ವರ್ಣ *s;

  ಯದಿ (panicking == 0)
    acquire(&pr.lock);

  va_start(ap, fmt);
  ಪ್ರತಿ (i = 0; (cx = fmt[i] & 0xff) != 0; i++) {
    ಯದಿ (cx != '%') {
      consputc(cx);
      ಅನುವರ್ತ;
    }
    i++;
    c0 = fmt[i + 0] & 0xff;
    c1 = c2 = 0;
    ಯದಿ (c0)
      c1 = fmt[i + 1] & 0xff;
    ಯದಿ (c1)
      c2 = fmt[i + 2] & 0xff;
    ಯದಿ (c0 == 'd') {
      printint(va_arg(ap, ಪೂರ್ಣಾಙ್ಕ), 10, 1);
    } ಅನ್ಯಥಾ ಯದಿ (c0 == 'l' && c1 == 'd') {
      printint(va_arg(ap, uint64), 10, 1);
      i += 1;
    } ಅನ್ಯಥಾ ಯದಿ (c0 == 'l' && c1 == 'l' && c2 == 'd') {
      printint(va_arg(ap, uint64), 10, 1);
      i += 2;
    } ಅನ್ಯಥಾ ಯದಿ (c0 == 'u') {
      printint(va_arg(ap, uint32), 10, 0);
    } ಅನ್ಯಥಾ ಯದಿ (c0 == 'l' && c1 == 'u') {
      printint(va_arg(ap, uint64), 10, 0);
      i += 1;
    } ಅನ್ಯಥಾ ಯದಿ (c0 == 'l' && c1 == 'l' && c2 == 'u') {
      printint(va_arg(ap, uint64), 10, 0);
      i += 2;
    } ಅನ್ಯಥಾ ಯದಿ (c0 == 'x') {
      printint(va_arg(ap, uint32), 16, 0);
    } ಅನ್ಯಥಾ ಯದಿ (c0 == 'l' && c1 == 'x') {
      printint(va_arg(ap, uint64), 16, 0);
      i += 1;
    } ಅನ್ಯಥಾ ಯದಿ (c0 == 'l' && c1 == 'l' && c2 == 'x') {
      printint(va_arg(ap, uint64), 16, 0);
      i += 2;
    } ಅನ್ಯಥಾ ಯದಿ (c0 == 'p') {
      printptr(va_arg(ap, uint64));
    } ಅನ್ಯಥಾ ಯದಿ (c0 == 'c') {
      consputc(va_arg(ap, uint));
    } ಅನ್ಯಥಾ ಯದಿ (c0 == 's') {
      ಯದಿ ((s = va_arg(ap, ವರ್ಣ *)) == 0)
        s = "(null)";
      ಪ್ರತಿ (; *s; s++)
        consputc(*s);
    } ಅನ್ಯಥಾ ಯದಿ (c0 == '%') {
      consputc('%');
    } ಅನ್ಯಥಾ ಯದಿ (c0 == 0) {
      ಭಙ್ಗ;
    } ಅನ್ಯಥಾ {
      // Print unknown % sequence to draw attention.
      consputc('%');
      consputc(c0);
    }
  }
  va_end(ap);

  ಯದಿ (panicking == 0)
    release(&pr.lock);

  ಪ್ರತ್ಯಾಗಮ 0;
}

ಶೂನ್ಯ
panic(ವರ್ಣ *s)
{
  panicking = 1;
  printk("panic: ");
  printk("%s\n", s);
  panicked = 1; // freeze uart output from other CPUs
  ಪ್ರತಿ (;;)
    ;
}

ಶೂನ್ಯ
printkinit(ಶೂನ್ಯ)
{
  initlock(&pr.lock, "pr");
}
