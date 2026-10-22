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
  *(ca_ncala uint32 *)(PLIC + UART0_IRQ * 4) = 1;
  *(ca_ncala uint32 *)(PLIC + VIRTIO0_IRQ * 4) = 1;
}

void
plicinithart(void)
{
  ຈຳນວນເຕັມ hart = cpuid();

  // set enable bits for this hart's S-mode
  // for the uart and virtio disk.
  *(ca_ncala uint32 *)PLIC_SENABLE(hart) =
    (1 << UART0_IRQ) | (1 << VIRTIO0_IRQ);

  // set this hart's S-mode priority threshold to 0.
  *(ca_ncala uint32 *)PLIC_SPRIORITY(hart) = 0;
}

// ask the PLIC what interrupt we should serve.
ຈຳນວນເຕັມ
plic_claim(void)
{
  ຈຳນວນເຕັມ hart = cpuid();
  ຈຳນວນເຕັມ irq = *(ca_ncala uint32 *)PLIC_SCLAIM(hart);
  ສົ່ງຄືນ irq;
}

// tell the PLIC we've served this IRQ.
void
plic_complete(ຈຳນວນເຕັມ irq)
{
  ຈຳນວນເຕັມ hart = cpuid();
  *(ca_ncala uint32 *)PLIC_SCLAIM(hart) = irq;
}
