//
// low-level driver for 16550a UART.
//

#ಅನ್ತರ್ಭಾವ "types.h"
#ಅನ್ತರ್ಭಾವ "param.h"
#ಅನ್ತರ್ಭಾವ "memlayout.h"
#ಅನ್ತರ್ಭಾವ "riscv.h"
#ಅನ್ತರ್ಭಾವ "spinlock.h"
#ಅನ್ತರ್ಭಾವ "sleeplock.h"
#ಅನ್ತರ್ಭಾವ "proc.h"
#ಅನ್ತರ್ಭಾವ "defs.h"

// the UART control registers are memory-mapped
// at address UART0. this macro returns the
// address of one of the registers.
#define Reg(reg) ((ಚಞ್ಚಲ unsigned ವರ್ಣ *)(UART0 + (reg)))

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
ಸ್ಥಿರ ಸಂರಚನಾ sleeplock tx_lock;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ tx_chan; // &tx_chan is the "wait channel"

ಬಾಹ್ಯ ಚಞ್ಚಲ ಪೂರ್ಣಾಙ್ಕ panicking; // from printk.c
ಬಾಹ್ಯ ಚಞ್ಚಲ ಪೂರ್ಣಾಙ್ಕ panicked;  // from printk.c

ಶೂನ್ಯ
uartinit(ಶೂನ್ಯ)
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
ಶೂನ್ಯ
uartwrite(ವರ್ಣ buf[], ಪೂರ್ಣಾಙ್ಕ n)
{
  acquiresleep(&tx_lock);

  ಪೂರ್ಣಾಙ್ಕ i = 0;
  ಯಾವತ್ (i < n) {
    sleep_prepare(&tx_chan);
    ಯದಿ (ReadReg(LSR) & LSR_TX_IDLE) {
      WriteReg(THR, buf[i]);
      i += 1;
    } ಅನ್ಯಥಾ {
      sleep();
    }
  }

  releasesleep(&tx_lock);
}

// write a byte to the uart without using
// interrupts, for use by kernel printk() and
// to echo characters. it spins waiting for the uart's
// output register to be empty.
ಶೂನ್ಯ
uartputc_sync(ಪೂರ್ಣಾಙ್ಕ c)
{
  ಯದಿ (panicking == 0)
    ತಳ್ಳು_off();

  ಯದಿ (panicked) {
    ಪ್ರತಿ (;;)
      ;
  }

  // wait for UART to set Transmit Holding Empty in LSR.
  ಯಾವತ್ ((ReadReg(LSR) & LSR_TX_IDLE) == 0)
    ;
  WriteReg(THR, c);

  ಯದಿ (panicking == 0)
    ತೆಗೆ_off();
}

// try to read one input character from the UART.
// return -1 if none is waiting.
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
uartgetc(ಶೂನ್ಯ)
{
  // is input ready?
  ಯದಿ (ReadReg(LSR) & LSR_RX_READY) {
    ಪ್ರತ್ಯಾಗಮ ReadReg(RHR);
  } ಅನ್ಯಥಾ {
    ಪ್ರತ್ಯಾಗಮ -1;
  }
}

// handle a uart interrupt, raised because input has
// arrived, or the uart is ready for more output, or
// both. called from devintr().
ಶೂನ್ಯ
uartintr(ಶೂನ್ಯ)
{
  ReadReg(ISR); // acknowledge the interrupt

  ಯದಿ (ReadReg(LSR) & LSR_TX_IDLE) {
    // UART finished transmitting; wake up sending thread.
    wakeup(&tx_chan);
  }

  // read and process incoming characters, if any.
  ಯಾವತ್ (1) {
    ಪೂರ್ಣಾಙ್ಕ c = uartgetc();
    ಯದಿ (c == -1)
      ಭಙ್ಗ;
    consoleintr(c);
  }
}
