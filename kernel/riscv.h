#اگر_غیر_تعریف_شدہ __ASSEMBLER__

// which hart (core) is this?
ساکن اندرونی uint64
r_mhartid()
{
  uint64 x;
  asm متلون("csrr %0, mhartid" : "=r"(x));
  واپس x;
}

// Machine Status Register, mstatus

#تعریف_کرو MSTATUS_MPP_نقاب (3L << 11) // previous mode.
#تعریف_کرو MSTATUS_MPP_M    (3L << 11)
#تعریف_کرو MSTATUS_MPP_S    (1L << 11)
#تعریف_کرو MSTATUS_MPP_U    (0L << 11)

ساکن اندرونی uint64
r_mstatus()
{
  uint64 x;
  asm متلون("csrr %0, mstatus" : "=r"(x));
  واپس x;
}

ساکن اندرونی خالی
w_mstatus(uint64 x)
{
  asm متلون("csrw mstatus, %0" : : "r"(x));
}

// machine exception program counter, holds the
// instruction address to which a return from
// exception will go.
ساکن اندرونی خالی
w_mepc(uint64 x)
{
  asm متلون("csrw mepc, %0" : : "r"(x));
}

// Supervisor Status Register, sstatus

#تعریف_کرو SSTATUS_SPP  (1L << 8) // Previous mode, 1=Supervisor, 0=User
#تعریف_کرو SSTATUS_SPIE (1L << 5) // Supervisor Previous Interrupt Enable
#تعریف_کرو SSTATUS_UPIE (1L << 4) // User Previous Interrupt Enable
#تعریف_کرو SSTATUS_SIE  (1L << 1) // Supervisor Interrupt Enable
#تعریف_کرو SSTATUS_UIE  (1L << 0) // User Interrupt Enable

ساکن اندرونی uint64
r_sstatus()
{
  uint64 x;
  asm متلون("csrr %0, sstatus" : "=r"(x));
  واپس x;
}

ساکن اندرونی خالی
w_sstatus(uint64 x)
{
  asm متلون("csrw sstatus, %0" : : "r"(x));
}

ساکن اندرونی خالی
s_sstatus(uint64 x)
{
  __asm__ __volatile__("csrs sstatus, %0" ::"rK"(x) : "memory");
}

ساکن اندرونی خالی
c_sstatus(uint64 x)
{
  __asm__ __volatile__("csrc sstatus, %0" ::"rK"(x) : "memory");
}

ساکن اندرونی uint64
rc_sstatus(uint64 x)
{
  __asm__ __volatile__("csrrc %0, sstatus, %1" : "=r"(x) : "rK"(x) : "memory");
  واپس x;
}

// Supervisor Interrupt Pending
ساکن اندرونی uint64
r_sip()
{
  uint64 x;
  asm متلون("csrr %0, sip" : "=r"(x));
  واپس x;
}

ساکن اندرونی خالی
w_sip(uint64 x)
{
  asm متلون("csrw sip, %0" : : "r"(x));
}

// Supervisor Interrupt Enable
#تعریف_کرو SIE_SEIE (1L << 9) // external
#تعریف_کرو SIE_STIE (1L << 5) // timer
ساکن اندرونی uint64
r_sie()
{
  uint64 x;
  asm متلون("csrr %0, sie" : "=r"(x));
  واپس x;
}

ساکن اندرونی خالی
w_sie(uint64 x)
{
  asm متلون("csrw sie, %0" : : "r"(x));
}

// Machine-mode Interrupt Enable
#تعریف_کرو MIE_STIE (1L << 5) // supervisor timer
ساکن اندرونی uint64
r_mie()
{
  uint64 x;
  asm متلون("csrr %0, mie" : "=r"(x));
  واپس x;
}

ساکن اندرونی خالی
w_mie(uint64 x)
{
  asm متلون("csrw mie, %0" : : "r"(x));
}

