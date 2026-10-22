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

#شامل <stdarg.h>

#شامل "types.h"
#شامل "param.h"
#شامل "spinlock.h"
#شامل "sleeplock.h"
#شامل "fs.h"
#شامل "file.h"
#شامل "memlayout.h"
#شامل "riscv.h"
#شامل "defs.h"
#شامل "proc.h"

#define BACKSPACE 0x100       // erase the last output character
#define C(x)      ((x) - '@') // Control-x

//
// send one character to the uart, but don't use
// interrupts or sleep(). safe to be called from
// interrupts, e.g. by printk and to echo input
// characters.
//
تهی
consputc(صحیح c)
{
  اگر (c == BACKSPACE) {
    // if the user typed backspace, overwrite with a space.
    uartputc_sync('\b');
    uartputc_sync(' ');
    uartputc_sync('\b');
  } وگرنه {
    uartputc_sync(c);
  }
}

ساختار {
  ساختار spinlock lock;

  // input circular buffer
#define درون_داد_BUF_SIZE 128
  نویسه buf[درون_داد_BUF_SIZE];
  uint r; // Read index
  uint w; // Write index
  uint e; // Edit index
} cons;

//
// user write() system calls to the console go here.
// uses sleep() and UART interrupts.
//
صحیح
consolewrite(صحیح user_src, uint64 src, صحیح n)
{
  نویسه buf[32]; // move batches from user space to uart.
  صحیح i = 0;

  وقتی (i < n) {
    صحیح nn = اندازه(buf);
    اگر (nn > n - i)
      nn = n - i;
    اگر (either_copyin(buf, user_src, src + i, nn) == -1)
      بشکن;
    uartwrite(buf, nn);
    i += nn;
  }

  بازگشت i;
}

//
// user read()s from the console go here.
// copy (up to) a whole input line to dst.
// user_dst indicates whether dst is a user
// or kernel address.
//
صحیح
consoleread(صحیح user_dst, uint64 dst, صحیح n)
{
  uint target;
  صحیح c;
  نویسه cbuf;

  target = n;
  acquire(&cons.lock);
  وقتی (n > 0) {
    // wait until interrupt handler has put some
    // input into cons.buffer.
    وقتی (cons.r == cons.w) {
      اگر (killed(myproc())) {
        release(&cons.lock);
        بازگشت -1;
      }
      sleep_prepare(&cons.r);
      release(&cons.lock);
      sleep();
      acquire(&cons.lock);
    }

    c = cons.buf[cons.r++ % درون_داد_BUF_SIZE];

    اگر (c == C('D')) { // end-of-file
      اگر (n < target) {
        // Save ^D for next time, to make sure
        // caller gets a 0-byte result.
        cons.r--;
      }
      بشکن;
    }

    // copy the input byte to the user-space buffer.
    cbuf = c;
    اگر (either_copyout(user_dst, dst, &cbuf, 1) == -1)
      بشکن;

    dst++;
    --n;

    اگر (c == '\n') {
      // a whole line has arrived, return to
      // the user-level read().
      بشکن;
    }
  }
  release(&cons.lock);

  بازگشت target - n;
}

//
// the console input interrupt handler.
// uartintr() calls this for each input character.
// do erase/kill processing, append to cons.buf,
// wake up consoleread() if a whole line has arrived.
//
تهی
consoleintr(صحیح c)
{
  acquire(&cons.lock);

  انتخاب (c) {
  حالت C('P'): // Print process list.
    procdump();
    بشکن;
  حالت C('U'): // Kill line.
    وقتی (cons.e != cons.w &&
           cons.buf[(cons.e - 1) % درون_داد_BUF_SIZE] != '\n') {
      cons.e--;
      consputc(BACKSPACE);
    }
    بشکن;
  حالت C('H'): // Backspace
  حالت '\x7f': // Delete key
    اگر (cons.e != cons.w) {
      cons.e--;
      consputc(BACKSPACE);
    }
    بشکن;
  پیشفرض:
    اگر (c != 0 && cons.e - cons.r < درون_داد_BUF_SIZE) {
      c = (c == '\r') ? '\n' : c;

      // echo back to the user.
      consputc(c);

      // store for consumption by consoleread().
      cons.buf[cons.e++ % درون_داد_BUF_SIZE] = c;

      اگر (c == '\n' || c == C('D') || cons.e - cons.r == درون_داد_BUF_SIZE) {
        // wake up consoleread() if a whole line (or end-of-file)
        // has arrived.
        cons.w = cons.e;
        wakeup(&cons.r);
      }
    }
    بشکن;
  }

  release(&cons.lock);
}

تهی
consoleinit(تهی)
{
  initlock(&cons.lock, "cons");

  uartinit();

  // connect read and write system calls
  // to consoleread and consolewrite.
  devsw[CONSOLE].اقرأ = consoleread;
  devsw[CONSOLE].اكتب = consolewrite;
}
