//
// low-level driver for 16550a UART.
//

#అన్తర్భావ "types.h"
#అన్తర్భావ "param.h"
#అన్తర్భావ "memlayout.h"
#అన్తర్భావ "riscv.h"
#అన్తర్భావ "spinlock.h"
#అన్తర్భావ "sleeplock.h"
#అన్తర్భావ "proc.h"
#అన్తర్భావ "defs.h"

// the UART control registers are memory-mapped
// at address UART0. this macro returns the
// address of one of the registers.
#నిర్వచించు Reg(reg) ((అస్థిర గుర్తులేని అక్షరం *)(UART0 + (reg)))

#నిర్వచించు ReadReg(reg)     (*(Reg(reg)))
#నిర్వచించు WriteReg(reg, v) (*(Reg(reg)) = (v))

// the UART control registers.
// some have different meanings for read vs write.
// see http://byterunner.com/16550.html
#నిర్వచించు RHR             0        // receive holding register (for input bytes)
#నిర్వచించు THR             0        // transmit holding register (for output bytes)
#నిర్వచించు IER             1        // interrupt enable register
#నిర్వచించు IER_RX_ENABLE   (1 << 0) // receiver interrupts
#నిర్వచించు IER_TX_ENABLE   (1 << 1) // transmit interrupts
#నిర్వచించు FCR             2        // FIFO control register
#నిర్వచించు FCR_FIFO_ENABLE (1 << 0)
#నిర్వచించు FCR_FIFO_CLEAR  (3 << 1) // clear the content of the two FIFOs
#నిర్వచించు ISR             2        // interrupt status register
#నిర్వచించు LCR             3        // line control register
#నిర్వచించు LCR_EIGHT_బిట్లు  (3 << 0)
#నిర్వచించు LCR_BAUD_LATCH  (1 << 7) // special mode to set baud rate
#నిర్వచించు LSR             5        // line status register
#నిర్వచించు LSR_RX_READY    (1 << 0) // input is waiting to be read from RHR
#నిర్వచించు LSR_TX_IDLE     (1 << 5) // THR can accept another character to send

// for sending threads to serialize their writes
స్థిర నిర్మాణం sleeplock tx_తాళం;
స్థిర పూర్ణాంకం tx_chan; // &tx_chan is the "wait channel"

బాహ్య అస్థిర పూర్ణాంకం panicking; // from printk.c
బాహ్య అస్థిర పూర్ణాంకం panicked;  // from printk.c

శూన్య
uartinit(శూన్య)
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
  WriteReg(LCR, LCR_EIGHT_బిట్లు);

  // reset and enable FIFOs.
  WriteReg(FCR, FCR_FIFO_ENABLE | FCR_FIFO_CLEAR);

  // enable transmit and receive interrupts.
  WriteReg(IER, IER_TX_ENABLE | IER_RX_ENABLE);

  initsleeplock(&tx_తాళం, "uart");
}

// transmit buf[] to the uart. it blocks if the
// uart is busy, so it cannot be called from
// interrupts, only from write() system calls.
శూన్య
uartwrite(అక్షరం buf[], పూర్ణాంకం n)
{
  acquiresleep(&tx_తాళం);

  పూర్ణాంకం i = 0;
  అయితే (i < n) {
    sleep_prepare(&tx_chan);
    ఒకవేళ (ReadReg(LSR) & LSR_TX_IDLE) {
      WriteReg(THR, buf[i]);
      i += 1;
    } లేకపోతే {
      sleep();
    }
  }

  releasesleep(&tx_తాళం);
}

// write a byte to the uart without using
// interrupts, for use by kernel printk() and
// to echo characters. it spins waiting for the uart's
// output register to be empty.
శూన్య
uartputc_sync(పూర్ణాంకం c)
{
  ఒకవేళ (panicking == 0)
    నెట్టు_off();

  ఒకవేళ (panicked) {
    కొరకు (;;)
      ;
  }

  // wait for UART to set Transmit Holding Empty in LSR.
  అయితే ((ReadReg(LSR) & LSR_TX_IDLE) == 0)
    ;
  WriteReg(THR, c);

  ఒకవేళ (panicking == 0)
    తీయి_off();
}

// try to read one input character from the UART.
// return -1 if none is waiting.
స్థిర పూర్ణాంకం
uartgetc(శూన్య)
{
  // is input ready?
  ఒకవేళ (ReadReg(LSR) & LSR_RX_READY) {
    తిరిగి ReadReg(RHR);
  } లేకపోతే {
    తిరిగి -1;
  }
}

// handle a uart interrupt, raised because input has
// arrived, or the uart is ready for more output, or
// both. called from devintr().
శూన్య
uartintr(శూన్య)
{
  ReadReg(ISR); // acknowledge the interrupt

  ఒకవేళ (ReadReg(LSR) & LSR_TX_IDLE) {
    // UART finished transmitting; wake up sending thread.
    wakeup(&tx_chan);
  }

  // read and process incoming characters, if any.
  అయితే (1) {
    పూర్ణాంకం c = uartgetc();
    ఒకవేళ (c == -1)
      ఆపు;
    consoleintr(c);
  }
}
