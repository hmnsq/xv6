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
#ವ್ಯಾಖ್ಯಾನಿಸು Reg(reg) ((ಅಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)(UART0 + (reg)))

#ವ್ಯಾಖ್ಯಾನಿಸು ReadReg(reg)     (*(Reg(reg)))
#ವ್ಯಾಖ್ಯಾನಿಸು WriteReg(reg, v) (*(Reg(reg)) = (v))

// the UART control registers.
// some have different meanings for read vs write.
// see http://byterunner.com/16550.html
#ವ್ಯಾಖ್ಯಾನಿಸು RHR             0        // receive holding register (for input bytes)
#ವ್ಯಾಖ್ಯಾನಿಸು THR             0        // transmit holding register (for output bytes)
#ವ್ಯಾಖ್ಯಾನಿಸು IER             1        // interrupt enable register
#ವ್ಯಾಖ್ಯಾನಿಸು IER_RX_ENABLE   (1 << 0) // receiver interrupts
#ವ್ಯಾಖ್ಯಾನಿಸು IER_TX_ENABLE   (1 << 1) // transmit interrupts
#ವ್ಯಾಖ್ಯಾನಿಸು FCR             2        // FIFO control register
#ವ್ಯಾಖ್ಯಾನಿಸು FCR_FIFO_ENABLE (1 << 0)
#ವ್ಯಾಖ್ಯಾನಿಸು FCR_FIFO_CLEAR  (3 << 1) // clear the content of the two FIFOs
#ವ್ಯಾಖ್ಯಾನಿಸು ISR             2        // interrupt status register
#ವ್ಯಾಖ್ಯಾನಿಸು LCR             3        // line control register
#ವ್ಯಾಖ್ಯಾನಿಸು LCR_EIGHT_ಬಿಟ್ಟುಗಳು  (3 << 0)
#ವ್ಯಾಖ್ಯಾನಿಸು LCR_BAUD_LATCH  (1 << 7) // special mode to set baud rate
#ವ್ಯಾಖ್ಯಾನಿಸು LSR             5        // line status register
#ವ್ಯಾಖ್ಯಾನಿಸು LSR_RX_READY    (1 << 0) // input is waiting to be read from RHR
#ವ್ಯಾಖ್ಯಾನಿಸು LSR_TX_IDLE     (1 << 5) // THR can accept another character to send

// for sending threads to serialize their writes
ಸ್ಥಿರ ರಚನೆ sleeplock tx_ಬೀಗ;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ tx_chan; // &tx_chan is the "wait channel"

ಬಾಹ್ಯ ಅಸ್ಥಿರ ಪೂರ್ಣಾಂಕ panicking; // from printk.c
ಬಾಹ್ಯ ಅಸ್ಥಿರ ಪೂರ್ಣಾಂಕ panicked;  // from printk.c

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
  WriteReg(LCR, LCR_EIGHT_ಬಿಟ್ಟುಗಳು);

  // reset and enable FIFOs.
  WriteReg(FCR, FCR_FIFO_ENABLE | FCR_FIFO_CLEAR);

  // enable transmit and receive interrupts.
  WriteReg(IER, IER_TX_ENABLE | IER_RX_ENABLE);

  initsleeplock(&tx_ಬೀಗ, "uart");
}

// transmit buf[] to the uart. it blocks if the
// uart is busy, so it cannot be called from
// interrupts, only from write() system calls.
ಶೂನ್ಯ
uartwrite(ಅಕ್ಷರ buf[], ಪೂರ್ಣಾಂಕ n)
{
  acquiresleep(&tx_ಬೀಗ);

  ಪೂರ್ಣಾಂಕ i = 0;
  ಆಗ (i < n) {
    sleep_prepare(&tx_chan);
    ಆದರೆ (ReadReg(LSR) & LSR_TX_IDLE) {
      WriteReg(THR, buf[i]);
      i += 1;
    } ಇಲ್ಲದಿದ್ದರೆ {
      sleep();
    }
  }

  releasesleep(&tx_ಬೀಗ);
}

// write a byte to the uart without using
// interrupts, for use by kernel printk() and
// to echo characters. it spins waiting for the uart's
// output register to be empty.
ಶೂನ್ಯ
uartputc_sync(ಪೂರ್ಣಾಂಕ c)
{
  ಆದರೆ (panicking == 0)
    ತಳ್ಳು_off();

  ಆದರೆ (panicked) {
    ಗಾಗಿ (;;)
      ;
  }

  // wait for UART to set Transmit Holding Empty in LSR.
  ಆಗ ((ReadReg(LSR) & LSR_TX_IDLE) == 0)
    ;
  WriteReg(THR, c);

  ಆದರೆ (panicking == 0)
    ತೆಗೆ_off();
}

// try to read one input character from the UART.
// return -1 if none is waiting.
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
uartgetc(ಶೂನ್ಯ)
{
  // is input ready?
  ಆದರೆ (ReadReg(LSR) & LSR_RX_READY) {
    ಹಿಂತಿರುಗಿ ReadReg(RHR);
  } ಇಲ್ಲದಿದ್ದರೆ {
    ಹಿಂತಿರುಗಿ -1;
  }
}

// handle a uart interrupt, raised because input has
// arrived, or the uart is ready for more output, or
// both. called from devintr().
ಶೂನ್ಯ
uartintr(ಶೂನ್ಯ)
{
  ReadReg(ISR); // acknowledge the interrupt

  ಆದರೆ (ReadReg(LSR) & LSR_TX_IDLE) {
    // UART finished transmitting; wake up sending thread.
    wakeup(&tx_chan);
  }

  // read and process incoming characters, if any.
  ಆಗ (1) {
    ಪೂರ್ಣಾಂಕ c = uartgetc();
    ಆದರೆ (c == -1)
      ನಿಲ್ಲಿಸು;
    consoleintr(c);
  }
}
