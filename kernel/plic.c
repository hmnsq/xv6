#అన్తర్భావ "types.h"
#అన్తర్భావ "param.h"
#అన్తర్భావ "memlayout.h"
#అన్తర్భావ "riscv.h"
#అన్తర్భావ "defs.h"

//
// the riscv Platform Level Interrupt Controller (PLIC).
//

శూన్య
plicinit(శూన్య)
{
  // set desired IRQ priorities non-zero (otherwise disabled).
  *(అస్థిర uint32 *)(PLIC + UART0_IRQ * 4) = 1;
  *(అస్థిర uint32 *)(PLIC + VIRTIO0_IRQ * 4) = 1;
}

శూన్య
plicinithart(శూన్య)
{
  పూర్ణాంకం hart = cpuid();

  // set enable bits for this hart's S-mode
  // for the uart and virtio disk.
  *(అస్థిర uint32 *)PLIC_SENABLE(hart) =
    (1 << UART0_IRQ) | (1 << VIRTIO0_IRQ);

  // set this hart's S-mode priority threshold to 0.
  *(అస్థిర uint32 *)PLIC_SPRIORITY(hart) = 0;
}

// ask the PLIC what interrupt we should serve.
పూర్ణాంకం
plic_claim(శూన్య)
{
  పూర్ణాంకం hart = cpuid();
  పూర్ణాంకం irq = *(అస్థిర uint32 *)PLIC_SCLAIM(hart);
  తిరిగి irq;
}

// tell the PLIC we've served this IRQ.
శూన్య
plic_complete(పూర్ణాంకం irq)
{
  పూర్ణాంకం hart = cpuid();
  *(అస్థిర uint32 *)PLIC_SCLAIM(hart) = irq;
}
