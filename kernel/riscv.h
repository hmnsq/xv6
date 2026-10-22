#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ __ASSEMBLER__

// which hart (core) is this?
ਸਥਿਰ ਸਤਰਅੰਦਰ uint64
r_mhartid()
{
  uint64 x;
  asm ਅਸਥਿਰ("csrr %0, mhartid" : "=r"(x));
  ਵਾਪਸ x;
}

// Machine Status Register, mstatus

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MSTATUS_MPP_ਨਕਾਬ (3L << 11) // previous mode.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MSTATUS_MPP_M    (3L << 11)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MSTATUS_MPP_S    (1L << 11)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MSTATUS_MPP_U    (0L << 11)

ਸਥਿਰ ਸਤਰਅੰਦਰ uint64
r_mstatus()
{
  uint64 x;
  asm ਅਸਥਿਰ("csrr %0, mstatus" : "=r"(x));
  ਵਾਪਸ x;
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ
w_mstatus(uint64 x)
{
  asm ਅਸਥਿਰ("csrw mstatus, %0" : : "r"(x));
}

// machine exception program counter, holds the
// instruction address to which a return from
// exception will go.
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ
w_mepc(uint64 x)
{
  asm ਅਸਥਿਰ("csrw mepc, %0" : : "r"(x));
}

// Supervisor Status Register, sstatus

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SSTATUS_SPP  (1L << 8) // Previous mode, 1=Supervisor, 0=User
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SSTATUS_SPIE (1L << 5) // Supervisor Previous Interrupt Enable
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SSTATUS_UPIE (1L << 4) // User Previous Interrupt Enable
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SSTATUS_SIE  (1L << 1) // Supervisor Interrupt Enable
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SSTATUS_UIE  (1L << 0) // User Interrupt Enable

ਸਥਿਰ ਸਤਰਅੰਦਰ uint64
r_sstatus()
{
  uint64 x;
  asm ਅਸਥਿਰ("csrr %0, sstatus" : "=r"(x));
  ਵਾਪਸ x;
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ
w_sstatus(uint64 x)
{
  asm ਅਸਥਿਰ("csrw sstatus, %0" : : "r"(x));
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ
s_sstatus(uint64 x)
{
  __asm__ __volatile__("csrs sstatus, %0" ::"rK"(x) : "memory");
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ
c_sstatus(uint64 x)
{
  __asm__ __volatile__("csrc sstatus, %0" ::"rK"(x) : "memory");
}

ਸਥਿਰ ਸਤਰਅੰਦਰ uint64
rc_sstatus(uint64 x)
{
  __asm__ __volatile__("csrrc %0, sstatus, %1" : "=r"(x) : "rK"(x) : "memory");
  ਵਾਪਸ x;
}

// Supervisor Interrupt Pending
ਸਥਿਰ ਸਤਰਅੰਦਰ uint64
r_sip()
{
  uint64 x;
  asm ਅਸਥਿਰ("csrr %0, sip" : "=r"(x));
  ਵਾਪਸ x;
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ
w_sip(uint64 x)
{
  asm ਅਸਥਿਰ("csrw sip, %0" : : "r"(x));
}

// Supervisor Interrupt Enable
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIE_SEIE (1L << 9) // external
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIE_STIE (1L << 5) // timer
ਸਥਿਰ ਸਤਰਅੰਦਰ uint64
r_sie()
{
  uint64 x;
  asm ਅਸਥਿਰ("csrr %0, sie" : "=r"(x));
  ਵਾਪਸ x;
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ
w_sie(uint64 x)
{
  asm ਅਸਥਿਰ("csrw sie, %0" : : "r"(x));
}

// Machine-mode Interrupt Enable
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MIE_STIE (1L << 5) // supervisor timer
ਸਥਿਰ ਸਤਰਅੰਦਰ uint64
r_mie()
{
  uint64 x;
  asm ਅਸਥਿਰ("csrr %0, mie" : "=r"(x));
  ਵਾਪਸ x;
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ
w_mie(uint64 x)
{
  asm ਅਸਥਿਰ("csrw mie, %0" : : "r"(x));
}

// supervisor exception program counter, holds the
// instruction address to which a return from
// exception will go.
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ
w_sepc(uint64 x)
{
  asm ਅਸਥਿਰ("csrw sepc, %0" : : "r"(x));
}

ਸਥਿਰ ਸਤਰਅੰਦਰ uint64
r_sepc()
{
  uint64 x;
  asm ਅਸਥਿਰ("csrr %0, sepc" : "=r"(x));
  ਵਾਪਸ x;
}

// Machine Exception Delegation
ਸਥਿਰ ਸਤਰਅੰਦਰ uint64
r_medeleg()
{
  uint64 x;
  asm ਅਸਥਿਰ("csrr %0, medeleg" : "=r"(x));
  ਵਾਪਸ x;
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ
w_medeleg(uint64 x)
{
  asm ਅਸਥਿਰ("csrw medeleg, %0" : : "r"(x));
}

// Machine Interrupt Delegation
ਸਥਿਰ ਸਤਰਅੰਦਰ uint64
r_mideleg()
{
  uint64 x;
  asm ਅਸਥਿਰ("csrr %0, mideleg" : "=r"(x));
  ਵਾਪਸ x;
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ
w_mideleg(uint64 x)
{
  asm ਅਸਥਿਰ("csrw mideleg, %0" : : "r"(x));
}

// Supervisor Trap-Vector Base Address
// low two bits are mode.
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ
w_stvec(uint64 x)
{
  asm ਅਸਥਿਰ("csrw stvec, %0" : : "r"(x));
}

ਸਥਿਰ ਸਤਰਅੰਦਰ uint64
r_stvec()
{
  uint64 x;
  asm ਅਸਥਿਰ("csrr %0, stvec" : "=r"(x));
  ਵਾਪਸ x;
}

// Supervisor Timer Comparison Register
ਸਥਿਰ ਸਤਰਅੰਦਰ uint64
r_stimecmp()
{
  uint64 x;
  // asm volatile("csrr %0, stimecmp" : "=r" (x) );
  asm ਅਸਥਿਰ("csrr %0, 0x14d" : "=r"(x));
  ਵਾਪਸ x;
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ
w_stimecmp(uint64 x)
{
  // asm volatile("csrw stimecmp, %0" : : "r" (x));
  asm ਅਸਥਿਰ("csrw 0x14d, %0" : : "r"(x));
}

// Machine Environment Configuration Register

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MENVCFG_STCE (1L << 63)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MENVCFG_ADUE (1L << 61)

ਸਥਿਰ ਸਤਰਅੰਦਰ uint64
r_menvcfg()
{
  uint64 x;
  // asm volatile("csrr %0, menvcfg" : "=r" (x) );
  asm ਅਸਥਿਰ("csrr %0, 0x30a" : "=r"(x));
  ਵਾਪਸ x;
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ
w_menvcfg(uint64 x)
{
  // asm volatile("csrw menvcfg, %0" : : "r" (x));
  asm ਅਸਥਿਰ("csrw 0x30a, %0" : : "r"(x));
}

// Physical Memory Protection
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ
w_pmpcfg0(uint64 x)
{
  asm ਅਸਥਿਰ("csrw pmpcfg0, %0" : : "r"(x));
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ
w_pmpaddr0(uint64 x)
{
  asm ਅਸਥਿਰ("csrw pmpaddr0, %0" : : "r"(x));
}

// use riscv's sv39 page table scheme.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SATP_SV39 (8L << 60)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAKE_SATP(pagetable) (SATP_SV39 | (((uint64)pagetable) >> 12))

// supervisor address translation and protection;
// holds the address of the page table.
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ
w_satp(uint64 x)
{
  asm ਅਸਥਿਰ("csrw satp, %0" : : "r"(x));
}

ਸਥਿਰ ਸਤਰਅੰਦਰ uint64
r_satp()
{
  uint64 x;
  asm ਅਸਥਿਰ("csrr %0, satp" : "=r"(x));
  ਵਾਪਸ x;
}

// Supervisor Trap Cause
ਸਥਿਰ ਸਤਰਅੰਦਰ uint64
r_scause()
{
  uint64 x;
  asm ਅਸਥਿਰ("csrr %0, scause" : "=r"(x));
  ਵਾਪਸ x;
}

// Supervisor Trap Value
ਸਥਿਰ ਸਤਰਅੰਦਰ uint64
r_stval()
{
  uint64 x;
  asm ਅਸਥਿਰ("csrr %0, stval" : "=r"(x));
  ਵਾਪਸ x;
}

// Machine-mode Counter-Enable
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ
w_mcounteren(uint64 x)
{
  asm ਅਸਥਿਰ("csrw mcounteren, %0" : : "r"(x));
}

ਸਥਿਰ ਸਤਰਅੰਦਰ uint64
r_mcounteren()
{
  uint64 x;
  asm ਅਸਥਿਰ("csrr %0, mcounteren" : "=r"(x));
  ਵਾਪਸ x;
}

// machine-mode cycle counter
ਸਥਿਰ ਸਤਰਅੰਦਰ uint64
r_ਸਮਾਂ()
{
  uint64 x;
  asm ਅਸਥਿਰ("csrr %0, time" : "=r"(x));
  ਵਾਪਸ x;
}

// enable device interrupts
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ
intr_on()
{
  s_sstatus(SSTATUS_SIE);
}

// disable device interrupts
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ
intr_off()
{
  c_sstatus(SSTATUS_SIE);
}

// are device interrupts enabled?
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ
intr_get()
{
  uint64 x = r_sstatus();
  ਵਾਪਸ (x & SSTATUS_SIE) != 0;
}

ਸਥਿਰ ਸਤਰਅੰਦਰ uint64
r_sp()
{
  uint64 x;
  asm ਅਸਥਿਰ("mv %0, sp" : "=r"(x));
  ਵਾਪਸ x;
}

// read and write tp, the thread pointer, which xv6 uses to hold
// this core's hartid (core number), the index into cpus[].
ਸਥਿਰ ਸਤਰਅੰਦਰ uint64
r_tp()
{
  uint64 x;
  asm ਅਸਥਿਰ("mv %0, tp" : "=r"(x));
  ਵਾਪਸ x;
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ
w_tp(uint64 x)
{
  asm ਅਸਥਿਰ("mv tp, %0" : : "r"(x));
}

ਸਥਿਰ ਸਤਰਅੰਦਰ uint64
r_ra()
{
  uint64 x;
  asm ਅਸਥਿਰ("mv %0, ra" : "=r"(x));
  ਵਾਪਸ x;
}

// flush the TLB.
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ
sfence_vma()
{
  // the zero, zero means flush all TLB entries.
  asm ਅਸਥਿਰ("sfence.vma zero, zero" ::: "memory");
}

// fence for memory-mapped IO
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ
io_fence()
{
  asm ਅਸਥਿਰ("fence iorw, iorw" ::: "memory");
}

// fence for icache
ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ
icache_fence()
{
  asm ਅਸਥਿਰ("fence.i" ::: "memory");
}

ਕਿਸਮਪਰਿਭਾਸ਼ਾ uint64 pte_t;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ uint64 *pagetable_t; // 512 PTEs

#ਜੇ_ਅੰਤ // __ASSEMBLER__

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PGSIZE  4096 // bytes per page
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PGSHIFT 12   // bits of offset within a page

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PGROUNDUP(sz)  (((sz) + PGSIZE - 1) & ~(PGSIZE - 1))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PGROUNDDOWN(a) (((a)) & ~(PGSIZE - 1))

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PTE_V (1L << 0) // valid
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PTE_R (1L << 1)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PTE_W (1L << 2)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PTE_X (1L << 3)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PTE_U (1L << 4) // user can access

// shift a physical address to the right place for a PTE.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PA2PTE(pa) ((((uint64)pa) >> 12) << 10)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PTE2PA(pte) (((pte) >> 10) << 12)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PTE_FLAGS(pte) ((pte) & 0x3FF)

// extract the three 9-bit page table indices from a virtual address.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PXMASK         0x1FF // 9 bits
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PXSHIFT(level) (PGSHIFT + (9 * (level)))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PX(level, va)  ((((uint64)(va)) >> PXSHIFT(level)) & PXMASK)

// one beyond the highest possible virtual address.
// MAXVA is actually one bit less than the max allowed by
// Sv39, to avoid having to sign-extend virtual addresses
// that have the high bit set.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAXVA (1L << (9 + 9 + 9 + 12 - 1))
