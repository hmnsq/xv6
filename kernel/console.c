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

#அந்தர்பாவ <stdarg.h>

#அந்தர்பாவ "types.h"
#அந்தர்பாவ "param.h"
#அந்தர்பாவ "spinlock.h"
#அந்தர்பாவ "sleeplock.h"
#அந்தர்பாவ "fs.h"
#அந்தர்பாவ "file.h"
#அந்தர்பாவ "memlayout.h"
#அந்தர்பாவ "riscv.h"
#அந்தர்பாவ "defs.h"
#அந்தர்பாவ "proc.h"

#define BACKSPACE 0x100       // erase the last output character
#define C(x)      ((x) - '@') // Control-x

//
// send one character to the uart, but don't use
// interrupts or sleep(). safe to be called from
// interrupts, e.g. by printk and to echo input
// characters.
//
வெறுமை
consputc(முழு c)
{
  எனில் (c == BACKSPACE) {
    // if the user typed backspace, overwrite with a space.
    uartputc_sync('\b');
    uartputc_sync(' ');
    uartputc_sync('\b');
  } இல்லையெனில் {
    uartputc_sync(c);
  }
}

கட்டமைப்பு {
  கட்டமைப்பு spinlock lock;

  // input circular buffer
#define உள்ளீடு_BUF_SIZE 128
  எழுத்து buf[உள்ளீடு_BUF_SIZE];
  uint r; // Read index
  uint w; // Write index
  uint e; // Edit index
} cons;

//
// user write() system calls to the console go here.
// uses sleep() and UART interrupts.
//
முழு
consolewrite(முழு user_src, uint64 src, முழு n)
{
  எழுத்து buf[32]; // move batches from user space to uart.
  முழு i = 0;

  போது (i < n) {
    முழு nn = அளவு(buf);
    எனில் (nn > n - i)
      nn = n - i;
    எனில் (either_copyin(buf, user_src, src + i, nn) == -1)
      நிறுத்து;
    uartwrite(buf, nn);
    i += nn;
  }

  திரும்பு i;
}

//
// user read()s from the console go here.
// copy (up to) a whole input line to dst.
// user_dst indicates whether dst is a user
// or kernel address.
//
முழு
consoleread(முழு user_dst, uint64 dst, முழு n)
{
  uint target;
  முழு c;
  எழுத்து cbuf;

  target = n;
  acquire(&cons.lock);
  போது (n > 0) {
    // wait until interrupt handler has put some
    // input into cons.buffer.
    போது (cons.r == cons.w) {
      எனில் (killed(myproc())) {
        release(&cons.lock);
        திரும்பு -1;
      }
      sleep_prepare(&cons.r);
      release(&cons.lock);
      sleep();
      acquire(&cons.lock);
    }

    c = cons.buf[cons.r++ % உள்ளீடு_BUF_SIZE];

    எனில் (c == C('D')) { // end-of-file
      எனில் (n < target) {
        // Save ^D for next time, to make sure
        // caller gets a 0-byte result.
        cons.r--;
      }
      நிறுத்து;
    }

    // copy the input byte to the user-space buffer.
    cbuf = c;
    எனில் (either_copyout(user_dst, dst, &cbuf, 1) == -1)
      நிறுத்து;

    dst++;
    --n;

    எனில் (c == '\n') {
      // a whole line has arrived, return to
      // the user-level read().
      நிறுத்து;
    }
  }
  release(&cons.lock);

  திரும்பு target - n;
}

//
// the console input interrupt handler.
// uartintr() calls this for each input character.
// do erase/kill processing, append to cons.buf,
// wake up consoleread() if a whole line has arrived.
//
வெறுமை
consoleintr(முழு c)
{
  acquire(&cons.lock);

  தேர்வு (c) {
  நிலை C('P'): // Print process list.
    procdump();
    நிறுத்து;
  நிலை C('U'): // Kill line.
    போது (cons.e != cons.w &&
           cons.buf[(cons.e - 1) % உள்ளீடு_BUF_SIZE] != '\n') {
      cons.e--;
      consputc(BACKSPACE);
    }
    நிறுத்து;
  நிலை C('H'): // Backspace
  நிலை '\x7f': // Delete key
    எனில் (cons.e != cons.w) {
      cons.e--;
      consputc(BACKSPACE);
    }
    நிறுத்து;
  இயல்பு:
    எனில் (c != 0 && cons.e - cons.r < உள்ளீடு_BUF_SIZE) {
      c = (c == '\r') ? '\n' : c;

      // echo back to the user.
      consputc(c);

      // store for consumption by consoleread().
      cons.buf[cons.e++ % உள்ளீடு_BUF_SIZE] = c;

      எனில் (c == '\n' || c == C('D') || cons.e - cons.r == உள்ளீடு_BUF_SIZE) {
        // wake up consoleread() if a whole line (or end-of-file)
        // has arrived.
        cons.w = cons.e;
        wakeup(&cons.r);
      }
    }
    நிறுத்து;
  }

  release(&cons.lock);
}

வெறுமை
consoleinit(வெறுமை)
{
  initlock(&cons.lock, "cons");

  uartinit();

  // connect read and write system calls
  // to consoleread and consolewrite.
  devsw[CONSOLE].பட = consoleread;
  devsw[CONSOLE].லிக = consolewrite;
}
