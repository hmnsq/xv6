#ifndef __ASSEMBLER__

// which hart (core) is this?
நிலையான உள்வரி uint64
r_mhartid()
{
  uint64 x;
  asm நிலையற்ற("csrr %0, mhartid" : "=r"(x));
  திரும்பு x;
}

// Machine Status Register, mstatus

#define MSTATUS_MPP_MASK (3L << 11) // previous mode.
#define MSTATUS_MPP_M    (3L << 11)
#define MSTATUS_MPP_S    (1L << 11)
#define MSTATUS_MPP_U    (0L << 11)

நிலையான உள்வரி uint64
r_mstatus()
{
  uint64 x;
  asm நிலையற்ற("csrr %0, mstatus" : "=r"(x));
  திரும்பு x;
}

நிலையான உள்வரி வெறுமை
w_mstatus(uint64 x)
{
  asm நிலையற்ற("csrw mstatus, %0" : : "r"(x));
}

// machine exception program counter, holds the
// instruction address to which a return from
// exception will go.
நிலையான உள்வரி வெறுமை
w_mepc(uint64 x)
{
  asm நிலையற்ற("csrw mepc, %0" : : "r"(x));
}

// Supervisor Status Register, sstatus

#define SSTATUS_SPP  (1L << 8) // Previous mode, 1=Supervisor, 0=User
#define SSTATUS_SPIE (1L << 5) // Supervisor Previous Interrupt Enable
#define SSTATUS_UPIE (1L << 4) // User Previous Interrupt Enable
#define SSTATUS_SIE  (1L << 1) // Supervisor Interrupt Enable
#define SSTATUS_UIE  (1L << 0) // User Interrupt Enable

நிலையான உள்வரி uint64
r_sstatus()
{
  uint64 x;
  asm நிலையற்ற("csrr %0, sstatus" : "=r"(x));
  திரும்பு x;
}

நிலையான உள்வரி வெறுமை
w_sstatus(uint64 x)
{
  asm நிலையற்ற("csrw sstatus, %0" : : "r"(x));
}

நிலையான உள்வரி வெறுமை
s_sstatus(uint64 x)
{
  __asm__ __volatile__("csrs sstatus, %0" ::"rK"(x) : "memory");
}

நிலையான உள்வரி வெறுமை
c_sstatus(uint64 x)
{
  __asm__ __volatile__("csrc sstatus, %0" ::"rK"(x) : "memory");
}

நிலையான உள்வரி uint64
rc_sstatus(uint64 x)
{
  __asm__ __volatile__("csrrc %0, sstatus, %1" : "=r"(x) : "rK"(x) : "memory");
  திரும்பு x;
}

// Supervisor Interrupt Pending
நிலையான உள்வரி uint64
r_sip()
{
  uint64 x;
  asm நிலையற்ற("csrr %0, sip" : "=r"(x));
  திரும்பு x;
}

நிலையான உள்வரி வெறுமை
w_sip(uint64 x)
{
  asm நிலையற்ற("csrw sip, %0" : : "r"(x));
}

// Supervisor Interrupt Enable
#define SIE_SEIE (1L << 9) // external
#define SIE_STIE (1L << 5) // timer
நிலையான உள்வரி uint64
r_sie()
{
  uint64 x;
  asm நிலையற்ற("csrr %0, sie" : "=r"(x));
  திரும்பு x;
}

நிலையான உள்வரி வெறுமை
w_sie(uint64 x)
{
  asm நிலையற்ற("csrw sie, %0" : : "r"(x));
}

// Machine-mode Interrupt Enable
#define MIE_STIE (1L << 5) // supervisor timer
நிலையான உள்வரி uint64
r_mie()
{
  uint64 x;
  asm நிலையற்ற("csrr %0, mie" : "=r"(x));
  திரும்பு x;
}

நிலையான உள்வரி வெறுமை
w_mie(uint64 x)
{
  asm நிலையற்ற("csrw mie, %0" : : "r"(x));
}

