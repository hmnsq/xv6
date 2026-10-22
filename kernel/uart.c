//
// low-level driver for 16550a UART.
//

#અન્તર્ભાવ "types.h"
#અન્તર્ભાવ "param.h"
#અન્તર્ભાવ "memlayout.h"
#અન્તર્ભાવ "riscv.h"
#અન્તર્ભાવ "spinlock.h"
#અન્તર્ભાવ "sleeplock.h"
#અન્તર્ભાવ "proc.h"
#અન્તર્ભાવ "defs.h"

// the UART control registers are memory-mapped
// at address UART0. this macro returns the
// address of one of the registers.
#define Reg(reg) ((અસ્થિર unsigned અક્ષર *)(UART0 + (reg)))

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
સ્થિર સંરચના sleeplock tx_lock;
સ્થિર પૂર્ણાંક tx_chan; // &tx_chan is the "wait channel"

બાહ્ય અસ્થિર પૂર્ણાંક panicking; // from printk.c
બાહ્ય અસ્થિર પૂર્ણાંક panicked;  // from printk.c

શૂન્ય
uartinit(શૂન્ય)
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
શૂન્ય
uartwrite(અક્ષર buf[], પૂર્ણાંક n)
{
  acquiresleep(&tx_lock);

  પૂર્ણાંક i = 0;
  જ્યારે (i < n) {
    sleep_prepare(&tx_chan);
    જો (ReadReg(LSR) & LSR_TX_IDLE) {
      WriteReg(THR, buf[i]);
      i += 1;
    } નહીંતર {
      sleep();
    }
  }

  releasesleep(&tx_lock);
}

// write a byte to the uart without using
// interrupts, for use by kernel printk() and
// to echo characters. it spins waiting for the uart's
// output register to be empty.
શૂન્ય
uartputc_sync(પૂર્ણાંક c)
{
  જો (panicking == 0)
    ધકેલો_off();

  જો (panicked) {
    માટે (;;)
      ;
  }

  // wait for UART to set Transmit Holding Empty in LSR.
  જ્યારે ((ReadReg(LSR) & LSR_TX_IDLE) == 0)
    ;
  WriteReg(THR, c);

  જો (panicking == 0)
    કાઢો_off();
}

// try to read one input character from the UART.
// return -1 if none is waiting.
સ્થિર પૂર્ણાંક
uartgetc(શૂન્ય)
{
  // is input ready?
  જો (ReadReg(LSR) & LSR_RX_READY) {
    પાછા ReadReg(RHR);
  } નહીંતર {
    પાછા -1;
  }
}

// handle a uart interrupt, raised because input has
// arrived, or the uart is ready for more output, or
// both. called from devintr().
શૂન્ય
uartintr(શૂન્ય)
{
  ReadReg(ISR); // acknowledge the interrupt

  જો (ReadReg(LSR) & LSR_TX_IDLE) {
    // UART finished transmitting; wake up sending thread.
    wakeup(&tx_chan);
  }

  // read and process incoming characters, if any.
  જ્યારે (1) {
    પૂર્ણાંક c = uartgetc();
    જો (c == -1)
      તોડો;
    consoleintr(c);
  }
}
