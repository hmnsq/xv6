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
#تعریف_کړه Reg(reg) ((متطاير بینښه حرف *)(UART0 + (reg)))

#تعریف_کړه ReadReg(reg)     (*(Reg(reg)))
#تعریف_کړه WriteReg(reg, v) (*(Reg(reg)) = (v))

// the UART control registers.
// some have different meanings for read vs write.
// see http://byterunner.com/16550.html
#تعریف_کړه RHR             0        // receive holding register (for input bytes)
#تعریف_کړه THR             0        // transmit holding register (for output bytes)
#تعریف_کړه IER             1        // interrupt enable register
#تعریف_کړه IER_RX_ENABLE   (1 << 0) // receiver interrupts
#تعریف_کړه IER_TX_ENABLE   (1 << 1) // transmit interrupts
#تعریف_کړه FCR             2        // FIFO control register
#تعریف_کړه FCR_FIFO_ENABLE (1 << 0)
#تعریف_کړه FCR_FIFO_CLEAR  (3 << 1) // clear the content of the two FIFOs
#تعریف_کړه ISR             2        // interrupt status register
#تعریف_کړه LCR             3        // line control register
#تعریف_کړه LCR_EIGHT_بټونه  (3 << 0)
#تعریف_کړه LCR_BAUD_LATCH  (1 << 7) // special mode to set baud rate
#تعریف_کړه LSR             5        // line status register
#تعریف_کړه LSR_RX_READY    (1 << 0) // input is waiting to be read from RHR
#تعریف_کړه LSR_TX_IDLE     (1 << 5) // THR can accept another character to send

// for sending threads to serialize their writes
ساكن بنية sleeplock tx_قلف;
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
  WriteReg(LCR, LCR_EIGHT_بټونه);

  // reset and enable FIFOs.
  WriteReg(FCR, FCR_FIFO_ENABLE | FCR_FIFO_CLEAR);

  // enable transmit and receive interrupts.
  WriteReg(IER, IER_TX_ENABLE | IER_RX_ENABLE);

  initsleeplock(&tx_قلف, "uart");
}

// transmit buf[] to the uart. it blocks if the
// uart is busy, so it cannot be called from
// interrupts, only from write() system calls.
فراغ
uartwrite(حرف buf[], صحيح n)
{
  acquiresleep(&tx_قلف);

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

  releasesleep(&tx_قلف);
}

// write a byte to the uart without using
// interrupts, for use by kernel printk() and
// to echo characters. it spins waiting for the uart's
// output register to be empty.
فراغ
uartputc_sync(صحيح c)
{
  إذا (panicking == 0)
    ټېل_off();

  إذا (panicked) {
    لكل (;;)
      ;
  }

  // wait for UART to set Transmit Holding Empty in LSR.
  طالما ((ReadReg(LSR) & LSR_TX_IDLE) == 0)
    ;
  WriteReg(THR, c);

  إذا (panicking == 0)
    راوباسه_off();
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
