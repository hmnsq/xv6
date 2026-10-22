#جيڪڏهن_وصف_نه_ٿيل __ASSEMBLER__

// which hart (core) is this?
ساكن مضمن uint64
r_mhartid()
{
  uint64 x;
  asm متطاير("csrr %0, mhartid" : "=r"(x));
  أرجع x;
}

// Machine Status Register, mstatus

#وصف_ڪريو MSTATUS_MPP_نقاب (3L << 11) // previous mode.
#وصف_ڪريو MSTATUS_MPP_M    (3L << 11)
#وصف_ڪريو MSTATUS_MPP_S    (1L << 11)
#وصف_ڪريو MSTATUS_MPP_U    (0L << 11)

ساكن مضمن uint64
r_mstatus()
{
  uint64 x;
  asm متطاير("csrr %0, mstatus" : "=r"(x));
  أرجع x;
}

ساكن مضمن فراغ
w_mstatus(uint64 x)
{
  asm متطاير("csrw mstatus, %0" : : "r"(x));
}

// machine exception program counter, holds the
// instruction address to which a return from
// exception will go.
ساكن مضمن فراغ
w_mepc(uint64 x)
{
  asm متطاير("csrw mepc, %0" : : "r"(x));
}

// Supervisor Status Register, sstatus

#وصف_ڪريو SSTATUS_SPP  (1L << 8) // Previous mode, 1=Supervisor, 0=User
#وصف_ڪريو SSTATUS_SPIE (1L << 5) // Supervisor Previous Interrupt Enable
#وصف_ڪريو SSTATUS_UPIE (1L << 4) // User Previous Interrupt Enable
#وصف_ڪريو SSTATUS_SIE  (1L << 1) // Supervisor Interrupt Enable
#وصف_ڪريو SSTATUS_UIE  (1L << 0) // User Interrupt Enable

ساكن مضمن uint64
r_sstatus()
{
  uint64 x;
  asm متطاير("csrr %0, sstatus" : "=r"(x));
  أرجع x;
}

ساكن مضمن فراغ
w_sstatus(uint64 x)
{
  asm متطاير("csrw sstatus, %0" : : "r"(x));
}

ساكن مضمن فراغ
s_sstatus(uint64 x)
{
  __asm__ __volatile__("csrs sstatus, %0" ::"rK"(x) : "memory");
}

ساكن مضمن فراغ
c_sstatus(uint64 x)
{
  __asm__ __volatile__("csrc sstatus, %0" ::"rK"(x) : "memory");
}

ساكن مضمن uint64
rc_sstatus(uint64 x)
{
  __asm__ __volatile__("csrrc %0, sstatus, %1" : "=r"(x) : "rK"(x) : "memory");
  أرجع x;
}

// Supervisor Interrupt Pending
ساكن مضمن uint64
r_sip()
{
  uint64 x;
  asm متطاير("csrr %0, sip" : "=r"(x));
  أرجع x;
}

ساكن مضمن فراغ
w_sip(uint64 x)
{
  asm متطاير("csrw sip, %0" : : "r"(x));
}

// Supervisor Interrupt Enable
#وصف_ڪريو SIE_SEIE (1L << 9) // external
#وصف_ڪريو SIE_STIE (1L << 5) // timer
ساكن مضمن uint64
r_sie()
{
  uint64 x;
  asm متطاير("csrr %0, sie" : "=r"(x));
  أرجع x;
}

ساكن مضمن فراغ
w_sie(uint64 x)
{
  asm متطاير("csrw sie, %0" : : "r"(x));
}

// Machine-mode Interrupt Enable
#وصف_ڪريو MIE_STIE (1L << 5) // supervisor timer
ساكن مضمن uint64
r_mie()
{
  uint64 x;
  asm متطاير("csrr %0, mie" : "=r"(x));
  أرجع x;
}

ساكن مضمن فراغ
w_mie(uint64 x)
{
  asm متطاير("csrw mie, %0" : : "r"(x));
}

