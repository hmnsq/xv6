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

ಅಸ್ಥಿರ ಪೂರ್ಣಾಂಕ panicking = 0; // printing a panic message
ಅಸ್ಥಿರ ಪೂರ್ಣಾಂಕ panicked = 0;  // spinning forever at end of a panic

// lock to avoid interleaving concurrent printk's.
ಸ್ಥಿರ ರಚನೆ {
  ರಚನೆ spinlock ಬೀಗ;
} pr;

ಸ್ಥಿರ ಅಕ್ಷರ digits[] = "0123456789abcdef";

ಸ್ಥಿರ ಶೂನ್ಯ
printint(ದೀರ್ಘ ದೀರ್ಘ xx, ಪೂರ್ಣಾಂಕ ತಳ, ಪೂರ್ಣಾಂಕ sign)
{
  ಅಕ್ಷರ buf[20];
  ಪೂರ್ಣಾಂಕ i;
  ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ದೀರ್ಘ x;

  ಆದರೆ (sign && (sign = (xx < 0)))
    x = -xx;
  ಇಲ್ಲದಿದ್ದರೆ
    x = xx;

  i = 0;
  ಮಾಡು {
    buf[i++] = digits[x % ತಳ];
  } ಆಗ ((x /= ತಳ) != 0);

  ಆದರೆ (sign)
    buf[i++] = '-';

  ಆಗ (--i >= 0)
    consputc(buf[i]);
}

ಸ್ಥಿರ ಶೂನ್ಯ
printptr(uint64 x)
{
  ಪೂರ್ಣಾಂಕ i;
  consputc('0');
  consputc('x');
  ಗಾಗಿ (i = 0; i < (ಗಾತ್ರ(uint64) * 2); i++, x <<= 4)
    consputc(digits[x >> (ಗಾತ್ರ(uint64) * 8 - 4)]);
}

// Print to the console.
ಪೂರ್ಣಾಂಕ
printk(ಅಕ್ಷರ *fmt, ...)
{
  va_list ap;
  ಪೂರ್ಣಾಂಕ i, cx, c0, c1, c2;
  ಅಕ್ಷರ *s;

  ಆದರೆ (panicking == 0)
    acquire(&pr.ಬೀಗ);

  va_start(ap, fmt);
  ಗಾಗಿ (i = 0; (cx = fmt[i] & 0xff) != 0; i++) {
    ಆದರೆ (cx != '%') {
      consputc(cx);
      ಮುಂದುವರಿ;
    }
    i++;
    c0 = fmt[i + 0] & 0xff;
    c1 = c2 = 0;
    ಆದರೆ (c0)
      c1 = fmt[i + 1] & 0xff;
    ಆದರೆ (c1)
      c2 = fmt[i + 2] & 0xff;
    ಆದರೆ (c0 == 'd') {
      printint(va_arg(ap, ಪೂರ್ಣಾಂಕ), 10, 1);
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c0 == 'l' && c1 == 'd') {
      printint(va_arg(ap, uint64), 10, 1);
      i += 1;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c0 == 'l' && c1 == 'l' && c2 == 'd') {
      printint(va_arg(ap, uint64), 10, 1);
      i += 2;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c0 == 'u') {
      printint(va_arg(ap, uint32), 10, 0);
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c0 == 'l' && c1 == 'u') {
      printint(va_arg(ap, uint64), 10, 0);
      i += 1;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c0 == 'l' && c1 == 'l' && c2 == 'u') {
      printint(va_arg(ap, uint64), 10, 0);
      i += 2;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c0 == 'x') {
      printint(va_arg(ap, uint32), 16, 0);
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c0 == 'l' && c1 == 'x') {
      printint(va_arg(ap, uint64), 16, 0);
      i += 1;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c0 == 'l' && c1 == 'l' && c2 == 'x') {
      printint(va_arg(ap, uint64), 16, 0);
      i += 2;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c0 == 'p') {
      printptr(va_arg(ap, uint64));
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c0 == 'c') {
      consputc(va_arg(ap, uint));
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c0 == 's') {
      ಆದರೆ ((s = va_arg(ap, ಅಕ್ಷರ *)) == 0)
        s = "(null)";
      ಗಾಗಿ (; *s; s++)
        consputc(*s);
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c0 == '%') {
      consputc('%');
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c0 == 0) {
      ನಿಲ್ಲಿಸು;
    } ಇಲ್ಲದಿದ್ದರೆ {
      // Print unknown % sequence to draw attention.
      consputc('%');
      consputc(c0);
    }
  }
  va_end(ap);

  ಆದರೆ (panicking == 0)
    release(&pr.ಬೀಗ);

  ಹಿಂತಿರುಗಿ 0;
}

ಶೂನ್ಯ
panic(ಅಕ್ಷರ *s)
{
  panicking = 1;
  printk("panic: ");
  printk("%s\n", s);
  panicked = 1; // freeze uart output from other CPUs
  ಗಾಗಿ (;;)
    ;
}

ಶೂನ್ಯ
printkinit(ಶೂನ್ಯ)
{
  initlock(&pr.ಬೀಗ, "pr");
}
