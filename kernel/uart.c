//
// low-level driver for 16550a UART.
//

#ਅਨ੍ਤਰ੍ਭਾਵ "types.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "param.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "memlayout.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "riscv.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "spinlock.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "sleeplock.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "proc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "defs.h"

// the UART control registers are memory-mapped
// at address UART0. this macro returns the
// address of one of the registers.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Reg(reg) ((ਅਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)(UART0 + (reg)))

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ReadReg(reg)     (*(Reg(reg)))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WriteReg(reg, v) (*(Reg(reg)) = (v))

// the UART control registers.
// some have different meanings for read vs write.
// see http://byterunner.com/16550.html
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RHR             0        // receive holding register (for input bytes)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ THR             0        // transmit holding register (for output bytes)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IER             1        // interrupt enable register
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IER_RX_ENABLE   (1 << 0) // receiver interrupts
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IER_TX_ENABLE   (1 << 1) // transmit interrupts
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FCR             2        // FIFO control register
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FCR_FIFO_ENABLE (1 << 0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FCR_FIFO_CLEAR  (3 << 1) // clear the content of the two FIFOs
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ISR             2        // interrupt status register
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LCR             3        // line control register
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LCR_EIGHT_ਬਿੱਟ  (3 << 0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LCR_BAUD_LATCH  (1 << 7) // special mode to set baud rate
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LSR             5        // line status register
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LSR_RX_READY    (1 << 0) // input is waiting to be read from RHR
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LSR_TX_IDLE     (1 << 5) // THR can accept another character to send

// for sending threads to serialize their writes
ਸਥਿਰ ਬਣਤਰ sleeplock tx_ਤਾਲਾ;
ਸਥਿਰ ਪੂਰਨ tx_chan; // &tx_chan is the "wait channel"

ਬਾਹਰੀ ਅਸਥਿਰ ਪੂਰਨ panicking; // from printk.c
ਬਾਹਰੀ ਅਸਥਿਰ ਪੂਰਨ panicked;  // from printk.c

ਖਾਲੀ
uartinit(ਖਾਲੀ)
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
  WriteReg(LCR, LCR_EIGHT_ਬਿੱਟ);

  // reset and enable FIFOs.
  WriteReg(FCR, FCR_FIFO_ENABLE | FCR_FIFO_CLEAR);

  // enable transmit and receive interrupts.
  WriteReg(IER, IER_TX_ENABLE | IER_RX_ENABLE);

  initsleeplock(&tx_ਤਾਲਾ, "uart");
}

// transmit buf[] to the uart. it blocks if the
// uart is busy, so it cannot be called from
// interrupts, only from write() system calls.
ਖਾਲੀ
uartwrite(ਅੱਖਰ buf[], ਪੂਰਨ n)
{
  acquiresleep(&tx_ਤਾਲਾ);

  ਪੂਰਨ i = 0;
  ਜਦੋਂ (i < n) {
    sleep_prepare(&tx_chan);
    ਜੇ (ReadReg(LSR) & LSR_TX_IDLE) {
      WriteReg(THR, buf[i]);
      i += 1;
    } ਨਹੀਂਤਾਂ {
      sleep();
    }
  }

  releasesleep(&tx_ਤਾਲਾ);
}

// write a byte to the uart without using
// interrupts, for use by kernel printk() and
// to echo characters. it spins waiting for the uart's
// output register to be empty.
ਖਾਲੀ
uartputc_sync(ਪੂਰਨ c)
{
  ਜੇ (panicking == 0)
    ਧੱਕੋ_off();

  ਜੇ (panicked) {
    ਲਈ (;;)
      ;
  }

  // wait for UART to set Transmit Holding Empty in LSR.
  ਜਦੋਂ ((ReadReg(LSR) & LSR_TX_IDLE) == 0)
    ;
  WriteReg(THR, c);

  ਜੇ (panicking == 0)
    ਕੱਢੋ_off();
}

// try to read one input character from the UART.
// return -1 if none is waiting.
ਸਥਿਰ ਪੂਰਨ
uartgetc(ਖਾਲੀ)
{
  // is input ready?
  ਜੇ (ReadReg(LSR) & LSR_RX_READY) {
    ਵਾਪਸ ReadReg(RHR);
  } ਨਹੀਂਤਾਂ {
    ਵਾਪਸ -1;
  }
}

// handle a uart interrupt, raised because input has
// arrived, or the uart is ready for more output, or
// both. called from devintr().
ਖਾਲੀ
uartintr(ਖਾਲੀ)
{
  ReadReg(ISR); // acknowledge the interrupt

  ਜੇ (ReadReg(LSR) & LSR_TX_IDLE) {
    // UART finished transmitting; wake up sending thread.
    wakeup(&tx_chan);
  }

  // read and process incoming characters, if any.
  ਜਦੋਂ (1) {
    ਪੂਰਨ c = uartgetc();
    ਜੇ (c == -1)
      ਰੋਕੋ;
    consoleintr(c);
  }
}
