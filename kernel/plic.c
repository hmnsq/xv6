#अन्तर्भाव "types.h"
#अन्तर्भाव "param.h"
#अन्तर्भाव "memlayout.h"
#अन्तर्भाव "riscv.h"
#अन्तर्भाव "defs.h"

//
// the riscv Platform Level Interrupt Controller (PLIC).
//

शून्य
plicinit(शून्य)
{
  // set desired IRQ priorities non-zero (otherwise disabled).
  *(चञ्चल uint32 *)(PLIC + UART0_IRQ * 4) = 1;
  *(चञ्चल uint32 *)(PLIC + VIRTIO0_IRQ * 4) = 1;
}

शून्य
plicinithart(शून्य)
{
  पूर्णाङ्क hart = cpuid();

  // set enable bits for this hart's S-mode
  // for the uart and virtio disk.
  *(चञ्चल uint32 *)PLIC_SENABLE(hart) =
    (1 << UART0_IRQ) | (1 << VIRTIO0_IRQ);

  // set this hart's S-mode priority threshold to 0.
  *(चञ्चल uint32 *)PLIC_SPRIORITY(hart) = 0;
}

// ask the PLIC what interrupt we should serve.
पूर्णाङ्क
plic_claim(शून्य)
{
  पूर्णाङ्क hart = cpuid();
  पूर्णाङ्क irq = *(चञ्चल uint32 *)PLIC_SCLAIM(hart);
  पटिनिवत्त irq;
}

// tell the PLIC we've served this IRQ.
शून्य
plic_complete(पूर्णाङ्क irq)
{
  पूर्णाङ्क hart = cpuid();
  *(चञ्चल uint32 *)PLIC_SCLAIM(hart) = irq;
}
