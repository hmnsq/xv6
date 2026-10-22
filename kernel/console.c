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
consputc(पूर्णांक c)
{
  जर (c == BACKSPACE) {
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
  अक्षर buf[आदान_BUF_SIZE];
  uint r; // Read index
  uint w; // Write index
  uint e; // Edit index
} cons;

//
// user write() system calls to the console go here.
// uses sleep() and UART interrupts.
//
पूर्णांक
consolewrite(पूर्णांक user_src, uint64 src, पूर्णांक n)
{
  अक्षर buf[32]; // move batches from user space to uart.
  पूर्णांक i = 0;

  पर्यंत (i < n) {
    पूर्णांक nn = आकार(buf);
    जर (nn > n - i)
      nn = n - i;
    जर (either_copyin(buf, user_src, src + i, nn) == -1)
      थांबा;
    uartwrite(buf, nn);
    i += nn;
  }

  परत i;
}

//
// user read()s from the console go here.
// copy (up to) a whole input line to dst.
// user_dst indicates whether dst is a user
// or kernel address.
//
पूर्णांक
consoleread(पूर्णांक user_dst, uint64 dst, पूर्णांक n)
{
  uint target;
  पूर्णांक c;
  अक्षर cbuf;

  target = n;
  acquire(&cons.lock);
  पर्यंत (n > 0) {
    // wait until interrupt handler has put some
    // input into cons.buffer.
    पर्यंत (cons.r == cons.w) {
      जर (killed(myproc())) {
        release(&cons.lock);
        परत -1;
      }
      sleep_prepare(&cons.r);
      release(&cons.lock);
      sleep();
      acquire(&cons.lock);
    }

    c = cons.buf[cons.r++ % आदान_BUF_SIZE];

    जर (c == C('D')) { // end-of-file
      जर (n < target) {
        // Save ^D for next time, to make sure
        // caller gets a 0-byte result.
        cons.r--;
      }
      थांबा;
    }

    // copy the input byte to the user-space buffer.
    cbuf = c;
    जर (either_copyout(user_dst, dst, &cbuf, 1) == -1)
      थांबा;

    dst++;
    --n;

    जर (c == '\n') {
      // a whole line has arrived, return to
      // the user-level read().
      थांबा;
    }
  }
  release(&cons.lock);

  परत target - n;
}

//
// the console input interrupt handler.
// uartintr() calls this for each input character.
// do erase/kill processing, append to cons.buf,
// wake up consoleread() if a whole line has arrived.
//
शून्य
consoleintr(पूर्णांक c)
{
  acquire(&cons.lock);

  निवड (c) {
  स्थिती C('P'): // Print process list.
    procdump();
    थांबा;
  स्थिती C('U'): // Kill line.
    पर्यंत (cons.e != cons.w &&
           cons.buf[(cons.e - 1) % आदान_BUF_SIZE] != '\n') {
      cons.e--;
      consputc(BACKSPACE);
    }
    थांबा;
  स्थिती C('H'): // Backspace
  स्थिती '\x7f': // Delete key
    जर (cons.e != cons.w) {
      cons.e--;
      consputc(BACKSPACE);
    }
    थांबा;
  पूर्वनिर्धारित:
    जर (c != 0 && cons.e - cons.r < आदान_BUF_SIZE) {
      c = (c == '\r') ? '\n' : c;

      // echo back to the user.
      consputc(c);

      // store for consumption by consoleread().
      cons.buf[cons.e++ % आदान_BUF_SIZE] = c;

      जर (c == '\n' || c == C('D') || cons.e - cons.r == आदान_BUF_SIZE) {
        // wake up consoleread() if a whole line (or end-of-file)
        // has arrived.
        cons.w = cons.e;
        wakeup(&cons.r);
      }
    }
    थांबा;
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
