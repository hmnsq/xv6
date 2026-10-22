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

#include <stdarg.h>

#include "types.h"
#include "param.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "fs.h"
#include "file.h"
#include "memlayout.h"
#include "riscv.h"
#include "defs.h"
#include "proc.h"

#define BACKSPACE 0x100       // erase the last output character
#define C(x)      ((x) - '@') // Control-x

//
// send one character to the uart, but don't use
// interrupts or sleep(). safe to be called from
// interrupts, e.g. by printk and to echo input
// characters.
//
void
consputc(цяло c)
{
  ако (c == BACKSPACE) {
    // if the user typed backspace, overwrite with a space.
    uartputc_sync('\b');
    uartputc_sync(' ');
    uartputc_sync('\b');
  } иначе {
    uartputc_sync(c);
  }
}

struct {
  struct spinlock lock;

  // input circular buffer
#define INPUT_BUF_SIZE 128
  char buf[INPUT_BUF_SIZE];
  uint r; // Read index
  uint w; // Write index
  uint e; // Edit index
} cons;

//
// user write() system calls to the console go here.
// uses sleep() and UART interrupts.
//
цяло
consolewrite(цяло user_src, uint64 src, цяло n)
{
  char buf[32]; // move batches from user space to uart.
  цяло i = 0;

  докато (i < n) {
    цяло nn = sizeof(buf);
    ако (nn > n - i)
      nn = n - i;
    ако (either_copyin(buf, user_src, src + i, nn) == -1)
      break;
    uartwrite(buf, nn);
    i += nn;
  }

  върни i;
}

//
// user read()s from the console go here.
// copy (up to) a whole input line to dst.
// user_dst indicates whether dst is a user
// or kernel address.
//
цяло
consoleread(цяло user_dst, uint64 dst, цяло n)
{
  uint target;
  цяло c;
  char cbuf;

  target = n;
  acquire(&cons.lock);
  докато (n > 0) {
    // wait until interrupt handler has put some
    // input into cons.buffer.
    докато (cons.r == cons.w) {
      ако (killed(myproc())) {
        release(&cons.lock);
        върни -1;
      }
      sleep_prepare(&cons.r);
      release(&cons.lock);
      sleep();
      acquire(&cons.lock);
    }

    c = cons.buf[cons.r++ % INPUT_BUF_SIZE];

    ако (c == C('D')) { // end-of-file
      ако (n < target) {
        // Save ^D for next time, to make sure
        // caller gets a 0-byte result.
        cons.r--;
      }
      break;
    }

    // copy the input byte to the user-space buffer.
    cbuf = c;
    ако (either_copyout(user_dst, dst, &cbuf, 1) == -1)
      break;

    dst++;
    --n;

    ако (c == '\n') {
      // a whole line has arrived, return to
      // the user-level read().
      break;
    }
  }
  release(&cons.lock);

  върни target - n;
}

//
// the console input interrupt handler.
// uartintr() calls this for each input character.
// do erase/kill processing, append to cons.buf,
// wake up consoleread() if a whole line has arrived.
//
void
consoleintr(цяло c)
{
  acquire(&cons.lock);

  switch (c) {
  case C('P'): // Print process list.
    procdump();
    break;
  case C('U'): // Kill line.
    докато (cons.e != cons.w &&
           cons.buf[(cons.e - 1) % INPUT_BUF_SIZE] != '\n') {
      cons.e--;
      consputc(BACKSPACE);
    }
    break;
  case C('H'): // Backspace
  case '\x7f': // Delete key
    ако (cons.e != cons.w) {
      cons.e--;
      consputc(BACKSPACE);
    }
    break;
  default:
    ако (c != 0 && cons.e - cons.r < INPUT_BUF_SIZE) {
      c = (c == '\r') ? '\n' : c;

      // echo back to the user.
      consputc(c);

      // store for consumption by consoleread().
      cons.buf[cons.e++ % INPUT_BUF_SIZE] = c;

      ако (c == '\n' || c == C('D') || cons.e - cons.r == INPUT_BUF_SIZE) {
        // wake up consoleread() if a whole line (or end-of-file)
        // has arrived.
        cons.w = cons.e;
        wakeup(&cons.r);
      }
    }
    break;
  }

  release(&cons.lock);
}

void
consoleinit(void)
{
  initlock(&cons.lock, "cons");

  uartinit();

  // connect read and write system calls
  // to consoleread and consolewrite.
  devsw[CONSOLE].read = consoleread;
  devsw[CONSOLE].write = consolewrite;
}
