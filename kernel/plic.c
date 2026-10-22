#அந்தர்பாவ "types.h"
#அந்தர்பாவ "param.h"
#அந்தர்பாவ "memlayout.h"
#அந்தர்பாவ "riscv.h"
#அந்தர்பாவ "defs.h"

//
// the riscv Platform Level Interrupt Controller (PLIC).
//

வெறுமை
plicinit(வெறுமை)
{
  // set desired IRQ priorities non-zero (otherwise disabled).
  *(நிலையற்ற uint32 *)(PLIC + UART0_IRQ * 4) = 1;
  *(நிலையற்ற uint32 *)(PLIC + VIRTIO0_IRQ * 4) = 1;
}

வெறுமை
plicinithart(வெறுமை)
{
  முழு hart = cpuid();

  // set enable bits for this hart's S-mode
  // for the uart and virtio disk.
  *(நிலையற்ற uint32 *)PLIC_SENABLE(hart) =
    (1 << UART0_IRQ) | (1 << VIRTIO0_IRQ);

  // set this hart's S-mode priority threshold to 0.
  *(நிலையற்ற uint32 *)PLIC_SPRIORITY(hart) = 0;
}

// ask the PLIC what interrupt we should serve.
முழு
plic_claim(வெறுமை)
{
  முழு hart = cpuid();
  முழு irq = *(நிலையற்ற uint32 *)PLIC_SCLAIM(hart);
  திரும்பு irq;
}

// tell the PLIC we've served this IRQ.
வெறுமை
plic_complete(முழு irq)
{
  முழு hart = cpuid();
  *(நிலையற்ற uint32 *)PLIC_SCLAIM(hart) = irq;
}
