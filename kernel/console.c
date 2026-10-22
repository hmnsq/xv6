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
consputc(ཧྲིལ c)
{
  གལ་ཏེ (c == BACKSPACE) {
    // if the user typed backspace, overwrite with a space.
    uartputc_sync('\b');
    uartputc_sync(' ');
    uartputc_sync('\b');
  } དེ་མིན {
    uartputc_sync(c);
  }
}

sa_nracanaa {
  sa_nracanaa spinlock lock;

  // input circular buffer
#define INPUT_BUF_SIZE 128
  var_na buf[INPUT_BUF_SIZE];
  uint r; // Read index
  uint w; // Write index
  uint e; // Edit index
} cons;

//
// user write() system calls to the console go here.
// uses sleep() and UART interrupts.
//
ཧྲིལ
consolewrite(ཧྲིལ user_src, uint64 src, ཧྲིལ n)
{
  var_na buf[32]; // move batches from user space to uart.
  ཧྲིལ i = 0;

  སྐབས (i < n) {
    ཧྲིལ nn = parimaa_na(buf);
    གལ་ཏེ (nn > n - i)
      nn = n - i;
    གལ་ཏེ (either_copyin(buf, user_src, src + i, nn) == -1)
      bha_nga;
    uartwrite(buf, nn);
    i += nn;
  }

  ཕྱིར i;
}

//
// user read()s from the console go here.
// copy (up to) a whole input line to dst.
// user_dst indicates whether dst is a user
// or kernel address.
//
ཧྲིལ
consoleread(ཧྲིལ user_dst, uint64 dst, ཧྲིལ n)
{
  uint target;
  ཧྲིལ c;
  var_na cbuf;

  target = n;
  acquire(&cons.lock);
  སྐབས (n > 0) {
    // wait until interrupt handler has put some
    // input into cons.buffer.
    སྐབས (cons.r == cons.w) {
      གལ་ཏེ (killed(myproc())) {
        release(&cons.lock);
        ཕྱིར -1;
      }
      sleep_prepare(&cons.r);
      release(&cons.lock);
      sleep();
      acquire(&cons.lock);
    }

    c = cons.buf[cons.r++ % INPUT_BUF_SIZE];

    གལ་ཏེ (c == C('D')) { // end-of-file
      གལ་ཏེ (n < target) {
        // Save ^D for next time, to make sure
        // caller gets a 0-byte result.
        cons.r--;
      }
      bha_nga;
    }

    // copy the input byte to the user-space buffer.
    cbuf = c;
    གལ་ཏེ (either_copyout(user_dst, dst, &cbuf, 1) == -1)
      bha_nga;

    dst++;
    --n;

    གལ་ཏེ (c == '\n') {
      // a whole line has arrived, return to
      // the user-level read().
      bha_nga;
    }
  }
  release(&cons.lock);

  ཕྱིར target - n;
}

//
// the console input interrupt handler.
// uartintr() calls this for each input character.
// do erase/kill processing, append to cons.buf,
// wake up consoleread() if a whole line has arrived.
//
void
consoleintr(ཧྲིལ c)
{
  acquire(&cons.lock);

  switch (c) {
  pak_sa C('P'): // Print process list.
    procdump();
    bha_nga;
  pak_sa C('U'): // Kill line.
    སྐབས (cons.e != cons.w &&
           cons.buf[(cons.e - 1) % INPUT_BUF_SIZE] != '\n') {
      cons.e--;
      consputc(BACKSPACE);
    }
    bha_nga;
  pak_sa C('H'): // Backspace
  pak_sa '\x7f': // Delete key
    གལ་ཏེ (cons.e != cons.w) {
      cons.e--;
      consputc(BACKSPACE);
    }
    bha_nga;
  default:
    གལ་ཏེ (c != 0 && cons.e - cons.r < INPUT_BUF_SIZE) {
      c = (c == '\r') ? '\n' : c;

      // echo back to the user.
      consputc(c);

      // store for consumption by consoleread().
      cons.buf[cons.e++ % INPUT_BUF_SIZE] = c;

      གལ་ཏེ (c == '\n' || c == C('D') || cons.e - cons.r == INPUT_BUF_SIZE) {
        // wake up consoleread() if a whole line (or end-of-file)
        // has arrived.
        cons.w = cons.e;
        wakeup(&cons.r);
      }
    }
    bha_nga;
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
