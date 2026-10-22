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

#अन्तर्भाव <stdarg.h>

#अन्तर्भाव "types.h"
#अन्तर्भाव "param.h"
#अन्तर्भाव "spinlock.h"
#अन्तर्भाव "sleeplock.h"
#अन्तर्भाव "fs.h"
#अन्तर्भाव "file.h"
#अन्तर्भाव "memlayout.h"
#अन्तर्भाव "riscv.h"
#अन्तर्भाव "defs.h"
#अन्तर्भाव "proc.h"

#define BACKSPACE 0x100       // erase the last output character
#define C(x)      ((x) - '@') // Control-x

//
// send one character to the uart, but don't use
// interrupts or sleep(). safe to be called from
// interrupts, e.g. by printk and to echo input
// characters.
//
शून्य
consputc(पूर्णाङ्क c)
{
  यदि (c == BACKSPACE) {
    // if the user typed backspace, overwrite with a space.
    uartputc_sync('\b');
    uartputc_sync(' ');
    uartputc_sync('\b');
  } अन्यथा {
    uartputc_sync(c);
  }
}

संरचना {
  संरचना spinlock lock;

  // input circular buffer
#define आदान_BUF_SIZE 128
  वर्ण buf[आदान_BUF_SIZE];
  uint r; // Read index
  uint w; // Write index
  uint e; // Edit index
} cons;

//
// user write() system calls to the console go here.
// uses sleep() and UART interrupts.
//
पूर्णाङ्क
consolewrite(पूर्णाङ्क user_src, uint64 src, पूर्णाङ्क n)
{
  वर्ण buf[32]; // move batches from user space to uart.
  पूर्णाङ्क i = 0;

  यावत् (i < n) {
    पूर्णाङ्क nn = परिमाण(buf);
    यदि (nn > n - i)
      nn = n - i;
    यदि (either_copyin(buf, user_src, src + i, nn) == -1)
      भङ्ग;
    uartwrite(buf, nn);
    i += nn;
  }

  प्रत्यागम i;
}

//
// user read()s from the console go here.
// copy (up to) a whole input line to dst.
// user_dst indicates whether dst is a user
// or kernel address.
//
पूर्णाङ्क
consoleread(पूर्णाङ्क user_dst, uint64 dst, पूर्णाङ्क n)
{
  uint target;
  पूर्णाङ्क c;
  वर्ण cbuf;

  target = n;
  acquire(&cons.lock);
  यावत् (n > 0) {
    // wait until interrupt handler has put some
    // input into cons.buffer.
    यावत् (cons.r == cons.w) {
      यदि (killed(myproc())) {
        release(&cons.lock);
        प्रत्यागम -1;
      }
      sleep_prepare(&cons.r);
      release(&cons.lock);
      sleep();
      acquire(&cons.lock);
    }

    c = cons.buf[cons.r++ % आदान_BUF_SIZE];

    यदि (c == C('D')) { // end-of-file
      यदि (n < target) {
        // Save ^D for next time, to make sure
        // caller gets a 0-byte result.
        cons.r--;
      }
      भङ्ग;
    }

    // copy the input byte to the user-space buffer.
    cbuf = c;
    यदि (either_copyout(user_dst, dst, &cbuf, 1) == -1)
      भङ्ग;

    dst++;
    --n;

    यदि (c == '\n') {
      // a whole line has arrived, return to
      // the user-level read().
      भङ्ग;
    }
  }
  release(&cons.lock);

  प्रत्यागम target - n;
}

//
// the console input interrupt handler.
// uartintr() calls this for each input character.
// do erase/kill processing, append to cons.buf,
// wake up consoleread() if a whole line has arrived.
//
शून्य
consoleintr(पूर्णाङ्क c)
{
  acquire(&cons.lock);

  विकल्प (c) {
  पक्ष C('P'): // Print process list.
    procdump();
    भङ्ग;
  पक्ष C('U'): // Kill line.
    यावत् (cons.e != cons.w &&
           cons.buf[(cons.e - 1) % आदान_BUF_SIZE] != '\n') {
      cons.e--;
      consputc(BACKSPACE);
    }
    भङ्ग;
  पक्ष C('H'): // Backspace
  पक्ष '\x7f': // Delete key
    यदि (cons.e != cons.w) {
      cons.e--;
      consputc(BACKSPACE);
    }
    भङ्ग;
  स्वभाव:
    यदि (c != 0 && cons.e - cons.r < आदान_BUF_SIZE) {
      c = (c == '\r') ? '\n' : c;

      // echo back to the user.
      consputc(c);

      // store for consumption by consoleread().
      cons.buf[cons.e++ % आदान_BUF_SIZE] = c;

      यदि (c == '\n' || c == C('D') || cons.e - cons.r == आदान_BUF_SIZE) {
        // wake up consoleread() if a whole line (or end-of-file)
        // has arrived.
        cons.w = cons.e;
        wakeup(&cons.r);
      }
    }
    भङ्ग;
  }

  release(&cons.lock);
}

शून्य
consoleinit(शून्य)
{
  initlock(&cons.lock, "cons");

  uartinit();

  // connect read and write system calls
  // to consoleread and consolewrite.
  devsw[CONSOLE].पठ = consoleread;
  devsw[CONSOLE].लिख = consolewrite;
}
