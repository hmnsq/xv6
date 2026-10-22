#నిర్వచితం_కాకపోతే __ASSEMBLER__

// which hart (core) is this?
స్థిర అంతఃవరుస uint64
r_mhartid()
{
  uint64 x;
  asm అస్థిర("csrr %0, mhartid" : "=r"(x));
  తిరిగి x;
}

// Machine Status Register, mstatus

#నిర్వచించు MSTATUS_MPP_ముసుగు (3L << 11) // previous mode.
#నిర్వచించు MSTATUS_MPP_M    (3L << 11)
#నిర్వచించు MSTATUS_MPP_S    (1L << 11)
#నిర్వచించు MSTATUS_MPP_U    (0L << 11)

స్థిర అంతఃవరుస uint64
r_mstatus()
{
  uint64 x;
  asm అస్థిర("csrr %0, mstatus" : "=r"(x));
  తిరిగి x;
}

స్థిర అంతఃవరుస శూన్య
w_mstatus(uint64 x)
{
  asm అస్థిర("csrw mstatus, %0" : : "r"(x));
}

// machine exception program counter, holds the
// instruction address to which a return from
// exception will go.
స్థిర అంతఃవరుస శూన్య
w_mepc(uint64 x)
{
  asm అస్థిర("csrw mepc, %0" : : "r"(x));
}

// Supervisor Status Register, sstatus

#నిర్వచించు SSTATUS_SPP  (1L << 8) // Previous mode, 1=Supervisor, 0=User
#నిర్వచించు SSTATUS_SPIE (1L << 5) // Supervisor Previous Interrupt Enable
#నిర్వచించు SSTATUS_UPIE (1L << 4) // User Previous Interrupt Enable
#నిర్వచించు SSTATUS_SIE  (1L << 1) // Supervisor Interrupt Enable
#నిర్వచించు SSTATUS_UIE  (1L << 0) // User Interrupt Enable

స్థిర అంతఃవరుస uint64
r_sstatus()
{
  uint64 x;
  asm అస్థిర("csrr %0, sstatus" : "=r"(x));
  తిరిగి x;
}

స్థిర అంతఃవరుస శూన్య
w_sstatus(uint64 x)
{
  asm అస్థిర("csrw sstatus, %0" : : "r"(x));
}

స్థిర అంతఃవరుస శూన్య
s_sstatus(uint64 x)
{
  __asm__ __volatile__("csrs sstatus, %0" ::"rK"(x) : "memory");
}

స్థిర అంతఃవరుస శూన్య
c_sstatus(uint64 x)
{
  __asm__ __volatile__("csrc sstatus, %0" ::"rK"(x) : "memory");
}

స్థిర అంతఃవరుస uint64
rc_sstatus(uint64 x)
{
  __asm__ __volatile__("csrrc %0, sstatus, %1" : "=r"(x) : "rK"(x) : "memory");
  తిరిగి x;
}

// Supervisor Interrupt Pending
స్థిర అంతఃవరుస uint64
r_sip()
{
  uint64 x;
  asm అస్థిర("csrr %0, sip" : "=r"(x));
  తిరిగి x;
}

స్థిర అంతఃవరుస శూన్య
w_sip(uint64 x)
{
  asm అస్థిర("csrw sip, %0" : : "r"(x));
}

// Supervisor Interrupt Enable
#నిర్వచించు SIE_SEIE (1L << 9) // external
#నిర్వచించు SIE_STIE (1L << 5) // timer
స్థిర అంతఃవరుస uint64
r_sie()
{
  uint64 x;
  asm అస్థిర("csrr %0, sie" : "=r"(x));
  తిరిగి x;
}

స్థిర అంతఃవరుస శూన్య
w_sie(uint64 x)
{
  asm అస్థిర("csrw sie, %0" : : "r"(x));
}

// Machine-mode Interrupt Enable
#నిర్వచించు MIE_STIE (1L << 5) // supervisor timer
స్థిర అంతఃవరుస uint64
r_mie()
{
  uint64 x;
  asm అస్థిర("csrr %0, mie" : "=r"(x));
  తిరిగి x;
}

స్థిర అంతఃవరుస శూన్య
w_mie(uint64 x)
{
  asm అస్థిర("csrw mie, %0" : : "r"(x));
}

