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
consputc(পূর্ণসংখ্যা c)
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

কাঠামো {
  কাঠামো spinlock lock;

  // input circular buffer
#define প্রবেশ_BUF_আকার 128
  অক্ষর buf[প্রবেশ_BUF_আকার];
  uint r; // Read index
  uint w; // Write index
  uint e; // Edit index
} cons;

//
// user write() system calls to the console go here.
// uses sleep() and UART interrupts.
//
পূর্ণসংখ্যা
consolewrite(পূর্ণসংখ্যা user_src, uint64 src, পূর্ণসংখ্যা n)
{
  অক্ষর buf[32]; // move batches from user space to uart.
  পূর্ণসংখ্যা i = 0;

  যখন (i < n) {
    পূর্ণসংখ্যা nn = আকার(buf);
    যদি (nn > n - i)
      nn = n - i;
    যদি (either_copyin(buf, user_src, src + i, nn) == -1)
      বিরতি;
    uartwrite(buf, nn);
    i += nn;
  }

  ফেরত i;
}

//
// user read()s from the console go here.
// copy (up to) a whole input line to dst.
// user_dst indicates whether dst is a user
// or kernel address.
//
পূর্ণসংখ্যা
consoleread(পূর্ণসংখ্যা user_dst, uint64 dst, পূর্ণসংখ্যা n)
{
  uint target;
  পূর্ণসংখ্যা c;
  অক্ষর cbuf;

  target = n;
  acquire(&cons.lock);
  যখন (n > 0) {
    // wait until interrupt handler has put some
    // input into cons.buffer.
    যখন (cons.r == cons.w) {
      যদি (killed(myproc())) {
        release(&cons.lock);
        ফেরত -1;
      }
      sleep_prepare(&cons.r);
      release(&cons.lock);
      sleep();
      acquire(&cons.lock);
    }

    c = cons.buf[cons.r++ % প্রবেশ_BUF_আকার];

    যদি (c == C('D')) { // end-of-file
      যদি (n < target) {
        // Save ^D for next time, to make sure
        // caller gets a 0-byte result.
        cons.r--;
      }
      বিরতি;
    }

    // copy the input byte to the user-space buffer.
    cbuf = c;
    যদি (either_copyout(user_dst, dst, &cbuf, 1) == -1)
      বিরতি;

    dst++;
    --n;

    যদি (c == '\n') {
      // a whole line has arrived, return to
      // the user-level read().
      বিরতি;
    }
  }
  release(&cons.lock);

  ফেরত target - n;
}

//
// the console input interrupt handler.
// uartintr() calls this for each input character.
// do erase/kill processing, append to cons.buf,
// wake up consoleread() if a whole line has arrived.
//
শূন্য
consoleintr(পূর্ণসংখ্যা c)
{
  acquire(&cons.lock);

  নির্বাচন (c) {
  ক্ষেত্র C('P'): // Print process list.
    procdump();
    বিরতি;
  ক্ষেত্র C('U'): // Kill line.
    যখন (cons.e != cons.w &&
           cons.buf[(cons.e - 1) % প্রবেশ_BUF_আকার] != '\n') {
      cons.e--;
      consputc(BACKSPACE);
    }
    বিরতি;
  ক্ষেত্র C('H'): // Backspace
  ক্ষেত্র '\x7f': // Delete key
    যদি (cons.e != cons.w) {
      cons.e--;
      consputc(BACKSPACE);
    }
    বিরতি;
  পূর্বনির্ধারিত:
    যদি (c != 0 && cons.e - cons.r < প্রবেশ_BUF_আকার) {
      c = (c == '\r') ? '\n' : c;

      // echo back to the user.
      consputc(c);

      // store for consumption by consoleread().
      cons.buf[cons.e++ % প্রবেশ_BUF_আকার] = c;

      যদি (c == '\n' || c == C('D') || cons.e - cons.r == প্রবেশ_BUF_আকার) {
        // wake up consoleread() if a whole line (or end-of-file)
        // has arrived.
        cons.w = cons.e;
        wakeup(&cons.r);
      }
    }
    বিরতি;
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
