//
// low-level driver for 16550a UART.
//

#ଅନ୍ତର୍ଭାଵ "types.h"
#ଅନ୍ତର୍ଭାଵ "param.h"
#ଅନ୍ତର୍ଭାଵ "memlayout.h"
#ଅନ୍ତର୍ଭାଵ "riscv.h"
#ଅନ୍ତର୍ଭାଵ "spinlock.h"
#ଅନ୍ତର୍ଭାଵ "sleeplock.h"
#ଅନ୍ତର୍ଭାଵ "proc.h"
#ଅନ୍ତର୍ଭାଵ "defs.h"

// the UART control registers are memory-mapped
// at address UART0. this macro returns the
// address of one of the registers.
#define Reg(reg) ((ଅସ୍ଥିର unsigned ଅକ୍ଷର *)(UART0 + (reg)))

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
ସ୍ଥିର ସଂରଚନା sleeplock tx_lock;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା tx_chan; // &tx_chan is the "wait channel"

ବାହ୍ୟ ଅସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା panicking; // from printk.c
ବାହ୍ୟ ଅସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା panicked;  // from printk.c

ଶୂନ୍ୟ
uartinit(ଶୂନ୍ୟ)
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
ଶୂନ୍ୟ
uartwrite(ଅକ୍ଷର buf[], ପୂର୍ଣ୍ଣସଂଖ୍ୟା n)
{
  acquiresleep(&tx_lock);

  ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0;
  ଯେତେବେଳେ (i < n) {
    sleep_prepare(&tx_chan);
    ଯଦି (ReadReg(LSR) & LSR_TX_IDLE) {
      WriteReg(THR, buf[i]);
      i += 1;
    } ଅନ୍ୟଥା {
      sleep();
    }
  }

  releasesleep(&tx_lock);
}

// write a byte to the uart without using
// interrupts, for use by kernel printk() and
// to echo characters. it spins waiting for the uart's
// output register to be empty.
ଶୂନ୍ୟ
uartputc_sync(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c)
{
  ଯଦି (panicking == 0)
    ଠେଲ_off();

  ଯଦି (panicked) {
    ପାଇଁ (;;)
      ;
  }

  // wait for UART to set Transmit Holding Empty in LSR.
  ଯେତେବେଳେ ((ReadReg(LSR) & LSR_TX_IDLE) == 0)
    ;
  WriteReg(THR, c);

  ଯଦି (panicking == 0)
    କାଢ଼_off();
}

// try to read one input character from the UART.
// return -1 if none is waiting.
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
uartgetc(ଶୂନ୍ୟ)
{
  // is input ready?
  ଯଦି (ReadReg(LSR) & LSR_RX_READY) {
    ଫେରନ୍ତୁ ReadReg(RHR);
  } ଅନ୍ୟଥା {
    ଫେରନ୍ତୁ -1;
  }
}

// handle a uart interrupt, raised because input has
// arrived, or the uart is ready for more output, or
// both. called from devintr().
ଶୂନ୍ୟ
uartintr(ଶୂନ୍ୟ)
{
  ReadReg(ISR); // acknowledge the interrupt

  ଯଦି (ReadReg(LSR) & LSR_TX_IDLE) {
    // UART finished transmitting; wake up sending thread.
    wakeup(&tx_chan);
  }

  // read and process incoming characters, if any.
  ଯେତେବେଳେ (1) {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା c = uartgetc();
    ଯଦି (c == -1)
      ଭାଙ୍ଗ;
    consoleintr(c);
  }
}
