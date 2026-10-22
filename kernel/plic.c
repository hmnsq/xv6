#شامل "types.h"
#شامل "param.h"
#شامل "memlayout.h"
#شامل "riscv.h"
#شامل "defs.h"

//
// the riscv Platform Level Interrupt Controller (PLIC).
//

تهی
plicinit(تهی)
{
  // set desired IRQ priorities non-zero (otherwise disabled).
  *(فرار uint32 *)(PLIC + UART0_IRQ * 4) = 1;
  *(فرار uint32 *)(PLIC + VIRTIO0_IRQ * 4) = 1;
}

تهی
plicinithart(تهی)
{
  صحیح hart = cpuid();

  // set enable bits for this hart's S-mode
  // for the uart and virtio disk.
  *(فرار uint32 *)PLIC_SENABLE(hart) =
    (1 << UART0_IRQ) | (1 << VIRTIO0_IRQ);

  // set this hart's S-mode priority threshold to 0.
  *(فرار uint32 *)PLIC_SPRIORITY(hart) = 0;
}

// ask the PLIC what interrupt we should serve.
صحیح
plic_claim(تهی)
{
  صحیح hart = cpuid();
  صحیح irq = *(فرار uint32 *)PLIC_SCLAIM(hart);
  بازگشت irq;
}

// tell the PLIC we've served this IRQ.
تهی
plic_complete(صحیح irq)
{
  صحیح hart = cpuid();
  *(فرار uint32 *)PLIC_SCLAIM(hart) = irq;
}
