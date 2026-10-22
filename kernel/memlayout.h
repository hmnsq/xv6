// Physical memory layout

// qemu -machine virt is set up like this,
// based on qemu's hw/riscv/virt.c:
//
// 00001000 -- boot ROM, provided by qemu
// 02000000 -- CLINT
// 0C000000 -- PLIC
// 10000000 -- uart0
// 10001000 -- virtio disk
// 80000000 -- qemu's boot ROM loads the kernel here,
//             then jumps here.
// unused RAM after 80000000.

// the kernel uses physical memory thus:
// 80000000 -- entry.S, then kernel text and data
// end -- start of kernel page allocation area
// PHYSTOP -- end RAM used by the kernel

// qemu puts UART registers here in physical memory.
#ವ್ಯಾಖ್ಯಾನಿಸು UART0     0x10000000L
#ವ್ಯಾಖ್ಯಾನಿಸು UART0_IRQ 10

// virtio mmio interface
#ವ್ಯಾಖ್ಯಾನಿಸು VIRTIO0     0x10001000
#ವ್ಯಾಖ್ಯಾನಿಸು VIRTIO0_IRQ 1

// core-local interrupt controller (CLINT)
#ವ್ಯಾಖ್ಯಾನಿಸು CLINT_ತಳ  0x02000000L
#ವ್ಯಾಖ್ಯಾನಿಸು CLINT(hart) (CLINT_ತಳ + (hart) * 4)

// qemu puts platform-level interrupt controller (PLIC) here.
#ವ್ಯಾಖ್ಯಾನಿಸು PLIC                 0x0c000000L
#ವ್ಯಾಖ್ಯಾನಿಸು PLIC_ಆದ್ಯತೆ        (PLIC + 0x0)
#ವ್ಯಾಖ್ಯಾನಿಸು PLIC_PENDING         (PLIC + 0x1000)
#ವ್ಯಾಖ್ಯಾನಿಸು PLIC_SENABLE(hart)   (PLIC + 0x2080 + (hart) * 0x100)
#ವ್ಯಾಖ್ಯಾನಿಸು PLIC_SPRIORITY(hart) (PLIC + 0x201000 + (hart) * 0x2000)
#ವ್ಯಾಖ್ಯಾನಿಸು PLIC_SCLAIM(hart)    (PLIC + 0x201004 + (hart) * 0x2000)

// the kernel expects there to be RAM
// for use by the kernel and user pages
// from physical address 0x80000000 to PHYSTOP.
#ವ್ಯಾಖ್ಯಾನಿಸು KERNBASE 0x80000000L
#ವ್ಯಾಖ್ಯಾನಿಸು PHYSTOP  (KERNBASE + 128 * 1024 * 1024)

// map the trampoline page to the highest address,
// in both user and kernel space.
#ವ್ಯಾಖ್ಯಾನಿಸು TRAMPOLINE (MAXVA - PGSIZE)

// map kernel stacks beneath the trampoline,
// each surrounded by invalid guard pages.
#ವ್ಯಾಖ್ಯಾನಿಸು KSTACK(p) (TRAMPOLINE - ((p) + 1) * 2 * PGSIZE)

// User memory layout.
// Address zero first:
//ಪಠ್ಯ
//   original data and bss
//   fixed-size stack
//   expandable heap
//   ...
//   TRAPFRAME (p->trapframe, used by the trampoline)
//   TRAMPOLINE (the same page as in the kernel)
#ವ್ಯಾಖ್ಯಾನಿಸು TRAPFRAME (TRAMPOLINE - PGSIZE)
