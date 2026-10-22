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

#ଅନ୍ତର୍ଭାଵ <stdarg.h>

#ଅନ୍ତର୍ଭାଵ "types.h"
#ଅନ୍ତର୍ଭାଵ "param.h"
#ଅନ୍ତର୍ଭାଵ "spinlock.h"
#ଅନ୍ତର୍ଭାଵ "sleeplock.h"
#ଅନ୍ତର୍ଭାଵ "fs.h"
#ଅନ୍ତର୍ଭାଵ "file.h"
#ଅନ୍ତର୍ଭାଵ "memlayout.h"
#ଅନ୍ତର୍ଭାଵ "riscv.h"
#ଅନ୍ତର୍ଭାଵ "defs.h"
#ଅନ୍ତର୍ଭାଵ "proc.h"

#define BACKSPACE 0x100       // erase the last output character
#define C(x)      ((x) - '@') // Control-x

//
// send one character to the uart, but don't use
// interrupts or sleep(). safe to be called from
// interrupts, e.g. by printk and to echo input
// characters.
//
ଶୂନ୍ୟ
consputc(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c)
{
  ଯଦି (c == BACKSPACE) {
    // if the user typed backspace, overwrite with a space.
    uartputc_sync('\b');
    uartputc_sync(' ');
    uartputc_sync('\b');
  } ଅନ୍ୟଥା {
    uartputc_sync(c);
  }
}

ସଂରଚନା {
  ସଂରଚନା spinlock lock;

  // input circular buffer
#define ନିବେଶ_BUF_SIZE 128
  ଅକ୍ଷର buf[ନିବେଶ_BUF_SIZE];
  uint r; // Read index
  uint w; // Write index
  uint e; // Edit index
} cons;

//
// user write() system calls to the console go here.
// uses sleep() and UART interrupts.
//
ପୂର୍ଣ୍ଣସଂଖ୍ୟା
consolewrite(ପୂର୍ଣ୍ଣସଂଖ୍ୟା user_src, uint64 src, ପୂର୍ଣ୍ଣସଂଖ୍ୟା n)
{
  ଅକ୍ଷର buf[32]; // move batches from user space to uart.
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0;

  ଯେତେବେଳେ (i < n) {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା nn = ଆକାର(buf);
    ଯଦି (nn > n - i)
      nn = n - i;
    ଯଦି (either_copyin(buf, user_src, src + i, nn) == -1)
      ଭାଙ୍ଗ;
    uartwrite(buf, nn);
    i += nn;
  }

  ଫେରନ୍ତୁ i;
}

//
// user read()s from the console go here.
// copy (up to) a whole input line to dst.
// user_dst indicates whether dst is a user
// or kernel address.
//
ପୂର୍ଣ୍ଣସଂଖ୍ୟା
consoleread(ପୂର୍ଣ୍ଣସଂଖ୍ୟା user_dst, uint64 dst, ପୂର୍ଣ୍ଣସଂଖ୍ୟା n)
{
  uint target;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା c;
  ଅକ୍ଷର cbuf;

  target = n;
  acquire(&cons.lock);
  ଯେତେବେଳେ (n > 0) {
    // wait until interrupt handler has put some
    // input into cons.buffer.
    ଯେତେବେଳେ (cons.r == cons.w) {
      ଯଦି (killed(myproc())) {
        release(&cons.lock);
        ଫେରନ୍ତୁ -1;
      }
      sleep_prepare(&cons.r);
      release(&cons.lock);
      sleep();
      acquire(&cons.lock);
    }

    c = cons.buf[cons.r++ % ନିବେଶ_BUF_SIZE];

    ଯଦି (c == C('D')) { // end-of-file
      ଯଦି (n < target) {
        // Save ^D for next time, to make sure
        // caller gets a 0-byte result.
        cons.r--;
      }
      ଭାଙ୍ଗ;
    }

    // copy the input byte to the user-space buffer.
    cbuf = c;
    ଯଦି (either_copyout(user_dst, dst, &cbuf, 1) == -1)
      ଭାଙ୍ଗ;

    dst++;
    --n;

    ଯଦି (c == '\n') {
      // a whole line has arrived, return to
      // the user-level read().
      ଭାଙ୍ଗ;
    }
  }
  release(&cons.lock);

  ଫେରନ୍ତୁ target - n;
}

//
// the console input interrupt handler.
// uartintr() calls this for each input character.
// do erase/kill processing, append to cons.buf,
// wake up consoleread() if a whole line has arrived.
//
ଶୂନ୍ୟ
consoleintr(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c)
{
  acquire(&cons.lock);

  ବାଛ (c) {
  ସ୍ଥିତି C('P'): // Print process list.
    procdump();
    ଭାଙ୍ଗ;
  ସ୍ଥିତି C('U'): // Kill line.
    ଯେତେବେଳେ (cons.e != cons.w &&
           cons.buf[(cons.e - 1) % ନିବେଶ_BUF_SIZE] != '\n') {
      cons.e--;
      consputc(BACKSPACE);
    }
    ଭାଙ୍ଗ;
  ସ୍ଥିତି C('H'): // Backspace
  ସ୍ଥିତି '\x7f': // Delete key
    ଯଦି (cons.e != cons.w) {
      cons.e--;
      consputc(BACKSPACE);
    }
    ଭାଙ୍ଗ;
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    ଯଦି (c != 0 && cons.e - cons.r < ନିବେଶ_BUF_SIZE) {
      c = (c == '\r') ? '\n' : c;

      // echo back to the user.
      consputc(c);

      // store for consumption by consoleread().
      cons.buf[cons.e++ % ନିବେଶ_BUF_SIZE] = c;

      ଯଦି (c == '\n' || c == C('D') || cons.e - cons.r == ନିବେଶ_BUF_SIZE) {
        // wake up consoleread() if a whole line (or end-of-file)
        // has arrived.
        cons.w = cons.e;
        wakeup(&cons.r);
      }
    }
    ଭାଙ୍ଗ;
  }

  release(&cons.lock);
}

ଶୂନ୍ୟ
consoleinit(ଶୂନ୍ୟ)
{
  initlock(&cons.lock, "cons");

  uartinit();

  // connect read and write system calls
  // to consoleread and consolewrite.
  devsw[CONSOLE].ପଠ = consoleread;
  devsw[CONSOLE].ଲିଖ = consolewrite;
}
