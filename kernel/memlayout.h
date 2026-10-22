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
#تعریف_کړه UART0     0x10000000L
#تعریف_کړه UART0_IRQ 10

// virtio mmio interface
#تعریف_کړه VIRTIO0     0x10001000
#تعریف_کړه VIRTIO0_IRQ 1

// core-local interrupt controller (CLINT)
#تعریف_کړه CLINT_بنسټ  0x02000000L
#تعریف_کړه CLINT(hart) (CLINT_بنسټ + (hart) * 4)

// qemu puts platform-level interrupt controller (PLIC) here.
#تعریف_کړه PLIC                 0x0c000000L
#تعریف_کړه PLIC_لومړیتوب        (PLIC + 0x0)
#تعریف_کړه PLIC_PENDING         (PLIC + 0x1000)
#تعریف_کړه PLIC_SENABLE(hart)   (PLIC + 0x2080 + (hart) * 0x100)
#تعریف_کړه PLIC_SPRIORITY(hart) (PLIC + 0x201000 + (hart) * 0x2000)
#تعریف_کړه PLIC_SCLAIM(hart)    (PLIC + 0x201004 + (hart) * 0x2000)

// the kernel expects there to be RAM
// for use by the kernel and user pages
// from physical address 0x80000000 to PHYSTOP.
#تعریف_کړه KERNBASE 0x80000000L
#تعریف_کړه PHYSTOP  (KERNBASE + 128 * 1024 * 1024)

// map the trampoline page to the highest address,
// in both user and kernel space.
#تعریف_کړه TRAMPOLINE (MAXVA - PGSIZE)

// map kernel stacks beneath the trampoline,
// each surrounded by invalid guard pages.
#تعریف_کړه KSTACK(p) (TRAMPOLINE - ((p) + 1) * 2 * PGSIZE)

// User memory layout.
// Address zero first:
//متن
//   original data and bss
//   fixed-size stack
//   expandable heap
//   ...
//   TRAPFRAME (p->trapframe, used by the trampoline)
//   TRAMPOLINE (the same page as in the kernel)
#تعریف_کړه TRAPFRAME (TRAMPOLINE - PGSIZE)
