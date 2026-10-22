//
// low-level driver for 16550a UART.
//

#অন্তর্ভাব "types.h"
#অন্তর্ভাব "param.h"
#অন্তর্ভাব "memlayout.h"
#অন্তর্ভাব "riscv.h"
#অন্তর্ভাব "spinlock.h"
#অন্তর্ভাব "sleeplock.h"
#অন্তর্ভাব "proc.h"
#অন্তর্ভাব "defs.h"

// the UART control registers are memory-mapped
// at address UART0. this macro returns the
// address of one of the registers.
#define Reg(reg) ((অস্থির unsigned অক্ষর *)(UART0 + (reg)))

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
স্থির কাঠামো sleeplock tx_lock;
স্থির পূর্ণসংখ্যা tx_chan; // &tx_chan is the "wait channel"

বাহ্যিক অস্থির পূর্ণসংখ্যা panicking; // from printk.c
বাহ্যিক অস্থির পূর্ণসংখ্যা panicked;  // from printk.c

শূন্য
uartinit(শূন্য)
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
শূন্য
uartwrite(অক্ষর buf[], পূর্ণসংখ্যা n)
{
  acquiresleep(&tx_lock);

  পূর্ণসংখ্যা i = 0;
  যখন (i < n) {
    sleep_prepare(&tx_chan);
    যদি (ReadReg(LSR) & LSR_TX_IDLE) {
      WriteReg(THR, buf[i]);
      i += 1;
    } অন্যথা {
      sleep();
    }
  }

  releasesleep(&tx_lock);
}

// write a byte to the uart without using
// interrupts, for use by kernel printk() and
// to echo characters. it spins waiting for the uart's
// output register to be empty.
শূন্য
uartputc_sync(পূর্ণসংখ্যা c)
{
  যদি (panicking == 0)
    ঠেলো_off();

  যদি (panicked) {
    জন্য (;;)
      ;
  }

  // wait for UART to set Transmit Holding Empty in LSR.
  যখন ((ReadReg(LSR) & LSR_TX_IDLE) == 0)
    ;
  WriteReg(THR, c);

  যদি (panicking == 0)
    বের_করো_off();
}

// try to read one input character from the UART.
// return -1 if none is waiting.
স্থির পূর্ণসংখ্যা
uartgetc(শূন্য)
{
  // is input ready?
  যদি (ReadReg(LSR) & LSR_RX_READY) {
    ফেরত ReadReg(RHR);
  } অন্যথা {
    ফেরত -1;
  }
}

// handle a uart interrupt, raised because input has
// arrived, or the uart is ready for more output, or
// both. called from devintr().
শূন্য
uartintr(শূন্য)
{
  ReadReg(ISR); // acknowledge the interrupt

  যদি (ReadReg(LSR) & LSR_TX_IDLE) {
    // UART finished transmitting; wake up sending thread.
    wakeup(&tx_chan);
  }

  // read and process incoming characters, if any.
  যখন (1) {
    পূর্ণসংখ্যা c = uartgetc();
    যদি (c == -1)
      বিরতি;
    consoleintr(c);
  }
}
