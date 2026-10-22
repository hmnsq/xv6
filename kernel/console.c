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

#অন্তর্ভাব <stdarg.h>

#অন্তর্ভাব "types.h"
#অন্তর্ভাব "param.h"
#অন্তর্ভাব "spinlock.h"
#অন্তর্ভাব "sleeplock.h"
#অন্তর্ভাব "fs.h"
#অন্তর্ভাব "file.h"
#অন্তর্ভাব "memlayout.h"
#অন্তর্ভাব "riscv.h"
#অন্তর্ভাব "defs.h"
#অন্তর্ভাব "proc.h"

#define BACKSPACE 0x100       // erase the last output character
#define C(x)      ((x) - '@') // Control-x

//
// send one character to the uart, but don't use
// interrupts or sleep(). safe to be called from
// interrupts, e.g. by printk and to echo input
// characters.
//
শূন্য
consputc(পূর্ণাঙ্ক c)
{
  যদি (c == BACKSPACE) {
    // if the user typed backspace, overwrite with a space.
    uartputc_sync('\b');
    uartputc_sync(' ');
    uartputc_sync('\b');
  } অন্যথা {
    uartputc_sync(c);
  }
}

সংরচনা {
  সংরচনা spinlock lock;

  // input circular buffer
#define প্রবেশ_BUF_SIZE 128
  বর্ণ buf[প্রবেশ_BUF_SIZE];
  uint r; // Read index
  uint w; // Write index
  uint e; // Edit index
} cons;

//
// user write() system calls to the console go here.
// uses sleep() and UART interrupts.
//
পূর্ণাঙ্ক
consolewrite(পূর্ণাঙ্ক user_src, uint64 src, পূর্ণাঙ্ক n)
{
  বর্ণ buf[32]; // move batches from user space to uart.
  পূর্ণাঙ্ক i = 0;

  যাবত্ (i < n) {
    পূর্ণাঙ্ক nn = পরিমাণ(buf);
    যদি (nn > n - i)
      nn = n - i;
    যদি (either_copyin(buf, user_src, src + i, nn) == -1)
      ভঙ্গ;
    uartwrite(buf, nn);
    i += nn;
  }

  প্রত্যাগম i;
}

//
// user read()s from the console go here.
// copy (up to) a whole input line to dst.
// user_dst indicates whether dst is a user
// or kernel address.
//
পূর্ণাঙ্ক
consoleread(পূর্ণাঙ্ক user_dst, uint64 dst, পূর্ণাঙ্ক n)
{
  uint target;
  পূর্ণাঙ্ক c;
  বর্ণ cbuf;

  target = n;
  acquire(&cons.lock);
  যাবত্ (n > 0) {
    // wait until interrupt handler has put some
    // input into cons.buffer.
    যাবত্ (cons.r == cons.w) {
      যদি (killed(myproc())) {
        release(&cons.lock);
        প্রত্যাগম -1;
      }
      sleep_prepare(&cons.r);
      release(&cons.lock);
      sleep();
      acquire(&cons.lock);
    }

    c = cons.buf[cons.r++ % প্রবেশ_BUF_SIZE];

    যদি (c == C('D')) { // end-of-file
      যদি (n < target) {
        // Save ^D for next time, to make sure
        // caller gets a 0-byte result.
        cons.r--;
      }
      ভঙ্গ;
    }

    // copy the input byte to the user-space buffer.
    cbuf = c;
    যদি (either_copyout(user_dst, dst, &cbuf, 1) == -1)
      ভঙ্গ;

    dst++;
    --n;

    যদি (c == '\n') {
      // a whole line has arrived, return to
      // the user-level read().
      ভঙ্গ;
    }
  }
  release(&cons.lock);

  প্রত্যাগম target - n;
}

//
// the console input interrupt handler.
// uartintr() calls this for each input character.
// do erase/kill processing, append to cons.buf,
// wake up consoleread() if a whole line has arrived.
//
শূন্য
consoleintr(পূর্ণাঙ্ক c)
{
  acquire(&cons.lock);

  বিকল্প (c) {
  পক্ষ C('P'): // Print process list.
    procdump();
    ভঙ্গ;
  পক্ষ C('U'): // Kill line.
    যাবত্ (cons.e != cons.w &&
           cons.buf[(cons.e - 1) % প্রবেশ_BUF_SIZE] != '\n') {
      cons.e--;
      consputc(BACKSPACE);
    }
    ভঙ্গ;
  পক্ষ C('H'): // Backspace
  পক্ষ '\x7f': // Delete key
    যদি (cons.e != cons.w) {
      cons.e--;
      consputc(BACKSPACE);
    }
    ভঙ্গ;
  স্বভাব:
    যদি (c != 0 && cons.e - cons.r < প্রবেশ_BUF_SIZE) {
      c = (c == '\r') ? '\n' : c;

      // echo back to the user.
      consputc(c);

      // store for consumption by consoleread().
      cons.buf[cons.e++ % প্রবেশ_BUF_SIZE] = c;

      যদি (c == '\n' || c == C('D') || cons.e - cons.r == প্রবেশ_BUF_SIZE) {
        // wake up consoleread() if a whole line (or end-of-file)
        // has arrived.
        cons.w = cons.e;
        wakeup(&cons.r);
      }
    }
    ভঙ্গ;
  }

  release(&cons.lock);
}

শূন্য
consoleinit(শূন্য)
{
  initlock(&cons.lock, "cons");

  uartinit();

  // connect read and write system calls
  // to consoleread and consolewrite.
  devsw[CONSOLE].পঠ = consoleread;
  devsw[CONSOLE].লিখ = consolewrite;
}
