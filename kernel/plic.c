#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "defs.h"

//
// the riscv Platform Level Interrupt Controller (PLIC).
//

void
plicinit(void)
{
  // set desired IRQ priorities non-zero (otherwise disabled).
  *(volatile uint32 *)(PLIC + UART0_IRQ * 4) = 1;
  *(volatile uint32 *)(PLIC + VIRTIO0_IRQ * 4) = 1;
}

void
plicinithart(void)
{
  số_nguyên hart = cpuid();

  // set enable bits for this hart's S-mode
  // for the uart and virtio disk.
  *(volatile uint32 *)PLIC_SENABLE(hart) =
    (1 << UART0_IRQ) | (1 << VIRTIO0_IRQ);

  // set this hart's S-mode priority threshold to 0.
  *(volatile uint32 *)PLIC_SPRIORITY(hart) = 0;
}

// ask the PLIC what interrupt we should serve.
số_nguyên
plic_claim(void)
{
  số_nguyên hart = cpuid();
  số_nguyên irq = *(volatile uint32 *)PLIC_SCLAIM(hart);
  trả_về irq;
}

// tell the PLIC we've served this IRQ.
void
plic_complete(số_nguyên irq)
{
  số_nguyên hart = cpuid();
  *(volatile uint32 *)PLIC_SCLAIM(hart) = irq;
}
