#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ __ASSEMBLER__

// which hart (core) is this?
ಸ್ಥಿರ ಸಾಲೊಳಗೆ uint64
r_mhartid()
{
  uint64 x;
  asm ಅಸ್ಥಿರ("csrr %0, mhartid" : "=r"(x));
  ಹಿಂತಿರುಗಿ x;
}

// Machine Status Register, mstatus

#ವ್ಯಾಖ್ಯಾನಿಸು MSTATUS_MPP_ಮುಖವಾಡ (3L << 11) // previous mode.
#ವ್ಯಾಖ್ಯಾನಿಸು MSTATUS_MPP_M    (3L << 11)
#ವ್ಯಾಖ್ಯಾನಿಸು MSTATUS_MPP_S    (1L << 11)
#ವ್ಯಾಖ್ಯಾನಿಸು MSTATUS_MPP_U    (0L << 11)

ಸ್ಥಿರ ಸಾಲೊಳಗೆ uint64
r_mstatus()
{
  uint64 x;
  asm ಅಸ್ಥಿರ("csrr %0, mstatus" : "=r"(x));
  ಹಿಂತಿರುಗಿ x;
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ
w_mstatus(uint64 x)
{
  asm ಅಸ್ಥಿರ("csrw mstatus, %0" : : "r"(x));
}

// machine exception program counter, holds the
// instruction address to which a return from
// exception will go.
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ
w_mepc(uint64 x)
{
  asm ಅಸ್ಥಿರ("csrw mepc, %0" : : "r"(x));
}

// Supervisor Status Register, sstatus

#ವ್ಯಾಖ್ಯಾನಿಸು SSTATUS_SPP  (1L << 8) // Previous mode, 1=Supervisor, 0=User
#ವ್ಯಾಖ್ಯಾನಿಸು SSTATUS_SPIE (1L << 5) // Supervisor Previous Interrupt Enable
#ವ್ಯಾಖ್ಯಾನಿಸು SSTATUS_UPIE (1L << 4) // User Previous Interrupt Enable
#ವ್ಯಾಖ್ಯಾನಿಸು SSTATUS_SIE  (1L << 1) // Supervisor Interrupt Enable
#ವ್ಯಾಖ್ಯಾನಿಸು SSTATUS_UIE  (1L << 0) // User Interrupt Enable

ಸ್ಥಿರ ಸಾಲೊಳಗೆ uint64
r_sstatus()
{
  uint64 x;
  asm ಅಸ್ಥಿರ("csrr %0, sstatus" : "=r"(x));
  ಹಿಂತಿರುಗಿ x;
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ
w_sstatus(uint64 x)
{
  asm ಅಸ್ಥಿರ("csrw sstatus, %0" : : "r"(x));
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ
s_sstatus(uint64 x)
{
  __asm__ __volatile__("csrs sstatus, %0" ::"rK"(x) : "memory");
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ
c_sstatus(uint64 x)
{
  __asm__ __volatile__("csrc sstatus, %0" ::"rK"(x) : "memory");
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ uint64
rc_sstatus(uint64 x)
{
  __asm__ __volatile__("csrrc %0, sstatus, %1" : "=r"(x) : "rK"(x) : "memory");
  ಹಿಂತಿರುಗಿ x;
}

// Supervisor Interrupt Pending
ಸ್ಥಿರ ಸಾಲೊಳಗೆ uint64
r_sip()
{
  uint64 x;
  asm ಅಸ್ಥಿರ("csrr %0, sip" : "=r"(x));
  ಹಿಂತಿರುಗಿ x;
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ
w_sip(uint64 x)
{
  asm ಅಸ್ಥಿರ("csrw sip, %0" : : "r"(x));
}

// Supervisor Interrupt Enable
#ವ್ಯಾಖ್ಯಾನಿಸು SIE_SEIE (1L << 9) // external
#ವ್ಯಾಖ್ಯಾನಿಸು SIE_STIE (1L << 5) // timer
ಸ್ಥಿರ ಸಾಲೊಳಗೆ uint64
r_sie()
{
  uint64 x;
  asm ಅಸ್ಥಿರ("csrr %0, sie" : "=r"(x));
  ಹಿಂತಿರುಗಿ x;
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ
w_sie(uint64 x)
{
  asm ಅಸ್ಥಿರ("csrw sie, %0" : : "r"(x));
}

// Machine-mode Interrupt Enable
#ವ್ಯಾಖ್ಯಾನಿಸು MIE_STIE (1L << 5) // supervisor timer
ಸ್ಥಿರ ಸಾಲೊಳಗೆ uint64
r_mie()
{
  uint64 x;
  asm ಅಸ್ಥಿರ("csrr %0, mie" : "=r"(x));
  ಹಿಂತಿರುಗಿ x;
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ
w_mie(uint64 x)
{
  asm ಅಸ್ಥಿರ("csrw mie, %0" : : "r"(x));
}

// supervisor exception program counter, holds the
// instruction address to which a return from
// exception will go.
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ
w_sepc(uint64 x)
{
  asm ಅಸ್ಥಿರ("csrw sepc, %0" : : "r"(x));
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ uint64
r_sepc()
{
  uint64 x;
  asm ಅಸ್ಥಿರ("csrr %0, sepc" : "=r"(x));
  ಹಿಂತಿರುಗಿ x;
}

// Machine Exception Delegation
ಸ್ಥಿರ ಸಾಲೊಳಗೆ uint64
r_medeleg()
{
  uint64 x;
  asm ಅಸ್ಥಿರ("csrr %0, medeleg" : "=r"(x));
  ಹಿಂತಿರುಗಿ x;
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ
w_medeleg(uint64 x)
{
  asm ಅಸ್ಥಿರ("csrw medeleg, %0" : : "r"(x));
}

// Machine Interrupt Delegation
ಸ್ಥಿರ ಸಾಲೊಳಗೆ uint64
r_mideleg()
{
  uint64 x;
  asm ಅಸ್ಥಿರ("csrr %0, mideleg" : "=r"(x));
  ಹಿಂತಿರುಗಿ x;
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ
w_mideleg(uint64 x)
{
  asm ಅಸ್ಥಿರ("csrw mideleg, %0" : : "r"(x));
}

// Supervisor Trap-Vector Base Address
// low two bits are mode.
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ
w_stvec(uint64 x)
{
  asm ಅಸ್ಥಿರ("csrw stvec, %0" : : "r"(x));
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ uint64
r_stvec()
{
  uint64 x;
  asm ಅಸ್ಥಿರ("csrr %0, stvec" : "=r"(x));
  ಹಿಂತಿರುಗಿ x;
}

// Supervisor Timer Comparison Register
ಸ್ಥಿರ ಸಾಲೊಳಗೆ uint64
r_stimecmp()
{
  uint64 x;
  // asm volatile("csrr %0, stimecmp" : "=r" (x) );
  asm ಅಸ್ಥಿರ("csrr %0, 0x14d" : "=r"(x));
  ಹಿಂತಿರುಗಿ x;
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ
w_stimecmp(uint64 x)
{
  // asm volatile("csrw stimecmp, %0" : : "r" (x));
  asm ಅಸ್ಥಿರ("csrw 0x14d, %0" : : "r"(x));
}

// Machine Environment Configuration Register

#ವ್ಯಾಖ್ಯಾನಿಸು MENVCFG_STCE (1L << 63)
#ವ್ಯಾಖ್ಯಾನಿಸು MENVCFG_ADUE (1L << 61)

ಸ್ಥಿರ ಸಾಲೊಳಗೆ uint64
r_menvcfg()
{
  uint64 x;
  // asm volatile("csrr %0, menvcfg" : "=r" (x) );
  asm ಅಸ್ಥಿರ("csrr %0, 0x30a" : "=r"(x));
  ಹಿಂತಿರುಗಿ x;
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ
w_menvcfg(uint64 x)
{
  // asm volatile("csrw menvcfg, %0" : : "r" (x));
  asm ಅಸ್ಥಿರ("csrw 0x30a, %0" : : "r"(x));
}

// Physical Memory Protection
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ
w_pmpcfg0(uint64 x)
{
  asm ಅಸ್ಥಿರ("csrw pmpcfg0, %0" : : "r"(x));
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ
w_pmpaddr0(uint64 x)
{
  asm ಅಸ್ಥಿರ("csrw pmpaddr0, %0" : : "r"(x));
}

// use riscv's sv39 page table scheme.
#ವ್ಯಾಖ್ಯಾನಿಸು SATP_SV39 (8L << 60)

#ವ್ಯಾಖ್ಯಾನಿಸು MAKE_SATP(pagetable) (SATP_SV39 | (((uint64)pagetable) >> 12))

// supervisor address translation and protection;
// holds the address of the page table.
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ
w_satp(uint64 x)
{
  asm ಅಸ್ಥಿರ("csrw satp, %0" : : "r"(x));
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ uint64
r_satp()
{
  uint64 x;
  asm ಅಸ್ಥಿರ("csrr %0, satp" : "=r"(x));
  ಹಿಂತಿರುಗಿ x;
}

// Supervisor Trap Cause
ಸ್ಥಿರ ಸಾಲೊಳಗೆ uint64
r_scause()
{
  uint64 x;
  asm ಅಸ್ಥಿರ("csrr %0, scause" : "=r"(x));
  ಹಿಂತಿರುಗಿ x;
}

// Supervisor Trap Value
ಸ್ಥಿರ ಸಾಲೊಳಗೆ uint64
r_stval()
{
  uint64 x;
  asm ಅಸ್ಥಿರ("csrr %0, stval" : "=r"(x));
  ಹಿಂತಿರುಗಿ x;
}

// Machine-mode Counter-Enable
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ
w_mcounteren(uint64 x)
{
  asm ಅಸ್ಥಿರ("csrw mcounteren, %0" : : "r"(x));
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ uint64
r_mcounteren()
{
  uint64 x;
  asm ಅಸ್ಥಿರ("csrr %0, mcounteren" : "=r"(x));
  ಹಿಂತಿರುಗಿ x;
}

// machine-mode cycle counter
ಸ್ಥಿರ ಸಾಲೊಳಗೆ uint64
r_ಸಮಯ()
{
  uint64 x;
  asm ಅಸ್ಥಿರ("csrr %0, time" : "=r"(x));
  ಹಿಂತಿರುಗಿ x;
}

// enable device interrupts
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ
intr_on()
{
  s_sstatus(SSTATUS_SIE);
}

// disable device interrupts
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ
intr_off()
{
  c_sstatus(SSTATUS_SIE);
}

// are device interrupts enabled?
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ
intr_get()
{
  uint64 x = r_sstatus();
  ಹಿಂತಿರುಗಿ (x & SSTATUS_SIE) != 0;
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ uint64
r_sp()
{
  uint64 x;
  asm ಅಸ್ಥಿರ("mv %0, sp" : "=r"(x));
  ಹಿಂತಿರುಗಿ x;
}

// read and write tp, the thread pointer, which xv6 uses to hold
// this core's hartid (core number), the index into cpus[].
ಸ್ಥಿರ ಸಾಲೊಳಗೆ uint64
r_tp()
{
  uint64 x;
  asm ಅಸ್ಥಿರ("mv %0, tp" : "=r"(x));
  ಹಿಂತಿರುಗಿ x;
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ
w_tp(uint64 x)
{
  asm ಅಸ್ಥಿರ("mv tp, %0" : : "r"(x));
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ uint64
r_ra()
{
  uint64 x;
  asm ಅಸ್ಥಿರ("mv %0, ra" : "=r"(x));
  ಹಿಂತಿರುಗಿ x;
}

// flush the TLB.
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ
sfence_vma()
{
  // the zero, zero means flush all TLB entries.
  asm ಅಸ್ಥಿರ("sfence.vma zero, zero" ::: "memory");
}

// fence for memory-mapped IO
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ
io_fence()
{
  asm ಅಸ್ಥಿರ("fence iorw, iorw" ::: "memory");
}

// fence for icache
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ
icache_fence()
{
  asm ಅಸ್ಥಿರ("fence.i" ::: "memory");
}

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ uint64 pte_t;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ uint64 *pagetable_t; // 512 PTEs

#ಆದರೆ_ಅಂತ್ಯ // __ASSEMBLER__

#ವ್ಯಾಖ್ಯಾನಿಸು PGSIZE  4096 // bytes per page
#ವ್ಯಾಖ್ಯಾನಿಸು PGSHIFT 12   // bits of offset within a page

#ವ್ಯಾಖ್ಯಾನಿಸು PGROUNDUP(sz)  (((sz) + PGSIZE - 1) & ~(PGSIZE - 1))
#ವ್ಯಾಖ್ಯಾನಿಸು PGROUNDDOWN(a) (((a)) & ~(PGSIZE - 1))

#ವ್ಯಾಖ್ಯಾನಿಸು PTE_V (1L << 0) // valid
#ವ್ಯಾಖ್ಯಾನಿಸು PTE_R (1L << 1)
#ವ್ಯಾಖ್ಯಾನಿಸು PTE_W (1L << 2)
#ವ್ಯಾಖ್ಯಾನಿಸು PTE_X (1L << 3)
#ವ್ಯಾಖ್ಯಾನಿಸು PTE_U (1L << 4) // user can access

// shift a physical address to the right place for a PTE.
#ವ್ಯಾಖ್ಯಾನಿಸು PA2PTE(pa) ((((uint64)pa) >> 12) << 10)

#ವ್ಯಾಖ್ಯಾನಿಸು PTE2PA(pte) (((pte) >> 10) << 12)

#ವ್ಯಾಖ್ಯಾನಿಸು PTE_FLAGS(pte) ((pte) & 0x3FF)

// extract the three 9-bit page table indices from a virtual address.
#ವ್ಯಾಖ್ಯಾನಿಸು PXMASK         0x1FF // 9 bits
#ವ್ಯಾಖ್ಯಾನಿಸು PXSHIFT(level) (PGSHIFT + (9 * (level)))
#ವ್ಯಾಖ್ಯಾನಿಸು PX(level, va)  ((((uint64)(va)) >> PXSHIFT(level)) & PXMASK)

// one beyond the highest possible virtual address.
// MAXVA is actually one bit less than the max allowed by
// Sv39, to avoid having to sign-extend virtual addresses
// that have the high bit set.
#ವ್ಯಾಖ್ಯಾನಿಸು MAXVA (1L << (9 + 9 + 9 + 12 - 1))
