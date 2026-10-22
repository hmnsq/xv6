//
// formatted console output -- printk, panic.
//

#அந்தர்பாவ <stdarg.h>

#அந்தர்பாவ "types.h"
#அந்தர்பாவ "param.h"
#அந்தர்பாவ "spinlock.h"
#அந்தர்பாவ "sleeplock.h"
#அந்தர்பாவ "fs.h"
#அந்தர்பாவ "file.h"
#அந்தர்பாவ "memlayout.h"
#அந்தர்பாவ "riscv.h"
#அந்தர்பாவ "defs.h"
#அந்தர்பாவ "proc.h"

நிலையற்ற முழு panicking = 0; // printing a panic message
நிலையற்ற முழு panicked = 0;  // spinning forever at end of a panic

// lock to avoid interleaving concurrent printk's.
நிலையான கட்டமைப்பு {
  கட்டமைப்பு spinlock lock;
} pr;

நிலையான எழுத்து digits[] = "0123456789abcdef";

நிலையான வெறுமை
printint(long long xx, முழு base, முழு sign)
{
  எழுத்து buf[20];
  முழு i;
  unsigned long long x;

  எனில் (sign && (sign = (xx < 0)))
    x = -xx;
  இல்லையெனில்
    x = xx;

  i = 0;
  செய் {
    buf[i++] = digits[x % base];
  } போது ((x /= base) != 0);

  எனில் (sign)
    buf[i++] = '-';

  போது (--i >= 0)
    consputc(buf[i]);
}

நிலையான வெறுமை
printptr(uint64 x)
{
  முழு i;
  consputc('0');
  consputc('x');
  க்கு (i = 0; i < (அளவு(uint64) * 2); i++, x <<= 4)
    consputc(digits[x >> (அளவு(uint64) * 8 - 4)]);
}

// Print to the console.
முழு
printk(எழுத்து *fmt, ...)
{
  va_list ap;
  முழு i, cx, c0, c1, c2;
  எழுத்து *s;

  எனில் (panicking == 0)
    acquire(&pr.lock);

  va_start(ap, fmt);
  க்கு (i = 0; (cx = fmt[i] & 0xff) != 0; i++) {
    எனில் (cx != '%') {
      consputc(cx);
      தொடர்;
    }
    i++;
    c0 = fmt[i + 0] & 0xff;
    c1 = c2 = 0;
    எனில் (c0)
      c1 = fmt[i + 1] & 0xff;
    எனில் (c1)
      c2 = fmt[i + 2] & 0xff;
    எனில் (c0 == 'd') {
      printint(va_arg(ap, முழு), 10, 1);
    } இல்லையெனில் எனில் (c0 == 'l' && c1 == 'd') {
      printint(va_arg(ap, uint64), 10, 1);
      i += 1;
    } இல்லையெனில் எனில் (c0 == 'l' && c1 == 'l' && c2 == 'd') {
      printint(va_arg(ap, uint64), 10, 1);
      i += 2;
    } இல்லையெனில் எனில் (c0 == 'u') {
      printint(va_arg(ap, uint32), 10, 0);
    } இல்லையெனில் எனில் (c0 == 'l' && c1 == 'u') {
      printint(va_arg(ap, uint64), 10, 0);
      i += 1;
    } இல்லையெனில் எனில் (c0 == 'l' && c1 == 'l' && c2 == 'u') {
      printint(va_arg(ap, uint64), 10, 0);
      i += 2;
    } இல்லையெனில் எனில் (c0 == 'x') {
      printint(va_arg(ap, uint32), 16, 0);
    } இல்லையெனில் எனில் (c0 == 'l' && c1 == 'x') {
      printint(va_arg(ap, uint64), 16, 0);
      i += 1;
    } இல்லையெனில் எனில் (c0 == 'l' && c1 == 'l' && c2 == 'x') {
      printint(va_arg(ap, uint64), 16, 0);
      i += 2;
    } இல்லையெனில் எனில் (c0 == 'p') {
      printptr(va_arg(ap, uint64));
    } இல்லையெனில் எனில் (c0 == 'c') {
      consputc(va_arg(ap, uint));
    } இல்லையெனில் எனில் (c0 == 's') {
      எனில் ((s = va_arg(ap, எழுத்து *)) == 0)
        s = "(null)";
      க்கு (; *s; s++)
        consputc(*s);
    } இல்லையெனில் எனில் (c0 == '%') {
      consputc('%');
    } இல்லையெனில் எனில் (c0 == 0) {
      நிறுத்து;
    } இல்லையெனில் {
      // Print unknown % sequence to draw attention.
      consputc('%');
      consputc(c0);
    }
  }
  va_end(ap);

  எனில் (panicking == 0)
    release(&pr.lock);

  திரும்பு 0;
}

வெறுமை
panic(எழுத்து *s)
{
  panicking = 1;
  printk("panic: ");
  printk("%s\n", s);
  panicked = 1; // freeze uart output from other CPUs
  க்கு (;;)
    ;
}

வெறுமை
printkinit(வெறுமை)
{
  initlock(&pr.lock, "pr");
}
