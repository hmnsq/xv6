//
// low-level driver for 16550a UART.
//

#அந்தர்பாவ "types.h"
#அந்தர்பாவ "param.h"
#அந்தர்பாவ "memlayout.h"
#அந்தர்பாவ "riscv.h"
#அந்தர்பாவ "spinlock.h"
#அந்தர்பாவ "sleeplock.h"
#அந்தர்பாவ "proc.h"
#அந்தர்பாவ "defs.h"

// the UART control registers are memory-mapped
// at address UART0. this macro returns the
// address of one of the registers.
#define Reg(reg) ((நிலையற்ற unsigned எழுத்து *)(UART0 + (reg)))

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
நிலையான கட்டமைப்பு sleeplock tx_lock;
நிலையான முழு tx_chan; // &tx_chan is the "wait channel"

வெளி நிலையற்ற முழு panicking; // from printk.c
வெளி நிலையற்ற முழு panicked;  // from printk.c

வெறுமை
uartinit(வெறுமை)
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
வெறுமை
uartwrite(எழுத்து buf[], முழு n)
{
  acquiresleep(&tx_lock);

  முழு i = 0;
  போது (i < n) {
    sleep_prepare(&tx_chan);
    எனில் (ReadReg(LSR) & LSR_TX_IDLE) {
      WriteReg(THR, buf[i]);
      i += 1;
    } இல்லையெனில் {
      sleep();
    }
  }

  releasesleep(&tx_lock);
}

// write a byte to the uart without using
// interrupts, for use by kernel printk() and
// to echo characters. it spins waiting for the uart's
// output register to be empty.
வெறுமை
uartputc_sync(முழு c)
{
  எனில் (panicking == 0)
    தள்ளு_off();

  எனில் (panicked) {
    க்கு (;;)
      ;
  }

  // wait for UART to set Transmit Holding Empty in LSR.
  போது ((ReadReg(LSR) & LSR_TX_IDLE) == 0)
    ;
  WriteReg(THR, c);

  எனில் (panicking == 0)
    எடு_off();
}

// try to read one input character from the UART.
// return -1 if none is waiting.
நிலையான முழு
uartgetc(வெறுமை)
{
  // is input ready?
  எனில் (ReadReg(LSR) & LSR_RX_READY) {
    திரும்பு ReadReg(RHR);
  } இல்லையெனில் {
    திரும்பு -1;
  }
}

// handle a uart interrupt, raised because input has
// arrived, or the uart is ready for more output, or
// both. called from devintr().
வெறுமை
uartintr(வெறுமை)
{
  ReadReg(ISR); // acknowledge the interrupt

  எனில் (ReadReg(LSR) & LSR_TX_IDLE) {
    // UART finished transmitting; wake up sending thread.
    wakeup(&tx_chan);
  }

  // read and process incoming characters, if any.
  போது (1) {
    முழு c = uartgetc();
    எனில் (c == -1)
      நிறுத்து;
    consoleintr(c);
  }
}
