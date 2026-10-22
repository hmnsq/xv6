#ಅನ್ತರ್ಭಾವ "types.h"
#ಅನ್ತರ್ಭಾವ "param.h"
#ಅನ್ತರ್ಭಾವ "memlayout.h"
#ಅನ್ತರ್ಭಾವ "riscv.h"
#ಅನ್ತರ್ಭಾವ "defs.h"

//
// the riscv Platform Level Interrupt Controller (PLIC).
//

ಶೂನ್ಯ
plicinit(ಶೂನ್ಯ)
{
  // set desired IRQ priorities non-zero (otherwise disabled).
  *(ಅಸ್ಥಿರ uint32 *)(PLIC + UART0_IRQ * 4) = 1;
  *(ಅಸ್ಥಿರ uint32 *)(PLIC + VIRTIO0_IRQ * 4) = 1;
}

ಶೂನ್ಯ
plicinithart(ಶೂನ್ಯ)
{
  ಪೂರ್ಣಾಂಕ hart = cpuid();

  // set enable bits for this hart's S-mode
  // for the uart and virtio disk.
  *(ಅಸ್ಥಿರ uint32 *)PLIC_SENABLE(hart) =
    (1 << UART0_IRQ) | (1 << VIRTIO0_IRQ);

  // set this hart's S-mode priority threshold to 0.
  *(ಅಸ್ಥಿರ uint32 *)PLIC_SPRIORITY(hart) = 0;
}

// ask the PLIC what interrupt we should serve.
ಪೂರ್ಣಾಂಕ
plic_claim(ಶೂನ್ಯ)
{
  ಪೂರ್ಣಾಂಕ hart = cpuid();
  ಪೂರ್ಣಾಂಕ irq = *(ಅಸ್ಥಿರ uint32 *)PLIC_SCLAIM(hart);
  ಹಿಂತಿರುಗಿ irq;
}

// tell the PLIC we've served this IRQ.
ಶೂನ್ಯ
plic_complete(ಪೂರ್ಣಾಂಕ irq)
{
  ಪೂರ್ಣಾಂಕ hart = cpuid();
  *(ಅಸ್ಥಿರ uint32 *)PLIC_SCLAIM(hart) = irq;
}
