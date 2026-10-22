//
// formatted console output -- printk, panic.
//

#અન્તર્ભાવ <stdarg.h>

#અન્તર્ભાવ "types.h"
#અન્તર્ભાવ "param.h"
#અન્તર્ભાવ "spinlock.h"
#અન્તર્ભાવ "sleeplock.h"
#અન્તર્ભાવ "fs.h"
#અન્તર્ભાવ "file.h"
#અન્તર્ભાવ "memlayout.h"
#અન્તર્ભાવ "riscv.h"
#અન્તર્ભાવ "defs.h"
#અન્તર્ભાવ "proc.h"

અસ્થિર પૂર્ણાંક panicking = 0; // printing a panic message
અસ્થિર પૂર્ણાંક panicked = 0;  // spinning forever at end of a panic

// lock to avoid interleaving concurrent printk's.
સ્થિર સંરચના {
  સંરચના spinlock lock;
} pr;

સ્થિર અક્ષર digits[] = "0123456789abcdef";

સ્થિર શૂન્ય
printint(long long xx, પૂર્ણાંક base, પૂર્ણાંક sign)
{
  અક્ષર buf[20];
  પૂર્ણાંક i;
  unsigned long long x;

  જો (sign && (sign = (xx < 0)))
    x = -xx;
  નહીંતર
    x = xx;

  i = 0;
  કરો {
    buf[i++] = digits[x % base];
  } જ્યારે ((x /= base) != 0);

  જો (sign)
    buf[i++] = '-';

  જ્યારે (--i >= 0)
    consputc(buf[i]);
}

સ્થિર શૂન્ય
printptr(uint64 x)
{
  પૂર્ણાંક i;
  consputc('0');
  consputc('x');
  માટે (i = 0; i < (કદ(uint64) * 2); i++, x <<= 4)
    consputc(digits[x >> (કદ(uint64) * 8 - 4)]);
}

// Print to the console.
પૂર્ણાંક
printk(અક્ષર *fmt, ...)
{
  va_list ap;
  પૂર્ણાંક i, cx, c0, c1, c2;
  અક્ષર *s;

  જો (panicking == 0)
    acquire(&pr.lock);

  va_start(ap, fmt);
  માટે (i = 0; (cx = fmt[i] & 0xff) != 0; i++) {
    જો (cx != '%') {
      consputc(cx);
      ચાલુ;
    }
    i++;
    c0 = fmt[i + 0] & 0xff;
    c1 = c2 = 0;
    જો (c0)
      c1 = fmt[i + 1] & 0xff;
    જો (c1)
      c2 = fmt[i + 2] & 0xff;
    જો (c0 == 'd') {
      printint(va_arg(ap, પૂર્ણાંક), 10, 1);
    } નહીંતર જો (c0 == 'l' && c1 == 'd') {
      printint(va_arg(ap, uint64), 10, 1);
      i += 1;
    } નહીંતર જો (c0 == 'l' && c1 == 'l' && c2 == 'd') {
      printint(va_arg(ap, uint64), 10, 1);
      i += 2;
    } નહીંતર જો (c0 == 'u') {
      printint(va_arg(ap, uint32), 10, 0);
    } નહીંતર જો (c0 == 'l' && c1 == 'u') {
      printint(va_arg(ap, uint64), 10, 0);
      i += 1;
    } નહીંતર જો (c0 == 'l' && c1 == 'l' && c2 == 'u') {
      printint(va_arg(ap, uint64), 10, 0);
      i += 2;
    } નહીંતર જો (c0 == 'x') {
      printint(va_arg(ap, uint32), 16, 0);
    } નહીંતર જો (c0 == 'l' && c1 == 'x') {
      printint(va_arg(ap, uint64), 16, 0);
      i += 1;
    } નહીંતર જો (c0 == 'l' && c1 == 'l' && c2 == 'x') {
      printint(va_arg(ap, uint64), 16, 0);
      i += 2;
    } નહીંતર જો (c0 == 'p') {
      printptr(va_arg(ap, uint64));
    } નહીંતર જો (c0 == 'c') {
      consputc(va_arg(ap, uint));
    } નહીંતર જો (c0 == 's') {
      જો ((s = va_arg(ap, અક્ષર *)) == 0)
        s = "(null)";
      માટે (; *s; s++)
        consputc(*s);
    } નહીંતર જો (c0 == '%') {
      consputc('%');
    } નહીંતર જો (c0 == 0) {
      તોડો;
    } નહીંતર {
      // Print unknown % sequence to draw attention.
      consputc('%');
      consputc(c0);
    }
  }
  va_end(ap);

  જો (panicking == 0)
    release(&pr.lock);

  પાછા 0;
}

શૂન્ય
panic(અક્ષર *s)
{
  panicking = 1;
  printk("panic: ");
  printk("%s\n", s);
  panicked = 1; // freeze uart output from other CPUs
  માટે (;;)
    ;
}

શૂન્ય
printkinit(શૂન્ય)
{
  initlock(&pr.lock, "pr");
}
