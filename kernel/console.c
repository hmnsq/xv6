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

#تعریف_کرو BACKSPACE 0x100       // erase the last output character
#تعریف_کرو C(x)      ((x) - '@') // Control-x

//
// send one character to the uart, but don't use
// interrupts or sleep(). safe to be called from
// interrupts, e.g. by printk and to echo input
// characters.
//
خالی
consputc(صحیح c)
{
  اگر (c == BACKSPACE) {
    // if the user typed backspace, overwrite with a space.
    uartputc_sync('\b');
    uartputc_sync(' ');
    uartputc_sync('\b');
  } ورنہ {
    uartputc_sync(c);
  }
}

ساخت {
  ساخت spinlock تالا;

  // input circular buffer
#تعریف_کرو داخلہ_BUF_سائز 128
  حرف buf[داخلہ_BUF_سائز];
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
  حرف buf[32]; // move batches from user space to uart.
  صحیح i = 0;

  جب (i < n) {
    صحیح nn = حجم(buf);
    اگر (nn > n - i)
      nn = n - i;
    اگر (either_copyin(buf, user_src, src + i, nn) == -1)
      توڑو;
    uartwrite(buf, nn);
    i += nn;
  }

  واپس i;
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
  uint ہدف;
  صحیح c;
  حرف cbuf;

  ہدف = n;
  acquire(&cons.تالا);
  جب (n > 0) {
    // wait until interrupt handler has put some
    // input into cons.buffer.
    جب (cons.r == cons.w) {
      اگر (killed(myproc())) {
        release(&cons.تالا);
        واپس -1;
      }
      sleep_prepare(&cons.r);
      release(&cons.تالا);
      sleep();
      acquire(&cons.تالا);
    }

    c = cons.buf[cons.r++ % داخلہ_BUF_سائز];

    اگر (c == C('D')) { // end-of-file
      اگر (n < ہدف) {
        // Save ^D for next time, to make sure
        // caller gets a 0-byte result.
        cons.r--;
      }
      توڑو;
    }

    // copy the input byte to the user-space buffer.
    cbuf = c;
    اگر (either_copyout(user_dst, dst, &cbuf, 1) == -1)
      توڑو;

    dst++;
    --n;

    اگر (c == '\n') {
      // a whole line has arrived, return to
      // the user-level read().
      توڑو;
    }
  }
  release(&cons.تالا);

  واپس ہدف - n;
}

//
// the console input interrupt handler.
// uartintr() calls this for each input character.
// do erase/kill processing, append to cons.buf,
// wake up consoleread() if a whole line has arrived.
//
خالی
consoleintr(صحیح c)
{
  acquire(&cons.تالا);

  انتخاب (c) {
  حالت C('P'): // Print process list.
    procdump();
    توڑو;
  حالت C('U'): // Kill line.
    جب (cons.e != cons.w &&
           cons.buf[(cons.e - 1) % داخلہ_BUF_سائز] != '\n') {
      cons.e--;
      consputc(BACKSPACE);
    }
    توڑو;
  حالت C('H'): // Backspace
  حالت '\x7f': // Delete key
    اگر (cons.e != cons.w) {
      cons.e--;
      consputc(BACKSPACE);
    }
    توڑو;
  طےشدہ:
    اگر (c != 0 && cons.e - cons.r < داخلہ_BUF_سائز) {
      c = (c == '\r') ? '\n' : c;

      // echo back to the user.
      consputc(c);

      // store for consumption by consoleread().
      cons.buf[cons.e++ % داخلہ_BUF_سائز] = c;

      اگر (c == '\n' || c == C('D') || cons.e - cons.r == داخلہ_BUF_سائز) {
        // wake up consoleread() if a whole line (or end-of-file)
        // has arrived.
        cons.w = cons.e;
        wakeup(&cons.r);
      }
    }
    توڑو;
  }

  release(&cons.تالا);
}

خالی
consoleinit(خالی)
{
  initlock(&cons.تالا, "cons");

  uartinit();

  // connect read and write system calls
  // to consoleread and consolewrite.
  devsw[CONSOLE].اقرأ = consoleread;
  devsw[CONSOLE].اكتب = consolewrite;
}
