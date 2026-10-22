//
// low-level driver for 16550a UART.
//

#ضمن "types.h"
#ضمن "param.h"
#ضمن "memlayout.h"
#ضمن "riscv.h"
#ضمن "spinlock.h"
#ضمن "sleeplock.h"
#ضمن "proc.h"
#ضمن "defs.h"

// the UART control registers are memory-mapped
// at address UART0. this macro returns the
// address of one of the registers.
#وصف_ڪريو Reg(reg) ((متطاير بنانشان حرف *)(UART0 + (reg)))

#وصف_ڪريو ReadReg(reg)     (*(Reg(reg)))
#وصف_ڪريو WriteReg(reg, v) (*(Reg(reg)) = (v))

// the UART control registers.
// some have different meanings for read vs write.
// see http://byterunner.com/16550.html
#وصف_ڪريو RHR             0        // receive holding register (for input bytes)
#وصف_ڪريو THR             0        // transmit holding register (for output bytes)
#وصف_ڪريو IER             1        // interrupt enable register
#وصف_ڪريو IER_RX_ENABLE   (1 << 0) // receiver interrupts
#وصف_ڪريو IER_TX_ENABLE   (1 << 1) // transmit interrupts
#وصف_ڪريو FCR             2        // FIFO control register
#وصف_ڪريو FCR_FIFO_ENABLE (1 << 0)
#وصف_ڪريو FCR_FIFO_CLEAR  (3 << 1) // clear the content of the two FIFOs
#وصف_ڪريو ISR             2        // interrupt status register
#وصف_ڪريو LCR             3        // line control register
#وصف_ڪريو LCR_EIGHT_بٽ  (3 << 0)
#وصف_ڪريو LCR_BAUD_LATCH  (1 << 7) // special mode to set baud rate
#وصف_ڪريو LSR             5        // line status register
#وصف_ڪريو LSR_RX_READY    (1 << 0) // input is waiting to be read from RHR
#وصف_ڪريو LSR_TX_IDLE     (1 << 5) // THR can accept another character to send

// for sending threads to serialize their writes
ساكن بنية sleeplock tx_تالو;
ساكن صحيح tx_chan; // &tx_chan is the "wait channel"

خارجي متطاير صحيح panicking; // from printk.c
خارجي متطاير صحيح panicked;  // from printk.c

فراغ
uartinit(فراغ)
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
  WriteReg(LCR, LCR_EIGHT_بٽ);

  // reset and enable FIFOs.
  WriteReg(FCR, FCR_FIFO_ENABLE | FCR_FIFO_CLEAR);

  // enable transmit and receive interrupts.
  WriteReg(IER, IER_TX_ENABLE | IER_RX_ENABLE);

  initsleeplock(&tx_تالو, "uart");
}

// transmit buf[] to the uart. it blocks if the
// uart is busy, so it cannot be called from
// interrupts, only from write() system calls.
فراغ
uartwrite(حرف buf[], صحيح n)
{
  acquiresleep(&tx_تالو);

  صحيح i = 0;
  طالما (i < n) {
    sleep_prepare(&tx_chan);
    إذا (ReadReg(LSR) & LSR_TX_IDLE) {
      WriteReg(THR, buf[i]);
      i += 1;
    } وإلا {
      sleep();
    }
  }

  releasesleep(&tx_تالو);
}

// write a byte to the uart without using
// interrupts, for use by kernel printk() and
// to echo characters. it spins waiting for the uart's
// output register to be empty.
فراغ
uartputc_sync(صحيح c)
{
  إذا (panicking == 0)
    ڌڪو_off();

  إذا (panicked) {
    لكل (;;)
      ;
  }

  // wait for UART to set Transmit Holding Empty in LSR.
  طالما ((ReadReg(LSR) & LSR_TX_IDLE) == 0)
    ;
  WriteReg(THR, c);

  إذا (panicking == 0)
    ڪڍو_off();
}

// try to read one input character from the UART.
// return -1 if none is waiting.
ساكن صحيح
uartgetc(فراغ)
{
  // is input ready?
  إذا (ReadReg(LSR) & LSR_RX_READY) {
    أرجع ReadReg(RHR);
  } وإلا {
    أرجع -1;
  }
}

// handle a uart interrupt, raised because input has
// arrived, or the uart is ready for more output, or
// both. called from devintr().
فراغ
uartintr(فراغ)
{
  ReadReg(ISR); // acknowledge the interrupt

  إذا (ReadReg(LSR) & LSR_TX_IDLE) {
    // UART finished transmitting; wake up sending thread.
    wakeup(&tx_chan);
  }

  // read and process incoming characters, if any.
  طالما (1) {
    صحيح c = uartgetc();
    إذا (c == -1)
      اكسر;
    consoleintr(c);
  }
}