// supervisor exception program counter, holds the
// instruction address to which a return from
// exception will go.
ساکن اندرونی خالی
w_sepc(uint64 x)
{
  asm متلون("csrw sepc, %0" : : "r"(x));
}

ساکن اندرونی uint64
r_sepc()
{
  uint64 x;
  asm متلون("csrr %0, sepc" : "=r"(x));
  واپس x;
}

// Machine Exception Delegation
ساکن اندرونی uint64
r_medeleg()
{
  uint64 x;
  asm متلون("csrr %0, medeleg" : "=r"(x));
  واپس x;
}

ساکن اندرونی خالی
w_medeleg(uint64 x)
{
  asm متلون("csrw medeleg, %0" : : "r"(x));
}

// Machine Interrupt Delegation
ساکن اندرونی uint64
r_mideleg()
{
  uint64 x;
  asm متلون("csrr %0, mideleg" : "=r"(x));
  واپس x;
}

ساکن اندرونی خالی
w_mideleg(uint64 x)
{
  asm متلون("csrw mideleg, %0" : : "r"(x));
}

// Supervisor Trap-Vector Base Address
// low two bits are mode.
ساکن اندرونی خالی
w_stvec(uint64 x)
{
  asm متلون("csrw stvec, %0" : : "r"(x));
}

ساکن اندرونی uint64
r_stvec()
{
  uint64 x;
  asm متلون("csrr %0, stvec" : "=r"(x));
  واپس x;
}

// Supervisor Timer Comparison Register
ساکن اندرونی uint64
r_stimecmp()
{
  uint64 x;
  // asm volatile("csrr %0, stimecmp" : "=r" (x) );
  asm متلون("csrr %0, 0x14d" : "=r"(x));
  واپس x;
}

ساکن اندرونی خالی
w_stimecmp(uint64 x)
{
  // asm volatile("csrw stimecmp, %0" : : "r" (x));
  asm متلون("csrw 0x14d, %0" : : "r"(x));
}

// Machine Environment Configuration Register

#تعریف_کرو MENVCFG_STCE (1L << 63)
#تعریف_کرو MENVCFG_ADUE (1L << 61)

ساکن اندرونی uint64
r_menvcfg()
{
  uint64 x;
  // asm volatile("csrr %0, menvcfg" : "=r" (x) );
  asm متلون("csrr %0, 0x30a" : "=r"(x));
  واپس x;
}

ساکن اندرونی خالی
w_menvcfg(uint64 x)
{
  // asm volatile("csrw menvcfg, %0" : : "r" (x));
  asm متلون("csrw 0x30a, %0" : : "r"(x));
}

// Physical Memory Protection
ساکن اندرونی خالی
w_pmpcfg0(uint64 x)
{
  asm متلون("csrw pmpcfg0, %0" : : "r"(x));
}

ساکن اندرونی خالی
w_pmpaddr0(uint64 x)
{
  asm متلون("csrw pmpaddr0, %0" : : "r"(x));
}

// use riscv's sv39 page table scheme.
#تعریف_کرو SATP_SV39 (8L << 60)

#تعریف_کرو MAKE_SATP(pagetable) (SATP_SV39 | (((uint64)pagetable) >> 12))

// supervisor address translation and protection;
// holds the address of the page table.
ساکن اندرونی خالی
w_satp(uint64 x)
{
  asm متلون("csrw satp, %0" : : "r"(x));
}

ساکن اندرونی uint64
r_satp()
{
  uint64 x;
  asm متلون("csrr %0, satp" : "=r"(x));
  واپس x;
}

// Supervisor Trap Cause
ساکن اندرونی uint64
r_scause()
{
  uint64 x;
  asm متلون("csrr %0, scause" : "=r"(x));
  واپس x;
}

// Supervisor Trap Value
ساکن اندرونی uint64
r_stval()
{
  uint64 x;
  asm متلون("csrr %0, stval" : "=r"(x));
  واپس x;
}

