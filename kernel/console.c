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

#समावेश <stdarg.h>

#समावेश "types.h"
#समावेश "param.h"
#समावेश "spinlock.h"
#समावेश "sleeplock.h"
#समावेश "fs.h"
#समावेश "file.h"
#समावेश "memlayout.h"
#समावेश "riscv.h"
#समावेश "defs.h"
#समावेश "proc.h"

#घोषणा BACKSPACE 0x100       // erase the last output character
#घोषणा C(x)      ((x) - '@') // Control-x

//
// send one character to the uart, but don't use
// interrupts or sleep(). safe to be called from
// interrupts, e.g. by printk and to echo input
// characters.
//
व्योम
consputc(पूर्णांक c)
{
  अगर (c == BACKSPACE) {
    // if the user typed backspace, overwrite with a space.
    uartputc_sync('\b');
    uartputc_sync(' ');
    uartputc_sync('\b');
  } अन्यथा {
    uartputc_sync(c);
  }
}

काष्ठा {
  काष्ठा spinlock lock;

  // input circular buffer
#घोषणा निवेश_BUF_आकार 128
  अक्षर buf[निवेश_BUF_आकार];
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

  जबतक (i < n) {
    पूर्णांक nn = माप(buf);
    अगर (nn > n - i)
      nn = n - i;
    अगर (either_copyin(buf, user_src, src + i, nn) == -1)
      अवरोध;
    uartwrite(buf, nn);
    i += nn;
  }

  वापस i;
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
  जबतक (n > 0) {
    // wait until interrupt handler has put some
    // input into cons.buffer.
    जबतक (cons.r == cons.w) {
      अगर (killed(myproc())) {
        release(&cons.lock);
        वापस -1;
      }
      sleep_prepare(&cons.r);
      release(&cons.lock);
      sleep();
      acquire(&cons.lock);
    }

    c = cons.buf[cons.r++ % निवेश_BUF_आकार];

    अगर (c == C('D')) { // end-of-file
      अगर (n < target) {
        // Save ^D for next time, to make sure
        // caller gets a 0-byte result.
        cons.r--;
      }
      अवरोध;
    }

    // copy the input byte to the user-space buffer.
    cbuf = c;
    अगर (either_copyout(user_dst, dst, &cbuf, 1) == -1)
      अवरोध;

    dst++;
    --n;

    अगर (c == '\n') {
      // a whole line has arrived, return to
      // the user-level read().
      अवरोध;
    }
  }
  release(&cons.lock);

  वापस target - n;
}

//
// the console input interrupt handler.
// uartintr() calls this for each input character.
// do erase/kill processing, append to cons.buf,
// wake up consoleread() if a whole line has arrived.
//
व्योम
consoleintr(पूर्णांक c)
{
  acquire(&cons.lock);

  चयन (c) {
  हाल C('P'): // Print process list.
    procdump();
    अवरोध;
  हाल C('U'): // Kill line.
    जबतक (cons.e != cons.w &&
           cons.buf[(cons.e - 1) % निवेश_BUF_आकार] != '\n') {
      cons.e--;
      consputc(BACKSPACE);
    }
    अवरोध;
  हाल C('H'): // Backspace
  हाल '\x7f': // Delete key
    अगर (cons.e != cons.w) {
      cons.e--;
      consputc(BACKSPACE);
    }
    अवरोध;
  शेष:
    अगर (c != 0 && cons.e - cons.r < निवेश_BUF_आकार) {
      c = (c == '\r') ? '\n' : c;

      // echo back to the user.
      consputc(c);

      // store for consumption by consoleread().
      cons.buf[cons.e++ % निवेश_BUF_आकार] = c;

      अगर (c == '\n' || c == C('D') || cons.e - cons.r == निवेश_BUF_आकार) {
        // wake up consoleread() if a whole line (or end-of-file)
        // has arrived.
        cons.w = cons.e;
        wakeup(&cons.r);
      }
    }
    अवरोध;
  }

  release(&cons.lock);
}

व्योम
consoleinit(व्योम)
{
  initlock(&cons.lock, "cons");

  uartinit();

  // connect read and write system calls
  // to consoleread and consolewrite.
  devsw[CONSOLE].पढ़ो = consoleread;
  devsw[CONSOLE].ग_लिखो = consolewrite;
}
