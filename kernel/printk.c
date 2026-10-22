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

अस्थिर पूर्णांक panicking = 0; // printing a panic message
अस्थिर पूर्णांक panicked = 0;  // spinning forever at end of a panic

// lock to avoid interleaving concurrent printk's.
स्थिर संरचना {
  संरचना spinlock lock;
} pr;

स्थिर अक्षर digits[] = "0123456789abcdef";

स्थिर शून्य
printint(दीर्घ दीर्घ xx, पूर्णांक base, पूर्णांक sign)
{
  अक्षर buf[20];
  पूर्णांक i;
  अचिन्हित दीर्घ दीर्घ x;

  जर (sign && (sign = (xx < 0)))
    x = -xx;
  अन्यथा
    x = xx;

  i = 0;
  करा {
    buf[i++] = digits[x % base];
  } पर्यंत ((x /= base) != 0);

  जर (sign)
    buf[i++] = '-';

  पर्यंत (--i >= 0)
    consputc(buf[i]);
}

स्थिर शून्य
printptr(uint64 x)
{
  पूर्णांक i;
  consputc('0');
  consputc('x');
  साठी (i = 0; i < (आकार(uint64) * 2); i++, x <<= 4)
    consputc(digits[x >> (आकार(uint64) * 8 - 4)]);
}

// Print to the console.
पूर्णांक
printk(अक्षर *fmt, ...)
{
  बहु_सूची ap;
  पूर्णांक i, cx, c0, c1, c2;
  अक्षर *s;

  जर (panicking == 0)
    acquire(&pr.lock);

  बहु_शुरू(ap, fmt);
  साठी (i = 0; (cx = fmt[i] & 0xff) != 0; i++) {
    जर (cx != '%') {
      consputc(cx);
      सुरूठेवा;
    }
    i++;
    c0 = fmt[i + 0] & 0xff;
    c1 = c2 = 0;
    जर (c0)
      c1 = fmt[i + 1] & 0xff;
    जर (c1)
      c2 = fmt[i + 2] & 0xff;
    जर (c0 == 'd') {
      printint(बहु_तर्क(ap, पूर्णांक), 10, 1);
    } अन्यथा जर (c0 == 'l' && c1 == 'd') {
      printint(बहु_तर्क(ap, uint64), 10, 1);
      i += 1;
    } अन्यथा जर (c0 == 'l' && c1 == 'l' && c2 == 'd') {
      printint(बहु_तर्क(ap, uint64), 10, 1);
      i += 2;
    } अन्यथा जर (c0 == 'u') {
      printint(बहु_तर्क(ap, uint32), 10, 0);
    } अन्यथा जर (c0 == 'l' && c1 == 'u') {
      printint(बहु_तर्क(ap, uint64), 10, 0);
      i += 1;
    } अन्यथा जर (c0 == 'l' && c1 == 'l' && c2 == 'u') {
      printint(बहु_तर्क(ap, uint64), 10, 0);
      i += 2;
    } अन्यथा जर (c0 == 'x') {
      printint(बहु_तर्क(ap, uint32), 16, 0);
    } अन्यथा जर (c0 == 'l' && c1 == 'x') {
      printint(बहु_तर्क(ap, uint64), 16, 0);
      i += 1;
    } अन्यथा जर (c0 == 'l' && c1 == 'l' && c2 == 'x') {
      printint(बहु_तर्क(ap, uint64), 16, 0);
      i += 2;
    } अन्यथा जर (c0 == 'p') {
      printptr(बहु_तर्क(ap, uint64));
    } अन्यथा जर (c0 == 'c') {
      consputc(बहु_तर्क(ap, uint));
    } अन्यथा जर (c0 == 's') {
      जर ((s = बहु_तर्क(ap, अक्षर *)) == 0)
        s = "(null)";
      साठी (; *s; s++)
        consputc(*s);
    } अन्यथा जर (c0 == '%') {
      consputc('%');
    } अन्यथा जर (c0 == 0) {
      थांबा;
    } अन्यथा {
      // Print unknown % sequence to draw attention.
      consputc('%');
      consputc(c0);
    }
  }
  बहु_पूर्ण(ap);

  जर (panicking == 0)
    release(&pr.lock);

  परत 0;
}

शून्य
panic(अक्षर *s)
{
  panicking = 1;
  printk("panic: ");
  printk("%s\n", s);
  panicked = 1; // freeze uart output from other CPUs
  साठी (;;)
    ;
}

शून्य
printkinit(शून्य)
{
  initlock(&pr.lock, "pr");
}
