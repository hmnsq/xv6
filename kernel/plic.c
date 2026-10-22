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
  整数 hart = cpuid();

  // set enable bits for this hart's S-mode
  // for the uart and virtio disk.
  *(volatile uint32 *)PLIC_SENABLE(hart) =
    (1 << UART0_IRQ) | (1 << VIRTIO0_IRQ);

  // set this hart's S-mode priority threshold to 0.
  *(volatile uint32 *)PLIC_SPRIORITY(hart) = 0;
}

// ask the PLIC what interrupt we should serve.
整数
plic_claim(void)
{
  整数 hart = cpuid();
  整数 irq = *(volatile uint32 *)PLIC_SCLAIM(hart);
  戻る irq;
}

// tell the PLIC we've served this IRQ.
void
plic_complete(整数 irq)
{
  整数 hart = cpuid();
  *(volatile uint32 *)PLIC_SCLAIM(hart) = irq;
}
