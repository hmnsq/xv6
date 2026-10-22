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

#અન્તર્ભાવ <stdarg.h>

#અન્તર્ભાવ "types.h"
#અન્તર્ભાવ "param.h"
#અન્તર્ભાવ "spinlock.h"
#અન્તર્ભાવ "sleeplock.h"
#અન્તર્ભાવ "fs.h"
#અન્તર્ભાવ "file.h"
#અન્તર્ભાવ "memlayout.h"
#અન્તર્ભાવ "riscv.h"
#અન્તર્ભાવ "defs.h"
#અન્તર્ભાવ "proc.h"

#define BACKSPACE 0x100       // erase the last output character
#define C(x)      ((x) - '@') // Control-x

//
// send one character to the uart, but don't use
// interrupts or sleep(). safe to be called from
// interrupts, e.g. by printk and to echo input
// characters.
//
શૂન્ય
consputc(પૂર્ણાંક c)
{
  જો (c == BACKSPACE) {
    // if the user typed backspace, overwrite with a space.
    uartputc_sync('\b');
    uartputc_sync(' ');
    uartputc_sync('\b');
  } નહીંતર {
    uartputc_sync(c);
  }
}

સંરચના {
  સંરચના spinlock lock;

  // input circular buffer
#define આગત_BUF_SIZE 128
  અક્ષર buf[આગત_BUF_SIZE];
  uint r; // Read index
  uint w; // Write index
  uint e; // Edit index
} cons;

//
// user write() system calls to the console go here.
// uses sleep() and UART interrupts.
//
પૂર્ણાંક
consolewrite(પૂર્ણાંક user_src, uint64 src, પૂર્ણાંક n)
{
  અક્ષર buf[32]; // move batches from user space to uart.
  પૂર્ણાંક i = 0;

  જ્યારે (i < n) {
    પૂર્ણાંક nn = કદ(buf);
    જો (nn > n - i)
      nn = n - i;
    જો (either_copyin(buf, user_src, src + i, nn) == -1)
      તોડો;
    uartwrite(buf, nn);
    i += nn;
  }

  પાછા i;
}

//
// user read()s from the console go here.
// copy (up to) a whole input line to dst.
// user_dst indicates whether dst is a user
// or kernel address.
//
પૂર્ણાંક
consoleread(પૂર્ણાંક user_dst, uint64 dst, પૂર્ણાંક n)
{
  uint target;
  પૂર્ણાંક c;
  અક્ષર cbuf;

  target = n;
  acquire(&cons.lock);
  જ્યારે (n > 0) {
    // wait until interrupt handler has put some
    // input into cons.buffer.
    જ્યારે (cons.r == cons.w) {
      જો (killed(myproc())) {
        release(&cons.lock);
        પાછા -1;
      }
      sleep_prepare(&cons.r);
      release(&cons.lock);
      sleep();
      acquire(&cons.lock);
    }

    c = cons.buf[cons.r++ % આગત_BUF_SIZE];

    જો (c == C('D')) { // end-of-file
      જો (n < target) {
        // Save ^D for next time, to make sure
        // caller gets a 0-byte result.
        cons.r--;
      }
      તોડો;
    }

    // copy the input byte to the user-space buffer.
    cbuf = c;
    જો (either_copyout(user_dst, dst, &cbuf, 1) == -1)
      તોડો;

    dst++;
    --n;

    જો (c == '\n') {
      // a whole line has arrived, return to
      // the user-level read().
      તોડો;
    }
  }
  release(&cons.lock);

  પાછા target - n;
}

//
// the console input interrupt handler.
// uartintr() calls this for each input character.
// do erase/kill processing, append to cons.buf,
// wake up consoleread() if a whole line has arrived.
//
શૂન્ય
consoleintr(પૂર્ણાંક c)
{
  acquire(&cons.lock);

  પસંદગી (c) {
  સ્થિતિ C('P'): // Print process list.
    procdump();
    તોડો;
  સ્થિતિ C('U'): // Kill line.
    જ્યારે (cons.e != cons.w &&
           cons.buf[(cons.e - 1) % આગત_BUF_SIZE] != '\n') {
      cons.e--;
      consputc(BACKSPACE);
    }
    તોડો;
  સ્થિતિ C('H'): // Backspace
  સ્થિતિ '\x7f': // Delete key
    જો (cons.e != cons.w) {
      cons.e--;
      consputc(BACKSPACE);
    }
    તોડો;
  મૂળભૂત:
    જો (c != 0 && cons.e - cons.r < આગત_BUF_SIZE) {
      c = (c == '\r') ? '\n' : c;

      // echo back to the user.
      consputc(c);

      // store for consumption by consoleread().
      cons.buf[cons.e++ % આગત_BUF_SIZE] = c;

      જો (c == '\n' || c == C('D') || cons.e - cons.r == આગત_BUF_SIZE) {
        // wake up consoleread() if a whole line (or end-of-file)
        // has arrived.
        cons.w = cons.e;
        wakeup(&cons.r);
      }
    }
    તોડો;
  }

  release(&cons.lock);
}

શૂન્ય
consoleinit(શૂન્ય)
{
  initlock(&cons.lock, "cons");

  uartinit();

  // connect read and write system calls
  // to consoleread and consolewrite.
  devsw[CONSOLE].પઠ = consoleread;
  devsw[CONSOLE].લિખ = consolewrite;
}