// supervisor exception program counter, holds the
// instruction address to which a return from
// exception will go.
స్థిర అంతఃవరుస శూన్య
w_sepc(uint64 x)
{
  asm అస్థిర("csrw sepc, %0" : : "r"(x));
}

స్థిర అంతఃవరుస uint64
r_sepc()
{
  uint64 x;
  asm అస్థిర("csrr %0, sepc" : "=r"(x));
  తిరిగి x;
}

// Machine Exception Delegation
స్థిర అంతఃవరుస uint64
r_medeleg()
{
  uint64 x;
  asm అస్థిర("csrr %0, medeleg" : "=r"(x));
  తిరిగి x;
}

స్థిర అంతఃవరుస శూన్య
w_medeleg(uint64 x)
{
  asm అస్థిర("csrw medeleg, %0" : : "r"(x));
}

// Machine Interrupt Delegation
స్థిర అంతఃవరుస uint64
r_mideleg()
{
  uint64 x;
  asm అస్థిర("csrr %0, mideleg" : "=r"(x));
  తిరిగి x;
}

స్థిర అంతఃవరుస శూన్య
w_mideleg(uint64 x)
{
  asm అస్థిర("csrw mideleg, %0" : : "r"(x));
}

// Supervisor Trap-Vector Base Address
// low two bits are mode.
స్థిర అంతఃవరుస శూన్య
w_stvec(uint64 x)
{
  asm అస్థిర("csrw stvec, %0" : : "r"(x));
}

స్థిర అంతఃవరుస uint64
r_stvec()
{
  uint64 x;
  asm అస్థిర("csrr %0, stvec" : "=r"(x));
  తిరిగి x;
}

// Supervisor Timer Comparison Register
స్థిర అంతఃవరుస uint64
r_stimecmp()
{
  uint64 x;
  // asm volatile("csrr %0, stimecmp" : "=r" (x) );
  asm అస్థిర("csrr %0, 0x14d" : "=r"(x));
  తిరిగి x;
}

స్థిర అంతఃవరుస శూన్య
w_stimecmp(uint64 x)
{
  // asm volatile("csrw stimecmp, %0" : : "r" (x));
  asm అస్థిర("csrw 0x14d, %0" : : "r"(x));
}

// Machine Environment Configuration Register

#నిర్వచించు MENVCFG_STCE (1L << 63)
#నిర్వచించు MENVCFG_ADUE (1L << 61)

స్థిర అంతఃవరుస uint64
r_menvcfg()
{
  uint64 x;
  // asm volatile("csrr %0, menvcfg" : "=r" (x) );
  asm అస్థిర("csrr %0, 0x30a" : "=r"(x));
  తిరిగి x;
}

స్థిర అంతఃవరుస శూన్య
w_menvcfg(uint64 x)
{
  // asm volatile("csrw menvcfg, %0" : : "r" (x));
  asm అస్థిర("csrw 0x30a, %0" : : "r"(x));
}

// Physical Memory Protection
స్థిర అంతఃవరుస శూన్య
w_pmpcfg0(uint64 x)
{
  asm అస్థిర("csrw pmpcfg0, %0" : : "r"(x));
}

స్థిర అంతఃవరుస శూన్య
w_pmpaddr0(uint64 x)
{
  asm అస్థిర("csrw pmpaddr0, %0" : : "r"(x));
}

// use riscv's sv39 page table scheme.
#నిర్వచించు SATP_SV39 (8L << 60)

#నిర్వచించు MAKE_SATP(pagetable) (SATP_SV39 | (((uint64)pagetable) >> 12))

// supervisor address translation and protection;
// holds the address of the page table.
స్థిర అంతఃవరుస శూన్య
w_satp(uint64 x)
{
  asm అస్థిర("csrw satp, %0" : : "r"(x));
}

స్థిర అంతఃవరుస uint64
r_satp()
{
  uint64 x;
  asm అస్థిర("csrr %0, satp" : "=r"(x));
  తిరిగి x;
}

// Supervisor Trap Cause
స్థిర అంతఃవరుస uint64
r_scause()
{
  uint64 x;
  asm అస్థిర("csrr %0, scause" : "=r"(x));
  తిరిగి x;
}

// Supervisor Trap Value
స్థిర అంతఃవరుస uint64
r_stval()
{
  uint64 x;
  asm అస్థిర("csrr %0, stval" : "=r"(x));
  తిరిగి x;
}

