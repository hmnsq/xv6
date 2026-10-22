//
// low-level driver for 16550a UART.
//

#अन्तर्भाव "types.h"
#अन्तर्भाव "param.h"
#अन्तर्भाव "memlayout.h"
#अन्तर्भाव "riscv.h"
#अन्तर्भाव "spinlock.h"
#अन्तर्भाव "sleeplock.h"
#अन्तर्भाव "proc.h"
#अन्तर्भाव "defs.h"

// the UART control registers are memory-mapped
// at address UART0. this macro returns the
// address of one of the registers.
#परिभाषय Reg(reg) ((चञ्चल अचिह्नित वर्ण *)(UART0 + (reg)))

#परिभाषय ReadReg(reg)     (*(Reg(reg)))
#परिभाषय WriteReg(reg, v) (*(Reg(reg)) = (v))

// the UART control registers.
// some have different meanings for read vs write.
// see http://byterunner.com/16550.html
#परिभाषय RHR             0        // receive holding register (for input bytes)
#परिभाषय THR             0        // transmit holding register (for output bytes)
#परिभाषय IER             1        // interrupt enable register
#परिभाषय IER_RX_ENABLE   (1 << 0) // receiver interrupts
#परिभाषय IER_TX_ENABLE   (1 << 1) // transmit interrupts
#परिभाषय FCR             2        // FIFO control register
#परिभाषय FCR_FIFO_ENABLE (1 << 0)
#परिभाषय FCR_FIFO_CLEAR  (3 << 1) // clear the content of the two FIFOs
#परिभाषय ISR             2        // interrupt status register
#परिभाषय LCR             3        // line control register
#परिभाषय LCR_EIGHT_द्वयङ्काः  (3 << 0)
#परिभाषय LCR_BAUD_LATCH  (1 << 7) // special mode to set baud rate
#परिभाषय LSR             5        // line status register
#परिभाषय LSR_RX_READY    (1 << 0) // input is waiting to be read from RHR
#परिभाषय LSR_TX_IDLE     (1 << 5) // THR can accept another character to send

// for sending threads to serialize their writes
स्थिर संरचना sleeplock tx_तालकम्;
स्थिर पूर्णाङ्क tx_chan; // &tx_chan is the "wait channel"

बाह्य चञ्चल पूर्णाङ्क panicking; // from printk.c
बाह्य चञ्चल पूर्णाङ्क panicked;  // from printk.c

शून्य
uartinit(शून्य)
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
  WriteReg(LCR, LCR_EIGHT_द्वयङ्काः);

  // reset and enable FIFOs.
  WriteReg(FCR, FCR_FIFO_ENABLE | FCR_FIFO_CLEAR);

  // enable transmit and receive interrupts.
  WriteReg(IER, IER_TX_ENABLE | IER_RX_ENABLE);

  initsleeplock(&tx_तालकम्, "uart");
}

// transmit buf[] to the uart. it blocks if the
// uart is busy, so it cannot be called from
// interrupts, only from write() system calls.
शून्य
uartwrite(वर्ण buf[], पूर्णाङ्क n)
{
  acquiresleep(&tx_तालकम्);

  पूर्णाङ्क i = 0;
  यावत् (i < n) {
    sleep_prepare(&tx_chan);
    यदि (ReadReg(LSR) & LSR_TX_IDLE) {
      WriteReg(THR, buf[i]);
      i += 1;
    } अन्यथा {
      sleep();
    }
  }

  releasesleep(&tx_तालकम्);
}

// write a byte to the uart without using
// interrupts, for use by kernel printk() and
// to echo characters. it spins waiting for the uart's
// output register to be empty.
शून्य
uartputc_sync(पूर्णाङ्क c)
{
  यदि (panicking == 0)
    प्रक्षिप_off();

  यदि (panicked) {
    प्रति (;;)
      ;
  }

  // wait for UART to set Transmit Holding Empty in LSR.
  यावत् ((ReadReg(LSR) & LSR_TX_IDLE) == 0)
    ;
  WriteReg(THR, c);

  यदि (panicking == 0)
    निष्कासय_off();
}

// try to read one input character from the UART.
// return -1 if none is waiting.
स्थिर पूर्णाङ्क
uartgetc(शून्य)
{
  // is input ready?
  यदि (ReadReg(LSR) & LSR_RX_READY) {
    प्रत्यागम ReadReg(RHR);
  } अन्यथा {
    प्रत्यागम -1;
  }
}

// handle a uart interrupt, raised because input has
// arrived, or the uart is ready for more output, or
// both. called from devintr().
शून्य
uartintr(शून्य)
{
  ReadReg(ISR); // acknowledge the interrupt

  यदि (ReadReg(LSR) & LSR_TX_IDLE) {
    // UART finished transmitting; wake up sending thread.
    wakeup(&tx_chan);
  }

  // read and process incoming characters, if any.
  यावत् (1) {
    पूर्णाङ्क c = uartgetc();
    यदि (c == -1)
      भङ्ग;
    consoleintr(c);
  }
}
