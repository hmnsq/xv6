#ଅନ୍ତର୍ଭାଵ "types.h"
#ଅନ୍ତର୍ଭାଵ "param.h"
#ଅନ୍ତର୍ଭାଵ "memlayout.h"
#ଅନ୍ତର୍ଭାଵ "riscv.h"
#ଅନ୍ତର୍ଭାଵ "defs.h"

//
// the riscv Platform Level Interrupt Controller (PLIC).
//

ଶୂନ୍ୟ
plicinit(ଶୂନ୍ୟ)
{
  // set desired IRQ priorities non-zero (otherwise disabled).
  *(ଅସ୍ଥିର uint32 *)(PLIC + UART0_IRQ * 4) = 1;
  *(ଅସ୍ଥିର uint32 *)(PLIC + VIRTIO0_IRQ * 4) = 1;
}

ଶୂନ୍ୟ
plicinithart(ଶୂନ୍ୟ)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା hart = cpuid();

  // set enable bits for this hart's S-mode
  // for the uart and virtio disk.
  *(ଅସ୍ଥିର uint32 *)PLIC_SENABLE(hart) =
    (1 << UART0_IRQ) | (1 << VIRTIO0_IRQ);

  // set this hart's S-mode priority threshold to 0.
  *(ଅସ୍ଥିର uint32 *)PLIC_SPRIORITY(hart) = 0;
}

// ask the PLIC what interrupt we should serve.
ପୂର୍ଣ୍ଣସଂଖ୍ୟା
plic_claim(ଶୂନ୍ୟ)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା hart = cpuid();
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା irq = *(ଅସ୍ଥିର uint32 *)PLIC_SCLAIM(hart);
  ଫେରନ୍ତୁ irq;
}

// tell the PLIC we've served this IRQ.
ଶୂନ୍ୟ
plic_complete(ପୂର୍ଣ୍ଣସଂଖ୍ୟା irq)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା hart = cpuid();
  *(ଅସ୍ଥିର uint32 *)PLIC_SCLAIM(hart) = irq;
}
