//
// Console input and output, to the uart.
// Reads are line at a time.
// Implements special input characters:
//   newline -- end of line
//   control-h -- backspace
//   control-u -- kill line
//   control-d -- end of file
//   control-p -- print process list
//

#ضمن <stdarg.h>

#ضمن "types.h"
#ضمن "param.h"
#ضمن "spinlock.h"
#ضمن "sleeplock.h"
#ضمن "fs.h"
#ضمن "file.h"
#ضمن "memlayout.h"
#ضمن "riscv.h"
#ضمن "defs.h"
#ضمن "proc.h"

#تعریف_کرو BACKSPACE 0x100       // erase the last output character
#تعریف_کرو C(x)      ((x) - '@') // Control-x

//
// send one character to the uart, but don't use
// interrupts or sleep(). safe to be called from
// interrupts, e.g. by printk and to echo input
// characters.
//
فراغ
consputc(صحيح c)
{
  إذا (c == BACKSPACE) {
    // if the user typed backspace, overwrite with a space.
    uartputc_sync('\b');
    uartputc_sync(' ');
    uartputc_sync('\b');
  } وإلا {
    uartputc_sync(c);
  }
}

بنية {
  بنية spinlock جندرا;

  // input circular buffer
#تعریف_کرو داخلہ_BUF_ناپ 128
  حرف buf[داخلہ_BUF_ناپ];
  uint r; // Read index
  uint w; // Write index
  uint e; // Edit index
} cons;

//
// user write() system calls to the console go here.
// uses sleep() and UART interrupts.
//
صحيح
consolewrite(صحيح user_src, uint64 src, صحيح n)
{
  حرف buf[32]; // move batches from user space to uart.
  صحيح i = 0;

  طالما (i < n) {
    صحيح nn = حجم(buf);
    إذا (nn > n - i)
      nn = n - i;
    إذا (either_copyin(buf, user_src, src + i, nn) == -1)
      اكسر;
    uartwrite(buf, nn);
    i += nn;
  }

  أرجع i;
}

//
// user read()s from the console go here.
// copy (up to) a whole input line to dst.
// user_dst indicates whether dst is a user
// or kernel address.
//
صحيح
consoleread(صحيح user_dst, uint64 dst, صحيح n)
{
  uint نشانہ;
  صحيح c;
  حرف cbuf;

  نشانہ = n;
  acquire(&cons.جندرا);
  طالما (n > 0) {
    // wait until interrupt handler has put some
    // input into cons.buffer.
    طالما (cons.r == cons.w) {
      إذا (killed(myproc())) {
        release(&cons.جندرا);
        أرجع -1;
      }
      sleep_prepare(&cons.r);
      release(&cons.جندرا);
      sleep();
      acquire(&cons.جندرا);
    }

    c = cons.buf[cons.r++ % داخلہ_BUF_ناپ];

    إذا (c == C('D')) { // end-of-file
      إذا (n < نشانہ) {
        // Save ^D for next time, to make sure
        // caller gets a 0-byte result.
        cons.r--;
      }
      اكسر;
    }

    // copy the input byte to the user-space buffer.
    cbuf = c;
    إذا (either_copyout(user_dst, dst, &cbuf, 1) == -1)
      اكسر;

    dst++;
    --n;

    إذا (c == '\n') {
      // a whole line has arrived, return to
      // the user-level read().
      اكسر;
    }
  }
  release(&cons.جندرا);

  أرجع نشانہ - n;
}

//
// the console input interrupt handler.
// uartintr() calls this for each input character.
// do erase/kill processing, append to cons.buf,
// wake up consoleread() if a whole line has arrived.
//
فراغ
consoleintr(صحيح c)
{
  acquire(&cons.جندرا);

  بدل (c) {
  حالة C('P'): // Print process list.
    procdump();
    اكسر;
  حالة C('U'): // Kill line.
    طالما (cons.e != cons.w &&
           cons.buf[(cons.e - 1) % داخلہ_BUF_ناپ] != '\n') {
      cons.e--;
      consputc(BACKSPACE);
    }
    اكسر;
  حالة C('H'): // Backspace
  حالة '\x7f': // Delete key
    إذا (cons.e != cons.w) {
      cons.e--;
      consputc(BACKSPACE);
    }
    اكسر;
  افتراضي:
    إذا (c != 0 && cons.e - cons.r < داخلہ_BUF_ناپ) {
      c = (c == '\r') ? '\n' : c;

      // echo back to the user.
      consputc(c);

      // store for consumption by consoleread().
      cons.buf[cons.e++ % داخلہ_BUF_ناپ] = c;

      إذا (c == '\n' || c == C('D') || cons.e - cons.r == داخلہ_BUF_ناپ) {
        // wake up consoleread() if a whole line (or end-of-file)
        // has arrived.
        cons.w = cons.e;
        wakeup(&cons.r);
      }
    }
    اكسر;
  }

  release(&cons.جندرا);
}

فراغ
consoleinit(فراغ)
{
  initlock(&cons.جندرا, "cons");

  uartinit();

  // connect read and write system calls
  // to consoleread and consolewrite.
  devsw[CONSOLE].اقرأ = consoleread;
  devsw[CONSOLE].اكتب = consolewrite;
}
