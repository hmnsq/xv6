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

#ಅನ್ತರ್ಭಾವ <stdarg.h>

#ಅನ್ತರ್ಭಾವ "types.h"
#ಅನ್ತರ್ಭಾವ "param.h"
#ಅನ್ತರ್ಭಾವ "spinlock.h"
#ಅನ್ತರ್ಭಾವ "sleeplock.h"
#ಅನ್ತರ್ಭಾವ "fs.h"
#ಅನ್ತರ್ಭಾವ "file.h"
#ಅನ್ತರ್ಭಾವ "memlayout.h"
#ಅನ್ತರ್ಭಾವ "riscv.h"
#ಅನ್ತರ್ಭಾವ "defs.h"
#ಅನ್ತರ್ಭಾವ "proc.h"

#define BACKSPACE 0x100       // erase the last output character
#define C(x)      ((x) - '@') // Control-x

//
// send one character to the uart, but don't use
// interrupts or sleep(). safe to be called from
// interrupts, e.g. by printk and to echo input
// characters.
//
ಶೂನ್ಯ
consputc(ಪೂರ್ಣಾಙ್ಕ c)
{
  ಯದಿ (c == BACKSPACE) {
    // if the user typed backspace, overwrite with a space.
    uartputc_sync('\b');
    uartputc_sync(' ');
    uartputc_sync('\b');
  } ಅನ್ಯಥಾ {
    uartputc_sync(c);
  }
}

ಸಂರಚನಾ {
  ಸಂರಚನಾ spinlock lock;

  // input circular buffer
#define ಒಳಹರಿವು_BUF_SIZE 128
  ವರ್ಣ buf[ಒಳಹರಿವು_BUF_SIZE];
  uint r; // Read index
  uint w; // Write index
  uint e; // Edit index
} cons;

//
// user write() system calls to the console go here.
// uses sleep() and UART interrupts.
//
ಪೂರ್ಣಾಙ್ಕ
consolewrite(ಪೂರ್ಣಾಙ್ಕ user_src, uint64 src, ಪೂರ್ಣಾಙ್ಕ n)
{
  ವರ್ಣ buf[32]; // move batches from user space to uart.
  ಪೂರ್ಣಾಙ್ಕ i = 0;

  ಯಾವತ್ (i < n) {
    ಪೂರ್ಣಾಙ್ಕ nn = ಪರಿಮಾಣ(buf);
    ಯದಿ (nn > n - i)
      nn = n - i;
    ಯದಿ (either_copyin(buf, user_src, src + i, nn) == -1)
      ಭಙ್ಗ;
    uartwrite(buf, nn);
    i += nn;
  }

  ಪ್ರತ್ಯಾಗಮ i;
}

//
// user read()s from the console go here.
// copy (up to) a whole input line to dst.
// user_dst indicates whether dst is a user
// or kernel address.
//
ಪೂರ್ಣಾಙ್ಕ
consoleread(ಪೂರ್ಣಾಙ್ಕ user_dst, uint64 dst, ಪೂರ್ಣಾಙ್ಕ n)
{
  uint target;
  ಪೂರ್ಣಾಙ್ಕ c;
  ವರ್ಣ cbuf;

  target = n;
  acquire(&cons.lock);
  ಯಾವತ್ (n > 0) {
    // wait until interrupt handler has put some
    // input into cons.buffer.
    ಯಾವತ್ (cons.r == cons.w) {
      ಯದಿ (killed(myproc())) {
        release(&cons.lock);
        ಪ್ರತ್ಯಾಗಮ -1;
      }
      sleep_prepare(&cons.r);
      release(&cons.lock);
      sleep();
      acquire(&cons.lock);
    }

    c = cons.buf[cons.r++ % ಒಳಹರಿವು_BUF_SIZE];

    ಯದಿ (c == C('D')) { // end-of-file
      ಯದಿ (n < target) {
        // Save ^D for next time, to make sure
        // caller gets a 0-byte result.
        cons.r--;
      }
      ಭಙ್ಗ;
    }

    // copy the input byte to the user-space buffer.
    cbuf = c;
    ಯದಿ (either_copyout(user_dst, dst, &cbuf, 1) == -1)
      ಭಙ್ಗ;

    dst++;
    --n;

    ಯದಿ (c == '\n') {
      // a whole line has arrived, return to
      // the user-level read().
      ಭಙ್ಗ;
    }
  }
  release(&cons.lock);

  ಪ್ರತ್ಯಾಗಮ target - n;
}

//
// the console input interrupt handler.
// uartintr() calls this for each input character.
// do erase/kill processing, append to cons.buf,
// wake up consoleread() if a whole line has arrived.
//
ಶೂನ್ಯ
consoleintr(ಪೂರ್ಣಾಙ್ಕ c)
{
  acquire(&cons.lock);

  ವಿಕಲ್ಪ (c) {
  ಪಕ್ಷ C('P'): // Print process list.
    procdump();
    ಭಙ್ಗ;
  ಪಕ್ಷ C('U'): // Kill line.
    ಯಾವತ್ (cons.e != cons.w &&
           cons.buf[(cons.e - 1) % ಒಳಹರಿವು_BUF_SIZE] != '\n') {
      cons.e--;
      consputc(BACKSPACE);
    }
    ಭಙ್ಗ;
  ಪಕ್ಷ C('H'): // Backspace
  ಪಕ್ಷ '\x7f': // Delete key
    ಯದಿ (cons.e != cons.w) {
      cons.e--;
      consputc(BACKSPACE);
    }
    ಭಙ್ಗ;
  ಸ್ವಭಾವ:
    ಯದಿ (c != 0 && cons.e - cons.r < ಒಳಹರಿವು_BUF_SIZE) {
      c = (c == '\r') ? '\n' : c;

      // echo back to the user.
      consputc(c);

      // store for consumption by consoleread().
      cons.buf[cons.e++ % ಒಳಹರಿವು_BUF_SIZE] = c;

      ಯದಿ (c == '\n' || c == C('D') || cons.e - cons.r == ಒಳಹರಿವು_BUF_SIZE) {
        // wake up consoleread() if a whole line (or end-of-file)
        // has arrived.
        cons.w = cons.e;
        wakeup(&cons.r);
      }
    }
    ಭಙ್ಗ;
  }

  release(&cons.lock);
}

ಶೂನ್ಯ
consoleinit(ಶೂನ್ಯ)
{
  initlock(&cons.lock, "cons");

  uartinit();

  // connect read and write system calls
  // to consoleread and consolewrite.
  devsw[CONSOLE].ಪಠ = consoleread;
  devsw[CONSOLE].ಲಿಖ = consolewrite;
}
