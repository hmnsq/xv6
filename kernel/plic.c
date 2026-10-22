#അന്തര്ഭാവ "types.h"
#അന്തര്ഭാവ "param.h"
#അന്തര്ഭാവ "memlayout.h"
#അന്തര്ഭാവ "riscv.h"
#അന്തര്ഭാവ "defs.h"

//
// the riscv Platform Level Interrupt Controller (PLIC).
//

ശൂന്യം
plicinit(ശൂന്യം)
{
  // set desired IRQ priorities non-zero (otherwise disabled).
  *(അസ്ഥിര uint32 *)(PLIC + UART0_IRQ * 4) = 1;
  *(അസ്ഥിര uint32 *)(PLIC + VIRTIO0_IRQ * 4) = 1;
}

ശൂന്യം
plicinithart(ശൂന്യം)
{
  പൂർണ്ണസംഖ്യ hart = cpuid();

  // set enable bits for this hart's S-mode
  // for the uart and virtio disk.
  *(അസ്ഥിര uint32 *)PLIC_SENABLE(hart) =
    (1 << UART0_IRQ) | (1 << VIRTIO0_IRQ);

  // set this hart's S-mode priority threshold to 0.
  *(അസ്ഥിര uint32 *)PLIC_SPRIORITY(hart) = 0;
}

// ask the PLIC what interrupt we should serve.
പൂർണ്ണസംഖ്യ
plic_claim(ശൂന്യം)
{
  പൂർണ്ണസംഖ്യ hart = cpuid();
  പൂർണ്ണസംഖ്യ irq = *(അസ്ഥിര uint32 *)PLIC_SCLAIM(hart);
  തിരികെ irq;
}

// tell the PLIC we've served this IRQ.
ശൂന്യം
plic_complete(പൂർണ്ണസംഖ്യ irq)
{
  പൂർണ്ണസംഖ്യ hart = cpuid();
  *(അസ്ഥിര uint32 *)PLIC_SCLAIM(hart) = irq;
}
