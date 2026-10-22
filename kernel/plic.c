#અન્તર્ભાવ "types.h"
#અન્તર્ભાવ "param.h"
#અન્તર્ભાવ "memlayout.h"
#અન્તર્ભાવ "riscv.h"
#અન્તર્ભાવ "defs.h"

//
// the riscv Platform Level Interrupt Controller (PLIC).
//

શૂન્ય
plicinit(શૂન્ય)
{
  // set desired IRQ priorities non-zero (otherwise disabled).
  *(અસ્થિર uint32 *)(PLIC + UART0_IRQ * 4) = 1;
  *(અસ્થિર uint32 *)(PLIC + VIRTIO0_IRQ * 4) = 1;
}

શૂન્ય
plicinithart(શૂન્ય)
{
  પૂર્ણાંક hart = cpuid();

  // set enable bits for this hart's S-mode
  // for the uart and virtio disk.
  *(અસ્થિર uint32 *)PLIC_SENABLE(hart) =
    (1 << UART0_IRQ) | (1 << VIRTIO0_IRQ);

  // set this hart's S-mode priority threshold to 0.
  *(અસ્થિર uint32 *)PLIC_SPRIORITY(hart) = 0;
}

// ask the PLIC what interrupt we should serve.
પૂર્ણાંક
plic_claim(શૂન્ય)
{
  પૂર્ણાંક hart = cpuid();
  પૂર્ણાંક irq = *(અસ્થિર uint32 *)PLIC_SCLAIM(hart);
  પાછા irq;
}

// tell the PLIC we've served this IRQ.
શૂન્ય
plic_complete(પૂર્ણાંક irq)
{
  પૂર્ણાંક hart = cpuid();
  *(અસ્થિર uint32 *)PLIC_SCLAIM(hart) = irq;
}
