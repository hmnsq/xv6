//
// low-level driver for 16550a UART.
//

#شامل "types.h"
#شامل "param.h"
#شامل "memlayout.h"
#شامل "riscv.h"
#شامل "spinlock.h"
#شامل "sleeplock.h"
#شامل "proc.h"
#شامل "defs.h"

// the UART control registers are memory-mapped
// at address UART0. this macro returns the
// address of one of the registers.
#تعریف_کرو Reg(reg) ((متلون غیرعلامتی حرف *)(UART0 + (reg)))

#تعریف_کرو ReadReg(reg)     (*(Reg(reg)))
#تعریف_کرو WriteReg(reg, v) (*(Reg(reg)) = (v))

// the UART control registers.
// some have different meanings for read vs write.
// see http://byterunner.com/16550.html
#تعریف_کرو RHR             0        // receive holding register (for input bytes)
#تعریف_کرو THR             0        // transmit holding register (for output bytes)
#تعریف_کرو IER             1        // interrupt enable register
#تعریف_کرو IER_RX_ENABLE   (1 << 0) // receiver interrupts
#تعریف_کرو IER_TX_ENABLE   (1 << 1) // transmit interrupts
#تعریف_کرو FCR             2        // FIFO control register
#تعریف_کرو FCR_FIFO_ENABLE (1 << 0)
#تعریف_کرو FCR_FIFO_CLEAR  (3 << 1) // clear the content of the two FIFOs
#تعریف_کرو ISR             2        // interrupt status register
#تعریف_کرو LCR             3        // line control register
#تعریف_کرو LCR_EIGHT_بٹ  (3 << 0)
#تعریف_کرو LCR_BAUD_LATCH  (1 << 7) // special mode to set baud rate
#تعریف_کرو LSR             5        // line status register
#تعریف_کرو LSR_RX_READY    (1 << 0) // input is waiting to be read from RHR
#تعریف_کرو LSR_TX_IDLE     (1 << 5) // THR can accept another character to send

// for sending threads to serialize their writes
ساکن ساخت sleeplock tx_تالا;
ساکن صحیح tx_chan; // &tx_chan is the "wait channel"

خارجی متلون صحیح panicking; // from printk.c
خارجی متلون صحیح panicked;  // from printk.c

خالی
uartinit(خالی)
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
  WriteReg(LCR, LCR_EIGHT_بٹ);

  // reset and enable FIFOs.
  WriteReg(FCR, FCR_FIFO_ENABLE | FCR_FIFO_CLEAR);

  // enable transmit and receive interrupts.
  WriteReg(IER, IER_TX_ENABLE | IER_RX_ENABLE);

  initsleeplock(&tx_تالا, "uart");
}

// transmit buf[] to the uart. it blocks if the
// uart is busy, so it cannot be called from
// interrupts, only from write() system calls.
خالی
uartwrite(حرف buf[], صحیح n)
{
  acquiresleep(&tx_تالا);

  صحیح i = 0;
  جب (i < n) {
    sleep_prepare(&tx_chan);
    اگر (ReadReg(LSR) & LSR_TX_IDLE) {
      WriteReg(THR, buf[i]);
      i += 1;
    } ورنہ {
      sleep();
    }
  }

  releasesleep(&tx_تالا);
}

// write a byte to the uart without using
// interrupts, for use by kernel printk() and
// to echo characters. it spins waiting for the uart's
// output register to be empty.
خالی
uartputc_sync(صحیح c)
{
  اگر (panicking == 0)
    ڈالو_off();

  اگر (panicked) {
    برائے (;;)
      ;
  }

  // wait for UART to set Transmit Holding Empty in LSR.
  جب ((ReadReg(LSR) & LSR_TX_IDLE) == 0)
    ;
  WriteReg(THR, c);

  اگر (panicking == 0)
    نکالو_off();
}

// try to read one input character from the UART.
// return -1 if none is waiting.
ساکن صحیح
uartgetc(خالی)
{
  // is input ready?
  اگر (ReadReg(LSR) & LSR_RX_READY) {
    واپس ReadReg(RHR);
  } ورنہ {
    واپس -1;
  }
}

// handle a uart interrupt, raised because input has
// arrived, or the uart is ready for more output, or
// both. called from devintr().
خالی
uartintr(خالی)
{
  ReadReg(ISR); // acknowledge the interrupt

  اگر (ReadReg(LSR) & LSR_TX_IDLE) {
    // UART finished transmitting; wake up sending thread.
    wakeup(&tx_chan);
  }

  // read and process incoming characters, if any.
  جب (1) {
    صحیح c = uartgetc();
    اگر (c == -1)
      توڑو;
    consoleintr(c);
  }
}
