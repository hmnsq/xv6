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

#ವ್ಯಾಖ್ಯಾನಿಸು BACKSPACE 0x100       // erase the last output character
#ವ್ಯಾಖ್ಯಾನಿಸು C(x)      ((x) - '@') // Control-x

//
// send one character to the uart, but don't use
// interrupts or sleep(). safe to be called from
// interrupts, e.g. by printk and to echo input
// characters.
//
ಶೂನ್ಯ
consputc(ಪೂರ್ಣಾಂಕ c)
{
  ಆದರೆ (c == BACKSPACE) {
    // if the user typed backspace, overwrite with a space.
    uartputc_sync('\b');
    uartputc_sync(' ');
    uartputc_sync('\b');
  } ಇಲ್ಲದಿದ್ದರೆ {
    uartputc_sync(c);
  }
}

ರಚನೆ {
  ರಚನೆ spinlock ಬೀಗ;

  // input circular buffer
#ವ್ಯಾಖ್ಯಾನಿಸು ಒಳಹರಿವು_BUF_ಗಾತ್ರ 128
  ಅಕ್ಷರ buf[ಒಳಹರಿವು_BUF_ಗಾತ್ರ];
  uint r; // Read index
  uint w; // Write index
  uint e; // Edit index
} cons;

//
// user write() system calls to the console go here.
// uses sleep() and UART interrupts.
//
ಪೂರ್ಣಾಂಕ
consolewrite(ಪೂರ್ಣಾಂಕ user_src, uint64 src, ಪೂರ್ಣಾಂಕ n)
{
  ಅಕ್ಷರ buf[32]; // move batches from user space to uart.
  ಪೂರ್ಣಾಂಕ i = 0;

  ಆಗ (i < n) {
    ಪೂರ್ಣಾಂಕ nn = ಗಾತ್ರ(buf);
    ಆದರೆ (nn > n - i)
      nn = n - i;
    ಆದರೆ (either_copyin(buf, user_src, src + i, nn) == -1)
      ನಿಲ್ಲಿಸು;
    uartwrite(buf, nn);
    i += nn;
  }

  ಹಿಂತಿರುಗಿ i;
}

//
// user read()s from the console go here.
// copy (up to) a whole input line to dst.
// user_dst indicates whether dst is a user
// or kernel address.
//
ಪೂರ್ಣಾಂಕ
consoleread(ಪೂರ್ಣಾಂಕ user_dst, uint64 dst, ಪೂರ್ಣಾಂಕ n)
{
  uint ಗುರಿ;
  ಪೂರ್ಣಾಂಕ c;
  ಅಕ್ಷರ cbuf;

  ಗುರಿ = n;
  acquire(&cons.ಬೀಗ);
  ಆಗ (n > 0) {
    // wait until interrupt handler has put some
    // input into cons.buffer.
    ಆಗ (cons.r == cons.w) {
      ಆದರೆ (killed(myproc())) {
        release(&cons.ಬೀಗ);
        ಹಿಂತಿರುಗಿ -1;
      }
      sleep_prepare(&cons.r);
      release(&cons.ಬೀಗ);
      sleep();
      acquire(&cons.ಬೀಗ);
    }

    c = cons.buf[cons.r++ % ಒಳಹರಿವು_BUF_ಗಾತ್ರ];

    ಆದರೆ (c == C('D')) { // end-of-file
      ಆದರೆ (n < ಗುರಿ) {
        // Save ^D for next time, to make sure
        // caller gets a 0-byte result.
        cons.r--;
      }
      ನಿಲ್ಲಿಸು;
    }

    // copy the input byte to the user-space buffer.
    cbuf = c;
    ಆದರೆ (either_copyout(user_dst, dst, &cbuf, 1) == -1)
      ನಿಲ್ಲಿಸು;

    dst++;
    --n;

    ಆದರೆ (c == '\n') {
      // a whole line has arrived, return to
      // the user-level read().
      ನಿಲ್ಲಿಸು;
    }
  }
  release(&cons.ಬೀಗ);

  ಹಿಂತಿರುಗಿ ಗುರಿ - n;
}

//
// the console input interrupt handler.
// uartintr() calls this for each input character.
// do erase/kill processing, append to cons.buf,
// wake up consoleread() if a whole line has arrived.
//
ಶೂನ್ಯ
consoleintr(ಪೂರ್ಣಾಂಕ c)
{
  acquire(&cons.ಬೀಗ);

  ಆಯ್ಕೆ (c) {
  ಸ್ಥಿತಿ C('P'): // Print process list.
    procdump();
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ C('U'): // Kill line.
    ಆಗ (cons.e != cons.w &&
           cons.buf[(cons.e - 1) % ಒಳಹರಿವು_BUF_ಗಾತ್ರ] != '\n') {
      cons.e--;
      consputc(BACKSPACE);
    }
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ C('H'): // Backspace
  ಸ್ಥಿತಿ '\x7f': // Delete key
    ಆದರೆ (cons.e != cons.w) {
      cons.e--;
      consputc(BACKSPACE);
    }
    ನಿಲ್ಲಿಸು;
  ಪೂರ್ವನಿಯೋಜಿತ:
    ಆದರೆ (c != 0 && cons.e - cons.r < ಒಳಹರಿವು_BUF_ಗಾತ್ರ) {
      c = (c == '\r') ? '\n' : c;

      // echo back to the user.
      consputc(c);

      // store for consumption by consoleread().
      cons.buf[cons.e++ % ಒಳಹರಿವು_BUF_ಗಾತ್ರ] = c;

      ಆದರೆ (c == '\n' || c == C('D') || cons.e - cons.r == ಒಳಹರಿವು_BUF_ಗಾತ್ರ) {
        // wake up consoleread() if a whole line (or end-of-file)
        // has arrived.
        cons.w = cons.e;
        wakeup(&cons.r);
      }
    }
    ನಿಲ್ಲಿಸು;
  }

  release(&cons.ಬೀಗ);
}

ಶೂನ್ಯ
consoleinit(ಶೂನ್ಯ)
{
  initlock(&cons.ಬೀಗ, "cons");

  uartinit();

  // connect read and write system calls
  // to consoleread and consolewrite.
  devsw[CONSOLE].ಪಠ = consoleread;
  devsw[CONSOLE].ಲಿಖ = consolewrite;
}
