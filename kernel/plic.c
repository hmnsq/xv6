#شامل "types.h"
#شامل "param.h"
#شامل "memlayout.h"
#شامل "riscv.h"
#شامل "defs.h"

//
// the riscv Platform Level Interrupt Controller (PLIC).
//

خالی
plicinit(خالی)
{
  // set desired IRQ priorities non-zero (otherwise disabled).
  *(متلون uint32 *)(PLIC + UART0_IRQ * 4) = 1;
  *(متلون uint32 *)(PLIC + VIRTIO0_IRQ * 4) = 1;
}

خالی
plicinithart(خالی)
{
  صحیح hart = cpuid();

  // set enable bits for this hart's S-mode
  // for the uart and virtio disk.
  *(متلون uint32 *)PLIC_SENABLE(hart) =
    (1 << UART0_IRQ) | (1 << VIRTIO0_IRQ);

  // set this hart's S-mode priority threshold to 0.
  *(متلون uint32 *)PLIC_SPRIORITY(hart) = 0;
}

// ask the PLIC what interrupt we should serve.
صحیح
plic_claim(خالی)
{
  صحیح hart = cpuid();
  صحیح irq = *(متلون uint32 *)PLIC_SCLAIM(hart);
  واپس irq;
}

// tell the PLIC we've served this IRQ.
خالی
plic_complete(صحیح irq)
{
  صحیح hart = cpuid();
  *(متلون uint32 *)PLIC_SCLAIM(hart) = irq;
}