// Machine-mode Counter-Enable
ساکن اندرونی خالی
w_mcounteren(uint64 x)
{
  asm متلون("csrw mcounteren, %0" : : "r"(x));
}

ساکن اندرونی uint64
r_mcounteren()
{
  uint64 x;
  asm متلون("csrr %0, mcounteren" : "=r"(x));
  واپس x;
}

// machine-mode cycle counter
ساکن اندرونی uint64
r_وقت()
{
  uint64 x;
  asm متلون("csrr %0, time" : "=r"(x));
  واپس x;
}

// enable device interrupts
ساکن اندرونی خالی
intr_on()
{
  s_sstatus(SSTATUS_SIE);
}

// disable device interrupts
ساکن اندرونی خالی
intr_off()
{
  c_sstatus(SSTATUS_SIE);
}

// are device interrupts enabled?
ساکن اندرونی صحیح
intr_get()
{
  uint64 x = r_sstatus();
  واپس (x & SSTATUS_SIE) != 0;
}

ساکن اندرونی uint64
r_sp()
{
  uint64 x;
  asm متلون("mv %0, sp" : "=r"(x));
  واپس x;
}

// read and write tp, the thread pointer, which xv6 uses to hold
// this core's hartid (core number), the index into cpus[].
ساکن اندرونی uint64
r_tp()
{
  uint64 x;
  asm متلون("mv %0, tp" : "=r"(x));
  واپس x;
}

ساکن اندرونی خالی
w_tp(uint64 x)
{
  asm متلون("mv tp, %0" : : "r"(x));
}

ساکن اندرونی uint64
r_ra()
{
  uint64 x;
  asm متلون("mv %0, ra" : "=r"(x));
  واپس x;
}

// flush the TLB.
ساکن اندرونی خالی
sfence_vma()
{
  // the zero, zero means flush all TLB entries.
  asm متلون("sfence.vma zero, zero" ::: "memory");
}

// fence for memory-mapped IO
ساکن اندرونی خالی
io_fence()
{
  asm متلون("fence iorw, iorw" ::: "memory");
}

// fence for icache
ساکن اندرونی خالی
icache_fence()
{
  asm متلون("fence.i" ::: "memory");
}

قسمتعریف uint64 pte_t;
قسمتعریف uint64 *pagetable_t; // 512 PTEs

#اگر_ختم // __ASSEMBLER__

#تعریف_کرو PGSIZE  4096 // bytes per page
#تعریف_کرو PGSHIFT 12   // bits of offset within a page

#تعریف_کرو PGROUNDUP(sz)  (((sz) + PGSIZE - 1) & ~(PGSIZE - 1))
#تعریف_کرو PGROUNDDOWN(a) (((a)) & ~(PGSIZE - 1))

#تعریف_کرو PTE_V (1L << 0) // valid
#تعریف_کرو PTE_R (1L << 1)
#تعریف_کرو PTE_W (1L << 2)
#تعریف_کرو PTE_X (1L << 3)
#تعریف_کرو PTE_U (1L << 4) // user can access

// shift a physical address to the right place for a PTE.
#تعریف_کرو PA2PTE(pa) ((((uint64)pa) >> 12) << 10)

#تعریف_کرو PTE2PA(pte) (((pte) >> 10) << 12)

#تعریف_کرو PTE_FLAGS(pte) ((pte) & 0x3FF)

// extract the three 9-bit page table indices from a virtual address.
#تعریف_کرو PXMASK         0x1FF // 9 bits
#تعریف_کرو PXSHIFT(level) (PGSHIFT + (9 * (level)))
#تعریف_کرو PX(level, va)  ((((uint64)(va)) >> PXSHIFT(level)) & PXMASK)

// one beyond the highest possible virtual address.
// MAXVA is actually one bit less than the max allowed by
// Sv39, to avoid having to sign-extend virtual addresses
// that have the high bit set.
#تعریف_کرو MAXVA (1L << (9 + 9 + 9 + 12 - 1))
