#ضمن "types.h"
#ضمن "param.h"
#ضمن "memlayout.h"
#ضمن "riscv.h"
#ضمن "defs.h"

//
// the riscv Platform Level Interrupt Controller (PLIC).
//

فراغ
plicinit(فراغ)
{
  // set desired IRQ priorities non-zero (otherwise disabled).
  *(متطاير uint32 *)(PLIC + UART0_IRQ * 4) = 1;
  *(متطاير uint32 *)(PLIC + VIRTIO0_IRQ * 4) = 1;
}

فراغ
plicinithart(فراغ)
{
  صحيح hart = cpuid();

  // set enable bits for this hart's S-mode
  // for the uart and virtio disk.
  *(متطاير uint32 *)PLIC_SENABLE(hart) =
    (1 << UART0_IRQ) | (1 << VIRTIO0_IRQ);

  // set this hart's S-mode priority threshold to 0.
  *(متطاير uint32 *)PLIC_SPRIORITY(hart) = 0;
}

// ask the PLIC what interrupt we should serve.
صحيح
plic_claim(فراغ)
{
  صحيح hart = cpuid();
  صحيح irq = *(متطاير uint32 *)PLIC_SCLAIM(hart);
  ܦܢܐ irq;
}

// tell the PLIC we've served this IRQ.
فراغ
plic_complete(صحيح irq)
{
  صحيح hart = cpuid();
  *(متطاير uint32 *)PLIC_SCLAIM(hart) = irq;
}