// supervisor exception program counter, holds the
// instruction address to which a return from
// exception will go.
நிலையான உள்வரி வெறுமை
w_sepc(uint64 x)
{
  asm நிலையற்ற("csrw sepc, %0" : : "r"(x));
}

நிலையான உள்வரி uint64
r_sepc()
{
  uint64 x;
  asm நிலையற்ற("csrr %0, sepc" : "=r"(x));
  திரும்பு x;
}

// Machine Exception Delegation
நிலையான உள்வரி uint64
r_medeleg()
{
  uint64 x;
  asm நிலையற்ற("csrr %0, medeleg" : "=r"(x));
  திரும்பு x;
}

நிலையான உள்வரி வெறுமை
w_medeleg(uint64 x)
{
  asm நிலையற்ற("csrw medeleg, %0" : : "r"(x));
}

// Machine Interrupt Delegation
நிலையான உள்வரி uint64
r_mideleg()
{
  uint64 x;
  asm நிலையற்ற("csrr %0, mideleg" : "=r"(x));
  திரும்பு x;
}

நிலையான உள்வரி வெறுமை
w_mideleg(uint64 x)
{
  asm நிலையற்ற("csrw mideleg, %0" : : "r"(x));
}

// Supervisor Trap-Vector Base Address
// low two bits are mode.
நிலையான உள்வரி வெறுமை
w_stvec(uint64 x)
{
  asm நிலையற்ற("csrw stvec, %0" : : "r"(x));
}

நிலையான உள்வரி uint64
r_stvec()
{
  uint64 x;
  asm நிலையற்ற("csrr %0, stvec" : "=r"(x));
  திரும்பு x;
}

// Supervisor Timer Comparison Register
நிலையான உள்வரி uint64
r_stimecmp()
{
  uint64 x;
  // asm volatile("csrr %0, stimecmp" : "=r" (x) );
  asm நிலையற்ற("csrr %0, 0x14d" : "=r"(x));
  திரும்பு x;
}

நிலையான உள்வரி வெறுமை
w_stimecmp(uint64 x)
{
  // asm volatile("csrw stimecmp, %0" : : "r" (x));
  asm நிலையற்ற("csrw 0x14d, %0" : : "r"(x));
}

// Machine Environment Configuration Register

#define MENVCFG_STCE (1L << 63)
#define MENVCFG_ADUE (1L << 61)

நிலையான உள்வரி uint64
r_menvcfg()
{
  uint64 x;
  // asm volatile("csrr %0, menvcfg" : "=r" (x) );
  asm நிலையற்ற("csrr %0, 0x30a" : "=r"(x));
  திரும்பு x;
}

நிலையான உள்வரி வெறுமை
w_menvcfg(uint64 x)
{
  // asm volatile("csrw menvcfg, %0" : : "r" (x));
  asm நிலையற்ற("csrw 0x30a, %0" : : "r"(x));
}

// Physical Memory Protection
நிலையான உள்வரி வெறுமை
w_pmpcfg0(uint64 x)
{
  asm நிலையற்ற("csrw pmpcfg0, %0" : : "r"(x));
}

நிலையான உள்வரி வெறுமை
w_pmpaddr0(uint64 x)
{
  asm நிலையற்ற("csrw pmpaddr0, %0" : : "r"(x));
}

// use riscv's sv39 page table scheme.
#define SATP_SV39 (8L << 60)

#define MAKE_SATP(pagetable) (SATP_SV39 | (((uint64)pagetable) >> 12))

// supervisor address translation and protection;
// holds the address of the page table.
நிலையான உள்வரி வெறுமை
w_satp(uint64 x)
{
  asm நிலையற்ற("csrw satp, %0" : : "r"(x));
}

நிலையான உள்வரி uint64
r_satp()
{
  uint64 x;
  asm நிலையற்ற("csrr %0, satp" : "=r"(x));
  திரும்பு x;
}

// Supervisor Trap Cause
நிலையான உள்வரி uint64
r_scause()
{
  uint64 x;
  asm நிலையற்ற("csrr %0, scause" : "=r"(x));
  திரும்பு x;
}

