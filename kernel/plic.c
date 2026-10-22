#समावेश "types.h"
#समावेश "param.h"
#समावेश "memlayout.h"
#समावेश "riscv.h"
#समावेश "defs.h"

//
// the riscv Platform Level Interrupt Controller (PLIC).
//

व्योम
plicinit(व्योम)
{
  // set desired IRQ priorities non-zero (otherwise disabled).
  *(अस्थिर uint32 *)(PLIC + UART0_IRQ * 4) = 1;
  *(अस्थिर uint32 *)(PLIC + VIRTIO0_IRQ * 4) = 1;
}

व्योम
plicinithart(व्योम)
{
  पूर्णांक hart = cpuid();

  // set enable bits for this hart's S-mode
  // for the uart and virtio disk.
  *(अस्थिर uint32 *)PLIC_SENABLE(hart) =
    (1 << UART0_IRQ) | (1 << VIRTIO0_IRQ);

  // set this hart's S-mode priority threshold to 0.
  *(अस्थिर uint32 *)PLIC_SPRIORITY(hart) = 0;
}

// ask the PLIC what interrupt we should serve.
पूर्णांक
plic_claim(व्योम)
{
  पूर्णांक hart = cpuid();
  पूर्णांक irq = *(अस्थिर uint32 *)PLIC_SCLAIM(hart);
  वापस irq;
}

// tell the PLIC we've served this IRQ.
व्योम
plic_complete(पूर्णांक irq)
{
  पूर्णांक hart = cpuid();
  *(अस्थिर uint32 *)PLIC_SCLAIM(hart) = irq;
}
