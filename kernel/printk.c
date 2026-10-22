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
printint(दीर्घ दीर्घ xx, पूर्णाङ्क base, पूर्णाङ्क sign)
{
  वर्ण buf[20];
  पूर्णाङ्क i;
  अचिन्हित दीर्घ दीर्घ x;

  यदि (sign && (sign = (xx < 0)))
    x = -xx;
  अन्यथा
    x = xx;

  i = 0;
  गर {
    buf[i++] = digits[x % base];
  } जबसम्म ((x /= base) != 0);

  यदि (sign)
    buf[i++] = '-';

  जबसम्म (--i >= 0)
    consputc(buf[i]);
}

स्थिर शून्य
printptr(uint64 x)
{
  पूर्णाङ्क i;
  consputc('0');
  consputc('x');
  लागि (i = 0; i < (परिमाण(uint64) * 2); i++, x <<= 4)
    consputc(digits[x >> (परिमाण(uint64) * 8 - 4)]);
}

// Print to the console.
पूर्णाङ्क
printk(वर्ण *fmt, ...)
{
  बहु_सूची ap;
  पूर्णाङ्क i, cx, c0, c1, c2;
  वर्ण *s;

  यदि (panicking == 0)
    acquire(&pr.lock);

  बहु_शुरू(ap, fmt);
  लागि (i = 0; (cx = fmt[i] & 0xff) != 0; i++) {
    यदि (cx != '%') {
      consputc(cx);
      जारी;
    }
    i++;
    c0 = fmt[i + 0] & 0xff;
    c1 = c2 = 0;
    यदि (c0)
      c1 = fmt[i + 1] & 0xff;
    यदि (c1)
      c2 = fmt[i + 2] & 0xff;
    यदि (c0 == 'd') {
      printint(बहु_तर्क(ap, पूर्णाङ्क), 10, 1);
    } अन्यथा यदि (c0 == 'l' && c1 == 'd') {
      printint(बहु_तर्क(ap, uint64), 10, 1);
      i += 1;
    } अन्यथा यदि (c0 == 'l' && c1 == 'l' && c2 == 'd') {
      printint(बहु_तर्क(ap, uint64), 10, 1);
      i += 2;
    } अन्यथा यदि (c0 == 'u') {
      printint(बहु_तर्क(ap, uint32), 10, 0);
    } अन्यथा यदि (c0 == 'l' && c1 == 'u') {
      printint(बहु_तर्क(ap, uint64), 10, 0);
      i += 1;
    } अन्यथा यदि (c0 == 'l' && c1 == 'l' && c2 == 'u') {
      printint(बहु_तर्क(ap, uint64), 10, 0);
      i += 2;
    } अन्यथा यदि (c0 == 'x') {
      printint(बहु_तर्क(ap, uint32), 16, 0);
    } अन्यथा यदि (c0 == 'l' && c1 == 'x') {
      printint(बहु_तर्क(ap, uint64), 16, 0);
      i += 1;
    } अन्यथा यदि (c0 == 'l' && c1 == 'l' && c2 == 'x') {
      printint(बहु_तर्क(ap, uint64), 16, 0);
      i += 2;
    } अन्यथा यदि (c0 == 'p') {
      printptr(बहु_तर्क(ap, uint64));
    } अन्यथा यदि (c0 == 'c') {
      consputc(बहु_तर्क(ap, uint));
    } अन्यथा यदि (c0 == 's') {
      यदि ((s = बहु_तर्क(ap, वर्ण *)) == 0)
        s = "(null)";
      लागि (; *s; s++)
        consputc(*s);
    } अन्यथा यदि (c0 == '%') {
      consputc('%');
    } अन्यथा यदि (c0 == 0) {
      रोक;
    } अन्यथा {
      // Print unknown % sequence to draw attention.
      consputc('%');
      consputc(c0);
    }
  }
  बहु_पूर्ण(ap);

  यदि (panicking == 0)
    release(&pr.lock);

  फर्काऊ 0;
}

शून्य
panic(वर्ण *s)
{
  panicking = 1;
  printk("panic: ");
  printk("%s\n", s);
  panicked = 1; // freeze uart output from other CPUs
  लागि (;;)
    ;
}

शून्य
printkinit(शून्य)
{
  initlock(&pr.lock, "pr");
}
