#অন্তর্ভাব "types.h"
#অন্তর্ভাব "param.h"
#অন্তর্ভাব "memlayout.h"
#অন্তর্ভাব "riscv.h"
#অন্তর্ভাব "defs.h"

//
// the riscv Platform Level Interrupt Controller (PLIC).
//

শূন্য
plicinit(শূন্য)
{
  // set desired IRQ priorities non-zero (otherwise disabled).
  *(অস্থির uint32 *)(PLIC + UART0_IRQ * 4) = 1;
  *(অস্থির uint32 *)(PLIC + VIRTIO0_IRQ * 4) = 1;
}

শূন্য
plicinithart(শূন্য)
{
  পূর্ণসংখ্যা hart = cpuid();

  // set enable bits for this hart's S-mode
  // for the uart and virtio disk.
  *(অস্থির uint32 *)PLIC_SENABLE(hart) =
    (1 << UART0_IRQ) | (1 << VIRTIO0_IRQ);

  // set this hart's S-mode priority threshold to 0.
  *(অস্থির uint32 *)PLIC_SPRIORITY(hart) = 0;
}

// ask the PLIC what interrupt we should serve.
পূর্ণসংখ্যা
plic_claim(শূন্য)
{
  পূর্ণসংখ্যা hart = cpuid();
  পূর্ণসংখ্যা irq = *(অস্থির uint32 *)PLIC_SCLAIM(hart);
  ফেরত irq;
}

// tell the PLIC we've served this IRQ.
শূন্য
plic_complete(পূর্ণসংখ্যা irq)
{
  পূর্ণসংখ্যা hart = cpuid();
  *(অস্থির uint32 *)PLIC_SCLAIM(hart) = irq;
}
