#ਅਨ੍ਤਰ੍ਭਾਵ "types.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "param.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "memlayout.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "riscv.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "defs.h"

//
// the riscv Platform Level Interrupt Controller (PLIC).
//

ਖਾਲੀ
plicinit(ਖਾਲੀ)
{
  // set desired IRQ priorities non-zero (otherwise disabled).
  *(ਅਸਥਿਰ uint32 *)(PLIC + UART0_IRQ * 4) = 1;
  *(ਅਸਥਿਰ uint32 *)(PLIC + VIRTIO0_IRQ * 4) = 1;
}

ਖਾਲੀ
plicinithart(ਖਾਲੀ)
{
  ਪੂਰਨ hart = cpuid();

  // set enable bits for this hart's S-mode
  // for the uart and virtio disk.
  *(ਅਸਥਿਰ uint32 *)PLIC_SENABLE(hart) =
    (1 << UART0_IRQ) | (1 << VIRTIO0_IRQ);

  // set this hart's S-mode priority threshold to 0.
  *(ਅਸਥਿਰ uint32 *)PLIC_SPRIORITY(hart) = 0;
}

// ask the PLIC what interrupt we should serve.
ਪੂਰਨ
plic_claim(ਖਾਲੀ)
{
  ਪੂਰਨ hart = cpuid();
  ਪੂਰਨ irq = *(ਅਸਥਿਰ uint32 *)PLIC_SCLAIM(hart);
  ਵਾਪਸ irq;
}

// tell the PLIC we've served this IRQ.
ਖਾਲੀ
plic_complete(ਪੂਰਨ irq)
{
  ਪੂਰਨ hart = cpuid();
  *(ਅਸਥਿਰ uint32 *)PLIC_SCLAIM(hart) = irq;
}
