//
// low-level driver for 16550a UART.
//

#समावेश "types.h"
#समावेश "param.h"
#समावेश "memlayout.h"
#समावेश "riscv.h"
#समावेश "spinlock.h"
#समावेश "sleeplock.h"
#समावेश "proc.h"
#समावेश "defs.h"

// the UART control registers are memory-mapped
// at address UART0. this macro returns the
// address of one of the registers.
#घोषणा Reg(reg) ((अस्थिर अचिन्हित अक्षर *)(UART0 + (reg)))

#घोषणा ReadReg(reg)     (*(Reg(reg)))
#घोषणा WriteReg(reg, v) (*(Reg(reg)) = (v))

// the UART control registers.
// some have different meanings for read vs write.
// see http://byterunner.com/16550.html
#घोषणा RHR             0        // receive holding register (for input bytes)
#घोषणा THR             0        // transmit holding register (for output bytes)
#घोषणा IER             1        // interrupt enable register
#घोषणा IER_RX_ENABLE   (1 << 0) // receiver interrupts
#घोषणा IER_TX_ENABLE   (1 << 1) // transmit interrupts
#घोषणा FCR             2        // FIFO control register
#घोषणा FCR_FIFO_ENABLE (1 << 0)
#घोषणा FCR_FIFO_CLEAR  (3 << 1) // clear the content of the two FIFOs
#घोषणा ISR             2        // interrupt status register
#घोषणा LCR             3        // line control register
#घोषणा LCR_EIGHT_BITS  (3 << 0)
#घोषणा LCR_BAUD_LATCH  (1 << 7) // special mode to set baud rate
#घोषणा LSR             5        // line status register
#घोषणा LSR_RX_READY    (1 << 0) // input is waiting to be read from RHR
#घोषणा LSR_TX_IDLE     (1 << 5) // THR can accept another character to send

// for sending threads to serialize their writes
अटल काष्ठा sleeplock tx_lock;
अटल पूर्णांक tx_chan; // &tx_chan is the "wait channel"

बाह्य अस्थिर पूर्णांक panicking; // from printk.c
बाह्य अस्थिर पूर्णांक panicked;  // from printk.c

व्योम
uartinit(व्योम)
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
व्योम
uartwrite(अक्षर buf[], पूर्णांक n)
{
  acquiresleep(&tx_lock);

  पूर्णांक i = 0;
  जबतक (i < n) {
    sleep_prepare(&tx_chan);
    अगर (ReadReg(LSR) & LSR_TX_IDLE) {
      WriteReg(THR, buf[i]);
      i += 1;
    } अन्यथा {
      sleep();
    }
  }

  releasesleep(&tx_lock);
}

// write a byte to the uart without using
// interrupts, for use by kernel printk() and
// to echo characters. it spins waiting for the uart's
// output register to be empty.
व्योम
uartputc_sync(पूर्णांक c)
{
  अगर (panicking == 0)
    धकेलो_off();

  अगर (panicked) {
    क्रम (;;)
      ;
  }

  // wait for UART to set Transmit Holding Empty in LSR.
  जबतक ((ReadReg(LSR) & LSR_TX_IDLE) == 0)
    ;
  WriteReg(THR, c);

  अगर (panicking == 0)
    निकालो_off();
}

// try to read one input character from the UART.
// return -1 if none is waiting.
अटल पूर्णांक
uartgetc(व्योम)
{
  // is input ready?
  अगर (ReadReg(LSR) & LSR_RX_READY) {
    वापस ReadReg(RHR);
  } अन्यथा {
    वापस -1;
  }
}

// handle a uart interrupt, raised because input has
// arrived, or the uart is ready for more output, or
// both. called from devintr().
व्योम
uartintr(व्योम)
{
  ReadReg(ISR); // acknowledge the interrupt

  अगर (ReadReg(LSR) & LSR_TX_IDLE) {
    // UART finished transmitting; wake up sending thread.
    wakeup(&tx_chan);
  }

  // read and process incoming characters, if any.
  जबतक (1) {
    पूर्णांक c = uartgetc();
    अगर (c == -1)
      अवरोध;
    consoleintr(c);
  }
}
