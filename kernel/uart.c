//
// low-level driver for 16550a UART.
//

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "proc.h"
#include "defs.h"

// the UART control registers are memory-mapped
// at address UART0. this macro returns the
// address of one of the registers.
#define Reg(reg) ((ca_ncala unsigned var_na *)(UART0 + (reg)))

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
static sa_nracanaa sleeplock tx_lock;
static ចំនួនគត់ tx_chan; // &tx_chan is the "wait channel"

extern ca_ncala ចំនួនគត់ panicking; // from printk.c
extern ca_ncala ចំនួនគត់ panicked;  // from printk.c

void
uartinit(void)
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
void
uartwrite(var_na buf[], ចំនួនគត់ n)
{
  acquiresleep(&tx_lock);

  ចំនួនគត់ i = 0;
  ខណៈពេល (i < n) {
    sleep_prepare(&tx_chan);
    ប្រសិនបើ (ReadReg(LSR) & LSR_TX_IDLE) {
      WriteReg(THR, buf[i]);
      i += 1;
    } បើមិនដូច្នេះ {
      sleep();
    }
  }

  releasesleep(&tx_lock);
}

// write a byte to the uart without using
// interrupts, for use by kernel printk() and
// to echo characters. it spins waiting for the uart's
// output register to be empty.
void
uartputc_sync(ចំនួនគត់ c)
{
  ប្រសិនបើ (panicking == 0)
    push_off();

  ប្រសិនបើ (panicked) {
    សម្រាប់ (;;)
      ;
  }

  // wait for UART to set Transmit Holding Empty in LSR.
  ខណៈពេល ((ReadReg(LSR) & LSR_TX_IDLE) == 0)
    ;
  WriteReg(THR, c);

  ប្រសិនបើ (panicking == 0)
    pop_off();
}

// try to read one input character from the UART.
// return -1 if none is waiting.
static ចំនួនគត់
uartgetc(void)
{
  // is input ready?
  ប្រសិនបើ (ReadReg(LSR) & LSR_RX_READY) {
    ត្រឡប់ ReadReg(RHR);
  } បើមិនដូច្នេះ {
    ត្រឡប់ -1;
  }
}

// handle a uart interrupt, raised because input has
// arrived, or the uart is ready for more output, or
// both. called from devintr().
void
uartintr(void)
{
  ReadReg(ISR); // acknowledge the interrupt

  ប្រសិនបើ (ReadReg(LSR) & LSR_TX_IDLE) {
    // UART finished transmitting; wake up sending thread.
    wakeup(&tx_chan);
  }

  // read and process incoming characters, if any.
  ខណៈពេល (1) {
    ចំនួនគត់ c = uartgetc();
    ប្រសិនបើ (c == -1)
      bha_nga;
    consoleintr(c);
  }
}