// supervisor exception program counter, holds the
// instruction address to which a return from
// exception will go.
ساكن مضمن فراغ
w_sepc(uint64 x)
{
  asm متطاير("csrw sepc, %0" : : "r"(x));
}

ساكن مضمن uint64
r_sepc()
{
  uint64 x;
  asm متطاير("csrr %0, sepc" : "=r"(x));
  أرجع x;
}

// Machine Exception Delegation
ساكن مضمن uint64
r_medeleg()
{
  uint64 x;
  asm متطاير("csrr %0, medeleg" : "=r"(x));
  أرجع x;
}

ساكن مضمن فراغ
w_medeleg(uint64 x)
{
  asm متطاير("csrw medeleg, %0" : : "r"(x));
}

// Machine Interrupt Delegation
ساكن مضمن uint64
r_mideleg()
{
  uint64 x;
  asm متطاير("csrr %0, mideleg" : "=r"(x));
  أرجع x;
}

ساكن مضمن فراغ
w_mideleg(uint64 x)
{
  asm متطاير("csrw mideleg, %0" : : "r"(x));
}

// Supervisor Trap-Vector Base Address
// low two bits are mode.
ساكن مضمن فراغ
w_stvec(uint64 x)
{
  asm متطاير("csrw stvec, %0" : : "r"(x));
}

ساكن مضمن uint64
r_stvec()
{
  uint64 x;
  asm متطاير("csrr %0, stvec" : "=r"(x));
  أرجع x;
}

// Supervisor Timer Comparison Register
ساكن مضمن uint64
r_stimecmp()
{
  uint64 x;
  // asm volatile("csrr %0, stimecmp" : "=r" (x) );
  asm متطاير("csrr %0, 0x14d" : "=r"(x));
  أرجع x;
}

ساكن مضمن فراغ
w_stimecmp(uint64 x)
{
  // asm volatile("csrw stimecmp, %0" : : "r" (x));
  asm متطاير("csrw 0x14d, %0" : : "r"(x));
}

// Machine Environment Configuration Register

#وصف_ڪريو MENVCFG_STCE (1L << 63)
#وصف_ڪريو MENVCFG_ADUE (1L << 61)

ساكن مضمن uint64
r_menvcfg()
{
  uint64 x;
  // asm volatile("csrr %0, menvcfg" : "=r" (x) );
  asm متطاير("csrr %0, 0x30a" : "=r"(x));
  أرجع x;
}

ساكن مضمن فراغ
w_menvcfg(uint64 x)
{
  // asm volatile("csrw menvcfg, %0" : : "r" (x));
  asm متطاير("csrw 0x30a, %0" : : "r"(x));
}

// Physical Memory Protection
ساكن مضمن فراغ
w_pmpcfg0(uint64 x)
{
  asm متطاير("csrw pmpcfg0, %0" : : "r"(x));
}

ساكن مضمن فراغ
w_pmpaddr0(uint64 x)
{
  asm متطاير("csrw pmpaddr0, %0" : : "r"(x));
}

// use riscv's sv39 page table scheme.
#وصف_ڪريو SATP_SV39 (8L << 60)

#وصف_ڪريو MAKE_SATP(pagetable) (SATP_SV39 | (((uint64)pagetable) >> 12))

// supervisor address translation and protection;
// holds the address of the page table.
ساكن مضمن فراغ
w_satp(uint64 x)
{
  asm متطاير("csrw satp, %0" : : "r"(x));
}

ساكن مضمن uint64
r_satp()
{
  uint64 x;
  asm متطاير("csrr %0, satp" : "=r"(x));
  أرجع x;
}

// Supervisor Trap Cause
ساكن مضمن uint64
r_scause()
{
  uint64 x;
  asm متطاير("csrr %0, scause" : "=r"(x));
  أرجع x;
}

// Supervisor Trap Value
ساكن مضمن uint64
r_stval()
{
  uint64 x;
  asm متطاير("csrr %0, stval" : "=r"(x));
  أرجع x;
}