// Machine-mode Counter-Enable
స్థిర అంతఃవరుస శూన్య
w_mcounteren(uint64 x)
{
  asm అస్థిర("csrw mcounteren, %0" : : "r"(x));
}

స్థిర అంతఃవరుస uint64
r_mcounteren()
{
  uint64 x;
  asm అస్థిర("csrr %0, mcounteren" : "=r"(x));
  తిరిగి x;
}

// machine-mode cycle counter
స్థిర అంతఃవరుస uint64
r_సమయం()
{
  uint64 x;
  asm అస్థిర("csrr %0, time" : "=r"(x));
  తిరిగి x;
}

// enable device interrupts
స్థిర అంతఃవరుస శూన్య
intr_on()
{
  s_sstatus(SSTATUS_SIE);
}

// disable device interrupts
స్థిర అంతఃవరుస శూన్య
intr_off()
{
  c_sstatus(SSTATUS_SIE);
}

// are device interrupts enabled?
స్థిర అంతఃవరుస పూర్ణాంకం
intr_get()
{
  uint64 x = r_sstatus();
  తిరిగి (x & SSTATUS_SIE) != 0;
}

స్థిర అంతఃవరుస uint64
r_sp()
{
  uint64 x;
  asm అస్థిర("mv %0, sp" : "=r"(x));
  తిరిగి x;
}

// read and write tp, the thread pointer, which xv6 uses to hold
// this core's hartid (core number), the index into cpus[].
స్థిర అంతఃవరుస uint64
r_tp()
{
  uint64 x;
  asm అస్థిర("mv %0, tp" : "=r"(x));
  తిరిగి x;
}

స్థిర అంతఃవరుస శూన్య
w_tp(uint64 x)
{
  asm అస్థిర("mv tp, %0" : : "r"(x));
}

స్థిర అంతఃవరుస uint64
r_ra()
{
  uint64 x;
  asm అస్థిర("mv %0, ra" : "=r"(x));
  తిరిగి x;
}

// flush the TLB.
స్థిర అంతఃవరుస శూన్య
sfence_vma()
{
  // the zero, zero means flush all TLB entries.
  asm అస్థిర("sfence.vma zero, zero" ::: "memory");
}

// fence for memory-mapped IO
స్థిర అంతఃవరుస శూన్య
io_fence()
{
  asm అస్థిర("fence iorw, iorw" ::: "memory");
}

// fence for icache
స్థిర అంతఃవరుస శూన్య
icache_fence()
{
  asm అస్థిర("fence.i" ::: "memory");
}

రకనిర్వచనం uint64 pte_t;
రకనిర్వచనం uint64 *pagetable_t; // 512 PTEs

#అయితే_ముగింపు // __ASSEMBLER__

#నిర్వచించు PGSIZE  4096 // bytes per page
#నిర్వచించు PGSHIFT 12   // bits of offset within a page

#నిర్వచించు PGROUNDUP(sz)  (((sz) + PGSIZE - 1) & ~(PGSIZE - 1))
#నిర్వచించు PGROUNDDOWN(a) (((a)) & ~(PGSIZE - 1))

#నిర్వచించు PTE_V (1L << 0) // valid
#నిర్వచించు PTE_R (1L << 1)
#నిర్వచించు PTE_W (1L << 2)
#నిర్వచించు PTE_X (1L << 3)
#నిర్వచించు PTE_U (1L << 4) // user can access

// shift a physical address to the right place for a PTE.
#నిర్వచించు PA2PTE(pa) ((((uint64)pa) >> 12) << 10)

#నిర్వచించు PTE2PA(pte) (((pte) >> 10) << 12)

#నిర్వచించు PTE_FLAGS(pte) ((pte) & 0x3FF)

// extract the three 9-bit page table indices from a virtual address.
#నిర్వచించు PXMASK         0x1FF // 9 bits
#నిర్వచించు PXSHIFT(level) (PGSHIFT + (9 * (level)))
#నిర్వచించు PX(level, va)  ((((uint64)(va)) >> PXSHIFT(level)) & PXMASK)

// one beyond the highest possible virtual address.
// MAXVA is actually one bit less than the max allowed by
// Sv39, to avoid having to sign-extend virtual addresses
// that have the high bit set.
#నిర్వచించు MAXVA (1L << (9 + 9 + 9 + 12 - 1))
