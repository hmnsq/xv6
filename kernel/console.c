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

#అన్తర్భావ <stdarg.h>

#అన్తర్భావ "types.h"
#అన్తర్భావ "param.h"
#అన్తర్భావ "spinlock.h"
#అన్తర్భావ "sleeplock.h"
#అన్తర్భావ "fs.h"
#అన్తర్భావ "file.h"
#అన్తర్భావ "memlayout.h"
#అన్తర్భావ "riscv.h"
#అన్తర్భావ "defs.h"
#అన్తర్భావ "proc.h"

#నిర్వచించు BACKSPACE 0x100       // erase the last output character
#నిర్వచించు C(x)      ((x) - '@') // Control-x

//
// send one character to the uart, but don't use
// interrupts or sleep(). safe to be called from
// interrupts, e.g. by printk and to echo input
// characters.
//
శూన్య
consputc(పూర్ణాంకం c)
{
  ఒకవేళ (c == BACKSPACE) {
    // if the user typed backspace, overwrite with a space.
    uartputc_sync('\b');
    uartputc_sync(' ');
    uartputc_sync('\b');
  } లేకపోతే {
    uartputc_sync(c);
  }
}

నిర్మాణం {
  నిర్మాణం spinlock తాళం;

  // input circular buffer
#నిర్వచించు ఆదానం_BUF_కొలత 128
  అక్షరం buf[ఆదానం_BUF_కొలత];
  uint r; // Read index
  uint w; // Write index
  uint e; // Edit index
} cons;

//
// user write() system calls to the console go here.
// uses sleep() and UART interrupts.
//
పూర్ణాంకం
consolewrite(పూర్ణాంకం user_src, uint64 src, పూర్ణాంకం n)
{
  అక్షరం buf[32]; // move batches from user space to uart.
  పూర్ణాంకం i = 0;

  అయితే (i < n) {
    పూర్ణాంకం nn = పరిమాణం(buf);
    ఒకవేళ (nn > n - i)
      nn = n - i;
    ఒకవేళ (either_copyin(buf, user_src, src + i, nn) == -1)
      ఆపు;
    uartwrite(buf, nn);
    i += nn;
  }

  తిరిగి i;
}

//
// user read()s from the console go here.
// copy (up to) a whole input line to dst.
// user_dst indicates whether dst is a user
// or kernel address.
//
పూర్ణాంకం
consoleread(పూర్ణాంకం user_dst, uint64 dst, పూర్ణాంకం n)
{
  uint లక్ష్యం;
  పూర్ణాంకం c;
  అక్షరం cbuf;

  లక్ష్యం = n;
  acquire(&cons.తాళం);
  అయితే (n > 0) {
    // wait until interrupt handler has put some
    // input into cons.buffer.
    అయితే (cons.r == cons.w) {
      ఒకవేళ (killed(myproc())) {
        release(&cons.తాళం);
        తిరిగి -1;
      }
      sleep_prepare(&cons.r);
      release(&cons.తాళం);
      sleep();
      acquire(&cons.తాళం);
    }

    c = cons.buf[cons.r++ % ఆదానం_BUF_కొలత];

    ఒకవేళ (c == C('D')) { // end-of-file
      ఒకవేళ (n < లక్ష్యం) {
        // Save ^D for next time, to make sure
        // caller gets a 0-byte result.
        cons.r--;
      }
      ఆపు;
    }

    // copy the input byte to the user-space buffer.
    cbuf = c;
    ఒకవేళ (either_copyout(user_dst, dst, &cbuf, 1) == -1)
      ఆపు;

    dst++;
    --n;

    ఒకవేళ (c == '\n') {
      // a whole line has arrived, return to
      // the user-level read().
      ఆపు;
    }
  }
  release(&cons.తాళం);

  తిరిగి లక్ష్యం - n;
}

//
// the console input interrupt handler.
// uartintr() calls this for each input character.
// do erase/kill processing, append to cons.buf,
// wake up consoleread() if a whole line has arrived.
//
శూన్య
consoleintr(పూర్ణాంకం c)
{
  acquire(&cons.తాళం);

  ఎంపిక (c) {
  స్థితి C('P'): // Print process list.
    procdump();
    ఆపు;
  స్థితి C('U'): // Kill line.
    అయితే (cons.e != cons.w &&
           cons.buf[(cons.e - 1) % ఆదానం_BUF_కొలత] != '\n') {
      cons.e--;
      consputc(BACKSPACE);
    }
    ఆపు;
  స్థితి C('H'): // Backspace
  స్థితి '\x7f': // Delete key
    ఒకవేళ (cons.e != cons.w) {
      cons.e--;
      consputc(BACKSPACE);
    }
    ఆపు;
  అప్రమేయ:
    ఒకవేళ (c != 0 && cons.e - cons.r < ఆదానం_BUF_కొలత) {
      c = (c == '\r') ? '\n' : c;

      // echo back to the user.
      consputc(c);

      // store for consumption by consoleread().
      cons.buf[cons.e++ % ఆదానం_BUF_కొలత] = c;

      ఒకవేళ (c == '\n' || c == C('D') || cons.e - cons.r == ఆదానం_BUF_కొలత) {
        // wake up consoleread() if a whole line (or end-of-file)
        // has arrived.
        cons.w = cons.e;
        wakeup(&cons.r);
      }
    }
    ఆపు;
  }

  release(&cons.తాళం);
}

శూన్య
consoleinit(శూన్య)
{
  initlock(&cons.తాళం, "cons");

  uartinit();

  // connect read and write system calls
  // to consoleread and consolewrite.
  devsw[CONSOLE].పఠ = consoleread;
  devsw[CONSOLE].లిఖ = consolewrite;
}
