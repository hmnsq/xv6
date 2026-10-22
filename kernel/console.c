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

#define BACKSPACE 0x100       // erase the last output character
#define C(x)      ((x) - '@') // Control-x

//
// send one character to the uart, but don't use
// interrupts or sleep(). safe to be called from
// interrupts, e.g. by printk and to echo input
// characters.
//
فراغ
consputc(صحيح c)
{
  𐤀𐤌 (c == BACKSPACE) {
    // if the user typed backspace, overwrite with a space.
    uartputc_sync('\b');
    uartputc_sync(' ');
    uartputc_sync('\b');
  } وإلا {
    uartputc_sync(c);
  }
}

بنية {
  بنية spinlock lock;

  // input circular buffer
#define INPUT_BUF_SIZE 128
  حرف buf[INPUT_BUF_SIZE];
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
    𐤀𐤌 (nn > n - i)
      nn = n - i;
    𐤀𐤌 (either_copyin(buf, user_src, src + i, nn) == -1)
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
  uint target;
  صحيح c;
  حرف cbuf;

  target = n;
  acquire(&cons.lock);
  طالما (n > 0) {
    // wait until interrupt handler has put some
    // input into cons.buffer.
    طالما (cons.r == cons.w) {
      𐤀𐤌 (killed(myproc())) {
        release(&cons.lock);
        أرجع -1;
      }
      sleep_prepare(&cons.r);
      release(&cons.lock);
      sleep();
      acquire(&cons.lock);
    }

    c = cons.buf[cons.r++ % INPUT_BUF_SIZE];

    𐤀𐤌 (c == C('D')) { // end-of-file
      𐤀𐤌 (n < target) {
        // Save ^D for next time, to make sure
        // caller gets a 0-byte result.
        cons.r--;
      }
      اكسر;
    }

    // copy the input byte to the user-space buffer.
    cbuf = c;
    𐤀𐤌 (either_copyout(user_dst, dst, &cbuf, 1) == -1)
      اكسر;

    dst++;
    --n;

    𐤀𐤌 (c == '\n') {
      // a whole line has arrived, return to
      // the user-level read().
      اكسر;
    }
  }
  release(&cons.lock);

  أرجع target - n;
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
  acquire(&cons.lock);

  بدل (c) {
  حالة C('P'): // Print process list.
    procdump();
    اكسر;
  حالة C('U'): // Kill line.
    طالما (cons.e != cons.w &&
           cons.buf[(cons.e - 1) % INPUT_BUF_SIZE] != '\n') {
      cons.e--;
      consputc(BACKSPACE);
    }
    اكسر;
  حالة C('H'): // Backspace
  حالة '\x7f': // Delete key
    𐤀𐤌 (cons.e != cons.w) {
      cons.e--;
      consputc(BACKSPACE);
    }
    اكسر;
  افتراضي:
    𐤀𐤌 (c != 0 && cons.e - cons.r < INPUT_BUF_SIZE) {
      c = (c == '\r') ? '\n' : c;

      // echo back to the user.
      consputc(c);

      // store for consumption by consoleread().
      cons.buf[cons.e++ % INPUT_BUF_SIZE] = c;

      𐤀𐤌 (c == '\n' || c == C('D') || cons.e - cons.r == INPUT_BUF_SIZE) {
        // wake up consoleread() if a whole line (or end-of-file)
        // has arrived.
        cons.w = cons.e;
        wakeup(&cons.r);
      }
    }
    اكسر;
  }

  release(&cons.lock);
}

فراغ
consoleinit(فراغ)
{
  initlock(&cons.lock, "cons");

  uartinit();

  // connect read and write system calls
  // to consoleread and consolewrite.
  devsw[CONSOLE].اقرأ = consoleread;
  devsw[CONSOLE].اكتب = consolewrite;
}