// Supervisor Trap Value
நிலையான உள்வரி uint64
r_stval()
{
  uint64 x;
  asm நிலையற்ற("csrr %0, stval" : "=r"(x));
  திரும்பு x;
}

// Machine-mode Counter-Enable
நிலையான உள்வரி வெறுமை
w_mcounteren(uint64 x)
{
  asm நிலையற்ற("csrw mcounteren, %0" : : "r"(x));
}

நிலையான உள்வரி uint64
r_mcounteren()
{
  uint64 x;
  asm நிலையற்ற("csrr %0, mcounteren" : "=r"(x));
  திரும்பு x;
}

// machine-mode cycle counter
நிலையான உள்வரி uint64
r_time()
{
  uint64 x;
  asm நிலையற்ற("csrr %0, time" : "=r"(x));
  திரும்பு x;
}

// enable device interrupts
நிலையான உள்வரி வெறுமை
intr_on()
{
  s_sstatus(SSTATUS_SIE);
}

// disable device interrupts
நிலையான உள்வரி வெறுமை
intr_off()
{
  c_sstatus(SSTATUS_SIE);
}

// are device interrupts enabled?
நிலையான உள்வரி முழு
intr_get()
{
  uint64 x = r_sstatus();
  திரும்பு (x & SSTATUS_SIE) != 0;
}

நிலையான உள்வரி uint64
r_sp()
{
  uint64 x;
  asm நிலையற்ற("mv %0, sp" : "=r"(x));
  திரும்பு x;
}

// read and write tp, the thread pointer, which xv6 uses to hold
// this core's hartid (core number), the index into cpus[].
நிலையான உள்வரி uint64
r_tp()
{
  uint64 x;
  asm நிலையற்ற("mv %0, tp" : "=r"(x));
  திரும்பு x;
}

நிலையான உள்வரி வெறுமை
w_tp(uint64 x)
{
  asm நிலையற்ற("mv tp, %0" : : "r"(x));
}

நிலையான உள்வரி uint64
r_ra()
{
  uint64 x;
  asm நிலையற்ற("mv %0, ra" : "=r"(x));
  திரும்பு x;
}

// flush the TLB.
நிலையான உள்வரி வெறுமை
sfence_vma()
{
  // the zero, zero means flush all TLB entries.
  asm நிலையற்ற("sfence.vma zero, zero" ::: "memory");
}

// fence for memory-mapped IO
நிலையான உள்வரி வெறுமை
io_fence()
{
  asm நிலையற்ற("fence iorw, iorw" ::: "memory");
}

// fence for icache
நிலையான உள்வரி வெறுமை
icache_fence()
{
  asm நிலையற்ற("fence.i" ::: "memory");
}

வகைவரை uint64 pte_t;
வகைவரை uint64 *pagetable_t; // 512 PTEs

#endif // __ASSEMBLER__

#define PGSIZE  4096 // bytes per page
#define PGSHIFT 12   // bits of offset within a page

#define PGROUNDUP(sz)  (((sz) + PGSIZE - 1) & ~(PGSIZE - 1))
#define PGROUNDDOWN(a) (((a)) & ~(PGSIZE - 1))

#define PTE_V (1L << 0) // valid
#define PTE_R (1L << 1)
#define PTE_W (1L << 2)
#define PTE_X (1L << 3)
#define PTE_U (1L << 4) // user can access

// shift a physical address to the right place for a PTE.
#define PA2PTE(pa) ((((uint64)pa) >> 12) << 10)

#define PTE2PA(pte) (((pte) >> 10) << 12)

#define PTE_FLAGS(pte) ((pte) & 0x3FF)

// extract the three 9-bit page table indices from a virtual address.
#define PXMASK         0x1FF // 9 bits
#define PXSHIFT(level) (PGSHIFT + (9 * (level)))
#define PX(level, va)  ((((uint64)(va)) >> PXSHIFT(level)) & PXMASK)

// one beyond the highest possible virtual address.
// MAXVA is actually one bit less than the max allowed by
// Sv39, to avoid having to sign-extend virtual addresses
// that have the high bit set.
#define MAXVA (1L << (9 + 9 + 9 + 12 - 1))
