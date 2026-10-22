//
// low-level driver for 16550a UART.
//

#അന്തര്ഭാവ "types.h"
#അന്തര്ഭാവ "param.h"
#അന്തര്ഭാവ "memlayout.h"
#അന്തര്ഭാവ "riscv.h"
#അന്തര്ഭാവ "spinlock.h"
#അന്തര്ഭാവ "sleeplock.h"
#അന്തര്ഭാവ "proc.h"
#അന്തര്ഭാവ "defs.h"

// the UART control registers are memory-mapped
// at address UART0. this macro returns the
// address of one of the registers.
#define Reg(reg) ((അസ്ഥിര unsigned അക്ഷരം *)(UART0 + (reg)))

#define ReadReg(reg)     (*(Reg(reg)))
#define WriteReg(reg, v) (*(Reg(reg)) = (v))

// the UART control registers.
// some have different meanings for read vs write.
// see http://byterunner.com/16550.html
#define RHR             0        // receive holding register (for input bytes)
#define THR             0        // transmit holding register (for output bytes)
#define IER             1        // interrupt enable register
#define IER_RX_ENABLE   (1 << 0) // receiver interrupts
#define IER_TX_ENABLE   (1 << 1) // transmit interrupts
#define FCR             2        // FIFO control register
#define FCR_FIFO_ENABLE (1 << 0)
#define FCR_FIFO_CLEAR  (3 << 1) // clear the content of the two FIFOs
#define ISR             2        // interrupt status register
#define LCR             3        // line control register
#define LCR_EIGHT_BITS  (3 << 0)
#define LCR_BAUD_LATCH  (1 << 7) // special mode to set baud rate
#define LSR             5        // line status register
#define LSR_RX_READY    (1 << 0) // input is waiting to be read from RHR
#define LSR_TX_IDLE     (1 << 5) // THR can accept another character to send

// for sending threads to serialize their writes
സ്ഥിതം ഘടന sleeplock tx_lock;
സ്ഥിതം പൂർണ്ണസംഖ്യ tx_chan; // &tx_chan is the "wait channel"

ബാഹ്യ അസ്ഥിര പൂർണ്ണസംഖ്യ panicking; // from printk.c
ബാഹ്യ അസ്ഥിര പൂർണ്ണസംഖ്യ panicked;  // from printk.c

ശൂന്യം
uartinit(ശൂന്യം)
{
  // disable interrupts.
  WriteReg(IER, 0x00);

  // special mode to set baud rate.
  WriteReg(LCR, LCR_BAUD_LATCH);

  // LSB for baud rate of 38.4K.
  WriteReg(0, 0x03);

  // MSB for baud rate of 38.4K.
  WriteReg(1, 0x00);

  // leave set-baud mode,
  // and set word length to 8 bits, no parity.
  WriteReg(LCR, LCR_EIGHT_BITS);

  // reset and enable FIFOs.
  WriteReg(FCR, FCR_FIFO_ENABLE | FCR_FIFO_CLEAR);

  // enable transmit and receive interrupts.
  WriteReg(IER, IER_TX_ENABLE | IER_RX_ENABLE);

  initsleeplock(&tx_lock, "uart");
}

// transmit buf[] to the uart. it blocks if the
// uart is busy, so it cannot be called from
// interrupts, only from write() system calls.
ശൂന്യം
uartwrite(അക്ഷരം buf[], പൂർണ്ണസംഖ്യ n)
{
  acquiresleep(&tx_lock);

  പൂർണ്ണസംഖ്യ i = 0;
  ആകുമ്പോൾ (i < n) {
    sleep_prepare(&tx_chan);
    എങ്കിൽ (ReadReg(LSR) & LSR_TX_IDLE) {
      WriteReg(THR, buf[i]);
      i += 1;
    } അല്ലെങ്കിൽ {
      sleep();
    }
  }

  releasesleep(&tx_lock);
}

// write a byte to the uart without using
// interrupts, for use by kernel printk() and
// to echo characters. it spins waiting for the uart's
// output register to be empty.
ശൂന്യം
uartputc_sync(പൂർണ്ണസംഖ്യ c)
{
  എങ്കിൽ (panicking == 0)
    തള്ളുക_off();

  എങ്കിൽ (panicked) {
    വേണ്ടി (;;)
      ;
  }

  // wait for UART to set Transmit Holding Empty in LSR.
  ആകുമ്പോൾ ((ReadReg(LSR) & LSR_TX_IDLE) == 0)
    ;
  WriteReg(THR, c);

  എങ്കിൽ (panicking == 0)
    എടുക്കുക_off();
}

// try to read one input character from the UART.
// return -1 if none is waiting.
സ്ഥിതം പൂർണ്ണസംഖ്യ
uartgetc(ശൂന്യം)
{
  // is input ready?
  എങ്കിൽ (ReadReg(LSR) & LSR_RX_READY) {
    തിരികെ ReadReg(RHR);
  } അല്ലെങ്കിൽ {
    തിരികെ -1;
  }
}

// handle a uart interrupt, raised because input has
// arrived, or the uart is ready for more output, or
// both. called from devintr().
ശൂന്യം
uartintr(ശൂന്യം)
{
  ReadReg(ISR); // acknowledge the interrupt

  എങ്കിൽ (ReadReg(LSR) & LSR_TX_IDLE) {
    // UART finished transmitting; wake up sending thread.
    wakeup(&tx_chan);
  }

  // read and process incoming characters, if any.
  ആകുമ്പോൾ (1) {
    പൂർണ്ണസംഖ്യ c = uartgetc();
    എങ്കിൽ (c == -1)
      നിർത്തുക;
    consoleintr(c);
  }
}
