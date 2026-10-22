//
// formatted console output -- printk, panic.
//

#समावेश <stdarg.h>

#समावेश "types.h"
#समावेश "param.h"
#समावेश "spinlock.h"
#समावेश "sleeplock.h"
#समावेश "fs.h"
#समावेश "file.h"
#समावेश "memlayout.h"
#समावेश "riscv.h"
#समावेश "defs.h"
#समावेश "proc.h"

अस्थिर पूर्णांक panicking = 0; // printing a panic message
अस्थिर पूर्णांक panicked = 0;  // spinning forever at end of a panic

// lock to avoid interleaving concurrent printk's.
अटल काष्ठा {
  काष्ठा spinlock lock;
} pr;

अटल अक्षर digits[] = "0123456789abcdef";

अटल व्योम
printint(दीर्घ दीर्घ xx, पूर्णांक base, पूर्णांक sign)
{
  अक्षर buf[20];
  पूर्णांक i;
  अचिन्हित दीर्घ दीर्घ x;

  अगर (sign && (sign = (xx < 0)))
    x = -xx;
  अन्यथा
    x = xx;

  i = 0;
  करो {
    buf[i++] = digits[x % base];
  } जबतक ((x /= base) != 0);

  अगर (sign)
    buf[i++] = '-';

  जबतक (--i >= 0)
    consputc(buf[i]);
}

अटल व्योम
printptr(uint64 x)
{
  पूर्णांक i;
  consputc('0');
  consputc('x');
  क्रम (i = 0; i < (माप(uint64) * 2); i++, x <<= 4)
    consputc(digits[x >> (माप(uint64) * 8 - 4)]);
}

// Print to the console.
पूर्णांक
printk(अक्षर *fmt, ...)
{
  बहु_सूची ap;
  पूर्णांक i, cx, c0, c1, c2;
  अक्षर *s;

  अगर (panicking == 0)
    acquire(&pr.lock);

  बहु_शुरू(ap, fmt);
  क्रम (i = 0; (cx = fmt[i] & 0xff) != 0; i++) {
    अगर (cx != '%') {
      consputc(cx);
      जारी;
    }
    i++;
    c0 = fmt[i + 0] & 0xff;
    c1 = c2 = 0;
    अगर (c0)
      c1 = fmt[i + 1] & 0xff;
    अगर (c1)
      c2 = fmt[i + 2] & 0xff;
    अगर (c0 == 'd') {
      printint(बहु_तर्क(ap, पूर्णांक), 10, 1);
    } अन्यथा अगर (c0 == 'l' && c1 == 'd') {
      printint(बहु_तर्क(ap, uint64), 10, 1);
      i += 1;
    } अन्यथा अगर (c0 == 'l' && c1 == 'l' && c2 == 'd') {
      printint(बहु_तर्क(ap, uint64), 10, 1);
      i += 2;
    } अन्यथा अगर (c0 == 'u') {
      printint(बहु_तर्क(ap, uint32), 10, 0);
    } अन्यथा अगर (c0 == 'l' && c1 == 'u') {
      printint(बहु_तर्क(ap, uint64), 10, 0);
      i += 1;
    } अन्यथा अगर (c0 == 'l' && c1 == 'l' && c2 == 'u') {
      printint(बहु_तर्क(ap, uint64), 10, 0);
      i += 2;
    } अन्यथा अगर (c0 == 'x') {
      printint(बहु_तर्क(ap, uint32), 16, 0);
    } अन्यथा अगर (c0 == 'l' && c1 == 'x') {
      printint(बहु_तर्क(ap, uint64), 16, 0);
      i += 1;
    } अन्यथा अगर (c0 == 'l' && c1 == 'l' && c2 == 'x') {
      printint(बहु_तर्क(ap, uint64), 16, 0);
      i += 2;
    } अन्यथा अगर (c0 == 'p') {
      printptr(बहु_तर्क(ap, uint64));
    } अन्यथा अगर (c0 == 'c') {
      consputc(बहु_तर्क(ap, uint));
    } अन्यथा अगर (c0 == 's') {
      अगर ((s = बहु_तर्क(ap, अक्षर *)) == 0)
        s = "(null)";
      क्रम (; *s; s++)
        consputc(*s);
    } अन्यथा अगर (c0 == '%') {
      consputc('%');
    } अन्यथा अगर (c0 == 0) {
      अवरोध;
    } अन्यथा {
      // Print unknown % sequence to draw attention.
      consputc('%');
      consputc(c0);
    }
  }
  बहु_पूर्ण(ap);

  अगर (panicking == 0)
    release(&pr.lock);

  वापस 0;
}

व्योम
panic(अक्षर *s)
{
  panicking = 1;
  printk("panic: ");
  printk("%s\n", s);
  panicked = 1; // freeze uart output from other CPUs
  क्रम (;;)
    ;
}

व्योम
printkinit(व्योम)
{
  initlock(&pr.lock, "pr");
}
