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

#അന്തര്ഭാവ <stdarg.h>

#അന്തര്ഭാവ "types.h"
#അന്തര്ഭാവ "param.h"
#അന്തര്ഭാവ "spinlock.h"
#അന്തര്ഭാവ "sleeplock.h"
#അന്തര്ഭാവ "fs.h"
#അന്തര്ഭാവ "file.h"
#അന്തര്ഭാവ "memlayout.h"
#അന്തര്ഭാവ "riscv.h"
#അന്തര്ഭാവ "defs.h"
#അന്തര്ഭാവ "proc.h"

#define BACKSPACE 0x100       // erase the last output character
#define C(x)      ((x) - '@') // Control-x

//
// send one character to the uart, but don't use
// interrupts or sleep(). safe to be called from
// interrupts, e.g. by printk and to echo input
// characters.
//
ശൂന്യം
consputc(പൂർണ്ണസംഖ്യ c)
{
  എങ്കിൽ (c == BACKSPACE) {
    // if the user typed backspace, overwrite with a space.
    uartputc_sync('\b');
    uartputc_sync(' ');
    uartputc_sync('\b');
  } അല്ലെങ്കിൽ {
    uartputc_sync(c);
  }
}

ഘടന {
  ഘടന spinlock lock;

  // input circular buffer
#define നിവേശം_BUF_SIZE 128
  അക്ഷരം buf[നിവേശം_BUF_SIZE];
  uint r; // Read index
  uint w; // Write index
  uint e; // Edit index
} cons;

//
// user write() system calls to the console go here.
// uses sleep() and UART interrupts.
//
പൂർണ്ണസംഖ്യ
consolewrite(പൂർണ്ണസംഖ്യ user_src, uint64 src, പൂർണ്ണസംഖ്യ n)
{
  അക്ഷരം buf[32]; // move batches from user space to uart.
  പൂർണ്ണസംഖ്യ i = 0;

  ആകുമ്പോൾ (i < n) {
    പൂർണ്ണസംഖ്യ nn = വലുപ്പം(buf);
    എങ്കിൽ (nn > n - i)
      nn = n - i;
    എങ്കിൽ (either_copyin(buf, user_src, src + i, nn) == -1)
      നിർത്തുക;
    uartwrite(buf, nn);
    i += nn;
  }

  തിരികെ i;
}

//
// user read()s from the console go here.
// copy (up to) a whole input line to dst.
// user_dst indicates whether dst is a user
// or kernel address.
//
പൂർണ്ണസംഖ്യ
consoleread(പൂർണ്ണസംഖ്യ user_dst, uint64 dst, പൂർണ്ണസംഖ്യ n)
{
  uint target;
  പൂർണ്ണസംഖ്യ c;
  അക്ഷരം cbuf;

  target = n;
  acquire(&cons.lock);
  ആകുമ്പോൾ (n > 0) {
    // wait until interrupt handler has put some
    // input into cons.buffer.
    ആകുമ്പോൾ (cons.r == cons.w) {
      എങ്കിൽ (killed(myproc())) {
        release(&cons.lock);
        തിരികെ -1;
      }
      sleep_prepare(&cons.r);
      release(&cons.lock);
      sleep();
      acquire(&cons.lock);
    }

    c = cons.buf[cons.r++ % നിവേശം_BUF_SIZE];

    എങ്കിൽ (c == C('D')) { // end-of-file
      എങ്കിൽ (n < target) {
        // Save ^D for next time, to make sure
        // caller gets a 0-byte result.
        cons.r--;
      }
      നിർത്തുക;
    }

    // copy the input byte to the user-space buffer.
    cbuf = c;
    എങ്കിൽ (either_copyout(user_dst, dst, &cbuf, 1) == -1)
      നിർത്തുക;

    dst++;
    --n;

    എങ്കിൽ (c == '\n') {
      // a whole line has arrived, return to
      // the user-level read().
      നിർത്തുക;
    }
  }
  release(&cons.lock);

  തിരികെ target - n;
}

//
// the console input interrupt handler.
// uartintr() calls this for each input character.
// do erase/kill processing, append to cons.buf,
// wake up consoleread() if a whole line has arrived.
//
ശൂന്യം
consoleintr(പൂർണ്ണസംഖ്യ c)
{
  acquire(&cons.lock);

  തിരഞ്ഞെടുപ്പ് (c) {
  അവസ്ഥ C('P'): // Print process list.
    procdump();
    നിർത്തുക;
  അവസ്ഥ C('U'): // Kill line.
    ആകുമ്പോൾ (cons.e != cons.w &&
           cons.buf[(cons.e - 1) % നിവേശം_BUF_SIZE] != '\n') {
      cons.e--;
      consputc(BACKSPACE);
    }
    നിർത്തുക;
  അവസ്ഥ C('H'): // Backspace
  അവസ്ഥ '\x7f': // Delete key
    എങ്കിൽ (cons.e != cons.w) {
      cons.e--;
      consputc(BACKSPACE);
    }
    നിർത്തുക;
  സ്ഥിരസ്ഥിതി:
    എങ്കിൽ (c != 0 && cons.e - cons.r < നിവേശം_BUF_SIZE) {
      c = (c == '\r') ? '\n' : c;

      // echo back to the user.
      consputc(c);

      // store for consumption by consoleread().
      cons.buf[cons.e++ % നിവേശം_BUF_SIZE] = c;

      എങ്കിൽ (c == '\n' || c == C('D') || cons.e - cons.r == നിവേശം_BUF_SIZE) {
        // wake up consoleread() if a whole line (or end-of-file)
        // has arrived.
        cons.w = cons.e;
        wakeup(&cons.r);
      }
    }
    നിർത്തുക;
  }

  release(&cons.lock);
}

ശൂന്യം
consoleinit(ശൂന്യം)
{
  initlock(&cons.lock, "cons");

  uartinit();

  // connect read and write system calls
  // to consoleread and consolewrite.
  devsw[CONSOLE].പഠ = consoleread;
  devsw[CONSOLE].ലിഖ = consolewrite;
}
