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
  정수 hart = cpuid();

  // set enable bits for this hart's S-mode
  // for the uart and virtio disk.
  *(volatile uint32 *)PLIC_SENABLE(hart) =
    (1 << UART0_IRQ) | (1 << VIRTIO0_IRQ);

  // set this hart's S-mode priority threshold to 0.
  *(volatile uint32 *)PLIC_SPRIORITY(hart) = 0;
}

// ask the PLIC what interrupt we should serve.
정수
plic_claim(void)
{
  정수 hart = cpuid();
  정수 irq = *(volatile uint32 *)PLIC_SCLAIM(hart);
  반환 irq;
}

// tell the PLIC we've served this IRQ.
void
plic_complete(정수 irq)
{
  정수 hart = cpuid();
  *(volatile uint32 *)PLIC_SCLAIM(hart) = irq;
}
