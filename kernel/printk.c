//
// formatted console output -- printk, panic.
//

#അന്തര്ഭാവ <stdarg.h>

#അന്തര്ഭാവ "types.h"
#അന്തര്ഭാവ "param.h"
#അന്തര്ഭാവ "spinlock.h"
#അന്തര്ഭാവ "sleeplock.h"
#അന്തര്ഭാവ "fs.h"
#അന്തര്ഭാവ "file.h"
#അന്തര്ഭാവ "memlayout.h"
#അന്തര്ഭാവ "riscv.h"
#അന്തര്ഭാവ "defs.h"
#അന്തര്ഭാവ "proc.h"

അസ്ഥിര പൂർണ്ണസംഖ്യ panicking = 0; // printing a panic message
അസ്ഥിര പൂർണ്ണസംഖ്യ panicked = 0;  // spinning forever at end of a panic

// lock to avoid interleaving concurrent printk's.
സ്ഥിതം ഘടന {
  ഘടന spinlock lock;
} pr;

സ്ഥിതം അക്ഷരം digits[] = "0123456789abcdef";

സ്ഥിതം ശൂന്യം
printint(long long xx, പൂർണ്ണസംഖ്യ base, പൂർണ്ണസംഖ്യ sign)
{
  അക്ഷരം buf[20];
  പൂർണ്ണസംഖ്യ i;
  unsigned long long x;

  എങ്കിൽ (sign && (sign = (xx < 0)))
    x = -xx;
  അല്ലെങ്കിൽ
    x = xx;

  i = 0;
  ചെയ്യുക {
    buf[i++] = digits[x % base];
  } ആകുമ്പോൾ ((x /= base) != 0);

  എങ്കിൽ (sign)
    buf[i++] = '-';

  ആകുമ്പോൾ (--i >= 0)
    consputc(buf[i]);
}

സ്ഥിതം ശൂന്യം
printptr(uint64 x)
{
  പൂർണ്ണസംഖ്യ i;
  consputc('0');
  consputc('x');
  വേണ്ടി (i = 0; i < (വലുപ്പം(uint64) * 2); i++, x <<= 4)
    consputc(digits[x >> (വലുപ്പം(uint64) * 8 - 4)]);
}

// Print to the console.
പൂർണ്ണസംഖ്യ
printk(അക്ഷരം *fmt, ...)
{
  va_list ap;
  പൂർണ്ണസംഖ്യ i, cx, c0, c1, c2;
  അക്ഷരം *s;

  എങ്കിൽ (panicking == 0)
    acquire(&pr.lock);

  va_start(ap, fmt);
  വേണ്ടി (i = 0; (cx = fmt[i] & 0xff) != 0; i++) {
    എങ്കിൽ (cx != '%') {
      consputc(cx);
      തുടരുക;
    }
    i++;
    c0 = fmt[i + 0] & 0xff;
    c1 = c2 = 0;
    എങ്കിൽ (c0)
      c1 = fmt[i + 1] & 0xff;
    എങ്കിൽ (c1)
      c2 = fmt[i + 2] & 0xff;
    എങ്കിൽ (c0 == 'd') {
      printint(va_arg(ap, പൂർണ്ണസംഖ്യ), 10, 1);
    } അല്ലെങ്കിൽ എങ്കിൽ (c0 == 'l' && c1 == 'd') {
      printint(va_arg(ap, uint64), 10, 1);
      i += 1;
    } അല്ലെങ്കിൽ എങ്കിൽ (c0 == 'l' && c1 == 'l' && c2 == 'd') {
      printint(va_arg(ap, uint64), 10, 1);
      i += 2;
    } അല്ലെങ്കിൽ എങ്കിൽ (c0 == 'u') {
      printint(va_arg(ap, uint32), 10, 0);
    } അല്ലെങ്കിൽ എങ്കിൽ (c0 == 'l' && c1 == 'u') {
      printint(va_arg(ap, uint64), 10, 0);
      i += 1;
    } അല്ലെങ്കിൽ എങ്കിൽ (c0 == 'l' && c1 == 'l' && c2 == 'u') {
      printint(va_arg(ap, uint64), 10, 0);
      i += 2;
    } അല്ലെങ്കിൽ എങ്കിൽ (c0 == 'x') {
      printint(va_arg(ap, uint32), 16, 0);
    } അല്ലെങ്കിൽ എങ്കിൽ (c0 == 'l' && c1 == 'x') {
      printint(va_arg(ap, uint64), 16, 0);
      i += 1;
    } അല്ലെങ്കിൽ എങ്കിൽ (c0 == 'l' && c1 == 'l' && c2 == 'x') {
      printint(va_arg(ap, uint64), 16, 0);
      i += 2;
    } അല്ലെങ്കിൽ എങ്കിൽ (c0 == 'p') {
      printptr(va_arg(ap, uint64));
    } അല്ലെങ്കിൽ എങ്കിൽ (c0 == 'c') {
      consputc(va_arg(ap, uint));
    } അല്ലെങ്കിൽ എങ്കിൽ (c0 == 's') {
      എങ്കിൽ ((s = va_arg(ap, അക്ഷരം *)) == 0)
        s = "(null)";
      വേണ്ടി (; *s; s++)
        consputc(*s);
    } അല്ലെങ്കിൽ എങ്കിൽ (c0 == '%') {
      consputc('%');
    } അല്ലെങ്കിൽ എങ്കിൽ (c0 == 0) {
      നിർത്തുക;
    } അല്ലെങ്കിൽ {
      // Print unknown % sequence to draw attention.
      consputc('%');
      consputc(c0);
    }
  }
  va_end(ap);

  എങ്കിൽ (panicking == 0)
    release(&pr.lock);

  തിരികെ 0;
}

ശൂന്യം
panic(അക്ഷരം *s)
{
  panicking = 1;
  printk("panic: ");
  printk("%s\n", s);
  panicked = 1; // freeze uart output from other CPUs
  വേണ്ടി (;;)
    ;
}

ശൂന്യം
printkinit(ശൂന്യം)
{
  initlock(&pr.lock, "pr");
}
