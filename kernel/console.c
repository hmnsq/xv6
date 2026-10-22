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

#ਅਨ੍ਤਰ੍ਭਾਵ <stdarg.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "types.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "param.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "spinlock.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "sleeplock.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "fs.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "file.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "memlayout.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "riscv.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "defs.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "proc.h"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BACKSPACE 0x100       // erase the last output character
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ C(x)      ((x) - '@') // Control-x

//
// send one character to the uart, but don't use
// interrupts or sleep(). safe to be called from
// interrupts, e.g. by printk and to echo input
// characters.
//
ਖਾਲੀ
consputc(ਪੂਰਨ c)
{
  ਜੇ (c == BACKSPACE) {
    // if the user typed backspace, overwrite with a space.
    uartputc_sync('\b');
    uartputc_sync(' ');
    uartputc_sync('\b');
  } ਨਹੀਂਤਾਂ {
    uartputc_sync(c);
  }
}

ਬਣਤਰ {
  ਬਣਤਰ spinlock ਤਾਲਾ;

  // input circular buffer
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਨਿਵੇਸ਼_BUF_ਮਾਪ 128
  ਅੱਖਰ buf[ਨਿਵੇਸ਼_BUF_ਮਾਪ];
  uint r; // Read index
  uint w; // Write index
  uint e; // Edit index
} cons;

//
// user write() system calls to the console go here.
// uses sleep() and UART interrupts.
//
ਪੂਰਨ
consolewrite(ਪੂਰਨ user_src, uint64 src, ਪੂਰਨ n)
{
  ਅੱਖਰ buf[32]; // move batches from user space to uart.
  ਪੂਰਨ i = 0;

  ਜਦੋਂ (i < n) {
    ਪੂਰਨ nn = ਆਕਾਰ(buf);
    ਜੇ (nn > n - i)
      nn = n - i;
    ਜੇ (either_copyin(buf, user_src, src + i, nn) == -1)
      ਰੋਕੋ;
    uartwrite(buf, nn);
    i += nn;
  }

  ਵਾਪਸ i;
}

//
// user read()s from the console go here.
// copy (up to) a whole input line to dst.
// user_dst indicates whether dst is a user
// or kernel address.
//
ਪੂਰਨ
consoleread(ਪੂਰਨ user_dst, uint64 dst, ਪੂਰਨ n)
{
  uint ਨਿਸ਼ਾਨਾ;
  ਪੂਰਨ c;
  ਅੱਖਰ cbuf;

  ਨਿਸ਼ਾਨਾ = n;
  acquire(&cons.ਤਾਲਾ);
  ਜਦੋਂ (n > 0) {
    // wait until interrupt handler has put some
    // input into cons.buffer.
    ਜਦੋਂ (cons.r == cons.w) {
      ਜੇ (killed(myproc())) {
        release(&cons.ਤਾਲਾ);
        ਵਾਪਸ -1;
      }
      sleep_prepare(&cons.r);
      release(&cons.ਤਾਲਾ);
      sleep();
      acquire(&cons.ਤਾਲਾ);
    }

    c = cons.buf[cons.r++ % ਨਿਵੇਸ਼_BUF_ਮਾਪ];

    ਜੇ (c == C('D')) { // end-of-file
      ਜੇ (n < ਨਿਸ਼ਾਨਾ) {
        // Save ^D for next time, to make sure
        // caller gets a 0-byte result.
        cons.r--;
      }
      ਰੋਕੋ;
    }

    // copy the input byte to the user-space buffer.
    cbuf = c;
    ਜੇ (either_copyout(user_dst, dst, &cbuf, 1) == -1)
      ਰੋਕੋ;

    dst++;
    --n;

    ਜੇ (c == '\n') {
      // a whole line has arrived, return to
      // the user-level read().
      ਰੋਕੋ;
    }
  }
  release(&cons.ਤਾਲਾ);

  ਵਾਪਸ ਨਿਸ਼ਾਨਾ - n;
}

//
// the console input interrupt handler.
// uartintr() calls this for each input character.
// do erase/kill processing, append to cons.buf,
// wake up consoleread() if a whole line has arrived.
//
ਖਾਲੀ
consoleintr(ਪੂਰਨ c)
{
  acquire(&cons.ਤਾਲਾ);

  ਚੋਣ (c) {
  ਹਾਲਤ C('P'): // Print process list.
    procdump();
    ਰੋਕੋ;
  ਹਾਲਤ C('U'): // Kill line.
    ਜਦੋਂ (cons.e != cons.w &&
           cons.buf[(cons.e - 1) % ਨਿਵੇਸ਼_BUF_ਮਾਪ] != '\n') {
      cons.e--;
      consputc(BACKSPACE);
    }
    ਰੋਕੋ;
  ਹਾਲਤ C('H'): // Backspace
  ਹਾਲਤ '\x7f': // Delete key
    ਜੇ (cons.e != cons.w) {
      cons.e--;
      consputc(BACKSPACE);
    }
    ਰੋਕੋ;
  ਮੂਲ:
    ਜੇ (c != 0 && cons.e - cons.r < ਨਿਵੇਸ਼_BUF_ਮਾਪ) {
      c = (c == '\r') ? '\n' : c;

      // echo back to the user.
      consputc(c);

      // store for consumption by consoleread().
      cons.buf[cons.e++ % ਨਿਵੇਸ਼_BUF_ਮਾਪ] = c;

      ਜੇ (c == '\n' || c == C('D') || cons.e - cons.r == ਨਿਵੇਸ਼_BUF_ਮਾਪ) {
        // wake up consoleread() if a whole line (or end-of-file)
        // has arrived.
        cons.w = cons.e;
        wakeup(&cons.r);
      }
    }
    ਰੋਕੋ;
  }

  release(&cons.ਤਾਲਾ);
}

ਖਾਲੀ
consoleinit(ਖਾਲੀ)
{
  initlock(&cons.ਤਾਲਾ, "cons");

  uartinit();

  // connect read and write system calls
  // to consoleread and consolewrite.
  devsw[CONSOLE].ਪਠ = consoleread;
  devsw[CONSOLE].ਲਿਖ = consolewrite;
}
