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
  *(চঞ্চল uint32 *)(PLIC + UART0_IRQ * 4) = 1;
  *(চঞ্চল uint32 *)(PLIC + VIRTIO0_IRQ * 4) = 1;
}

শূন্য
plicinithart(শূন্য)
{
  পূর্ণাঙ্ক hart = cpuid();

  // set enable bits for this hart's S-mode
  // for the uart and virtio disk.
  *(চঞ্চল uint32 *)PLIC_SENABLE(hart) =
    (1 << UART0_IRQ) | (1 << VIRTIO0_IRQ);

  // set this hart's S-mode priority threshold to 0.
  *(চঞ্চল uint32 *)PLIC_SPRIORITY(hart) = 0;
}

// ask the PLIC what interrupt we should serve.
পূর্ণাঙ্ক
plic_claim(শূন্য)
{
  পূর্ণাঙ্ক hart = cpuid();
  পূর্ণাঙ্ক irq = *(চঞ্চল uint32 *)PLIC_SCLAIM(hart);
  প্রত্যাগম irq;
}

// tell the PLIC we've served this IRQ.
শূন্য
plic_complete(পূর্ণাঙ্ক irq)
{
  পূর্ণাঙ্ক hart = cpuid();
  *(চঞ্চল uint32 *)PLIC_SCLAIM(hart) = irq;
}