// Machine-mode Counter-Enable
ساكن مضمن فراغ
w_mcounteren(uint64 x)
{
  asm متطاير("csrw mcounteren, %0" : : "r"(x));
}

ساكن مضمن uint64
r_mcounteren()
{
  uint64 x;
  asm متطاير("csrr %0, mcounteren" : "=r"(x));
  أرجع x;
}

// machine-mode cycle counter
ساكن مضمن uint64
r_وقت()
{
  uint64 x;
  asm متطاير("csrr %0, time" : "=r"(x));
  أرجع x;
}

// enable device interrupts
ساكن مضمن فراغ
intr_on()
{
  s_sstatus(SSTATUS_SIE);
}

// disable device interrupts
ساكن مضمن فراغ
intr_off()
{
  c_sstatus(SSTATUS_SIE);
}

// are device interrupts enabled?
ساكن مضمن صحيح
intr_get()
{
  uint64 x = r_sstatus();
  أرجع (x & SSTATUS_SIE) != 0;
}

ساكن مضمن uint64
r_sp()
{
  uint64 x;
  asm متطاير("mv %0, sp" : "=r"(x));
  أرجع x;
}

// read and write tp, the thread pointer, which xv6 uses to hold
// this core's hartid (core number), the index into cpus[].
ساكن مضمن uint64
r_tp()
{
  uint64 x;
  asm متطاير("mv %0, tp" : "=r"(x));
  أرجع x;
}

ساكن مضمن فراغ
w_tp(uint64 x)
{
  asm متطاير("mv tp, %0" : : "r"(x));
}

ساكن مضمن uint64
r_ra()
{
  uint64 x;
  asm متطاير("mv %0, ra" : "=r"(x));
  أرجع x;
}

// flush the TLB.
ساكن مضمن فراغ
sfence_vma()
{
  // the zero, zero means flush all TLB entries.
  asm متطاير("sfence.vma zero, zero" ::: "memory");
}

// fence for memory-mapped IO
ساكن مضمن فراغ
io_fence()
{
  asm متطاير("fence iorw, iorw" ::: "memory");
}

// fence for icache
ساكن مضمن فراغ
icache_fence()
{
  asm متطاير("fence.i" ::: "memory");
}

تعريفنوع uint64 pte_t;
تعريفنوع uint64 *pagetable_t; // 512 PTEs

#جيڪڏهن_پورو // __ASSEMBLER__

#وصف_ڪريو PGSIZE  4096 // bytes per page
#وصف_ڪريو PGSHIFT 12   // bits of offset within a page

#وصف_ڪريو PGROUNDUP(sz)  (((sz) + PGSIZE - 1) & ~(PGSIZE - 1))
#وصف_ڪريو PGROUNDDOWN(a) (((a)) & ~(PGSIZE - 1))

#وصف_ڪريو PTE_V (1L << 0) // valid
#وصف_ڪريو PTE_R (1L << 1)
#وصف_ڪريو PTE_W (1L << 2)
#وصف_ڪريو PTE_X (1L << 3)
#وصف_ڪريو PTE_U (1L << 4) // user can access

// shift a physical address to the right place for a PTE.
#وصف_ڪريو PA2PTE(pa) ((((uint64)pa) >> 12) << 10)

#وصف_ڪريو PTE2PA(pte) (((pte) >> 10) << 12)

#وصف_ڪريو PTE_FLAGS(pte) ((pte) & 0x3FF)

// extract the three 9-bit page table indices from a virtual address.
#وصف_ڪريو PXMASK         0x1FF // 9 bits
#وصف_ڪريو PXSHIFT(level) (PGSHIFT + (9 * (level)))
#وصف_ڪريو PX(level, va)  ((((uint64)(va)) >> PXSHIFT(level)) & PXMASK)

// one beyond the highest possible virtual address.
// MAXVA is actually one bit less than the max allowed by
// Sv39, to avoid having to sign-extend virtual addresses
// that have the high bit set.
#وصف_ڪريو MAXVA (1L << (9 + 9 + 9 + 12 - 1))
