#अगर_अघोषित __ASSEMBLER__

// which hart (core) is this?
अटल अंतरभूत uint64
r_mhartid()
{
  uint64 x;
  यंत्र अस्थिर("csrr %0, mhartid" : "=r"(x));
  वापस x;
}

// Machine Status Register, mstatus

#घोषणा MSTATUS_MPP_MASK (3L << 11) // previous mode.
#घोषणा MSTATUS_MPP_M    (3L << 11)
#घोषणा MSTATUS_MPP_S    (1L << 11)
#घोषणा MSTATUS_MPP_U    (0L << 11)

अटल अंतरभूत uint64
r_mstatus()
{
  uint64 x;
  यंत्र अस्थिर("csrr %0, mstatus" : "=r"(x));
  वापस x;
}

अटल अंतरभूत व्योम
w_mstatus(uint64 x)
{
  यंत्र अस्थिर("csrw mstatus, %0" : : "r"(x));
}

// machine exception program counter, holds the
// instruction address to which a return from
// exception will go.
अटल अंतरभूत व्योम
w_mepc(uint64 x)
{
  यंत्र अस्थिर("csrw mepc, %0" : : "r"(x));
}

// Supervisor Status Register, sstatus

#घोषणा SSTATUS_SPP  (1L << 8) // Previous mode, 1=Supervisor, 0=User
#घोषणा SSTATUS_SPIE (1L << 5) // Supervisor Previous Interrupt Enable
#घोषणा SSTATUS_UPIE (1L << 4) // User Previous Interrupt Enable
#घोषणा SSTATUS_SIE  (1L << 1) // Supervisor Interrupt Enable
#घोषणा SSTATUS_UIE  (1L << 0) // User Interrupt Enable

अटल अंतरभूत uint64
r_sstatus()
{
  uint64 x;
  यंत्र अस्थिर("csrr %0, sstatus" : "=r"(x));
  वापस x;
}

अटल अंतरभूत व्योम
w_sstatus(uint64 x)
{
  यंत्र अस्थिर("csrw sstatus, %0" : : "r"(x));
}

अटल अंतरभूत व्योम
s_sstatus(uint64 x)
{
  __asm__ __volatile__("csrs sstatus, %0" ::"rK"(x) : "memory");
}

अटल अंतरभूत व्योम
c_sstatus(uint64 x)
{
  __asm__ __volatile__("csrc sstatus, %0" ::"rK"(x) : "memory");
}

अटल अंतरभूत uint64
rc_sstatus(uint64 x)
{
  __asm__ __volatile__("csrrc %0, sstatus, %1" : "=r"(x) : "rK"(x) : "memory");
  वापस x;
}

// Supervisor Interrupt Pending
अटल अंतरभूत uint64
r_sip()
{
  uint64 x;
  यंत्र अस्थिर("csrr %0, sip" : "=r"(x));
  वापस x;
}

अटल अंतरभूत व्योम
w_sip(uint64 x)
{
  यंत्र अस्थिर("csrw sip, %0" : : "r"(x));
}

// Supervisor Interrupt Enable
#घोषणा SIE_SEIE (1L << 9) // external
#घोषणा SIE_STIE (1L << 5) // timer
अटल अंतरभूत uint64
r_sie()
{
  uint64 x;
  यंत्र अस्थिर("csrr %0, sie" : "=r"(x));
  वापस x;
}

अटल अंतरभूत व्योम
w_sie(uint64 x)
{
  यंत्र अस्थिर("csrw sie, %0" : : "r"(x));
}

// Machine-mode Interrupt Enable
#घोषणा MIE_STIE (1L << 5) // supervisor timer
अटल अंतरभूत uint64
r_mie()
{
  uint64 x;
  यंत्र अस्थिर("csrr %0, mie" : "=r"(x));
  वापस x;
}

अटल अंतरभूत व्योम
w_mie(uint64 x)
{
  यंत्र अस्थिर("csrw mie, %0" : : "r"(x));
}

// supervisor exception program counter, holds the
// instruction address to which a return from
// exception will go.
अटल अंतरभूत व्योम
w_sepc(uint64 x)
{
  यंत्र अस्थिर("csrw sepc, %0" : : "r"(x));
}

अटल अंतरभूत uint64
r_sepc()
{
  uint64 x;
  यंत्र अस्थिर("csrr %0, sepc" : "=r"(x));
  वापस x;
}

// Machine Exception Delegation
अटल अंतरभूत uint64
r_medeleg()
{
  uint64 x;
  यंत्र अस्थिर("csrr %0, medeleg" : "=r"(x));
  वापस x;
}

अटल अंतरभूत व्योम
w_medeleg(uint64 x)
{
  यंत्र अस्थिर("csrw medeleg, %0" : : "r"(x));
}

// Machine Interrupt Delegation
अटल अंतरभूत uint64
r_mideleg()
{
  uint64 x;
  यंत्र अस्थिर("csrr %0, mideleg" : "=r"(x));
  वापस x;
}

अटल अंतरभूत व्योम
w_mideleg(uint64 x)
{
  यंत्र अस्थिर("csrw mideleg, %0" : : "r"(x));
}

// Supervisor Trap-Vector Base Address
// low two bits are mode.
अटल अंतरभूत व्योम
w_stvec(uint64 x)
{
  यंत्र अस्थिर("csrw stvec, %0" : : "r"(x));
}

अटल अंतरभूत uint64
r_stvec()
{
  uint64 x;
  यंत्र अस्थिर("csrr %0, stvec" : "=r"(x));
  वापस x;
}

// Supervisor Timer Comparison Register
अटल अंतरभूत uint64
r_stimecmp()
{
  uint64 x;
  // asm volatile("csrr %0, stimecmp" : "=r" (x) );
  यंत्र अस्थिर("csrr %0, 0x14d" : "=r"(x));
  वापस x;
}

अटल अंतरभूत व्योम
w_stimecmp(uint64 x)
{
  // asm volatile("csrw stimecmp, %0" : : "r" (x));
  यंत्र अस्थिर("csrw 0x14d, %0" : : "r"(x));
}

// Machine Environment Configuration Register

#घोषणा MENVCFG_STCE (1L << 63)
#घोषणा MENVCFG_ADUE (1L << 61)

अटल अंतरभूत uint64
r_menvcfg()
{
  uint64 x;
  // asm volatile("csrr %0, menvcfg" : "=r" (x) );
  यंत्र अस्थिर("csrr %0, 0x30a" : "=r"(x));
  वापस x;
}

अटल अंतरभूत व्योम
w_menvcfg(uint64 x)
{
  // asm volatile("csrw menvcfg, %0" : : "r" (x));
  यंत्र अस्थिर("csrw 0x30a, %0" : : "r"(x));
}

// Physical Memory Protection
अटल अंतरभूत व्योम
w_pmpcfg0(uint64 x)
{
  यंत्र अस्थिर("csrw pmpcfg0, %0" : : "r"(x));
}

अटल अंतरभूत व्योम
w_pmpaddr0(uint64 x)
{
  यंत्र अस्थिर("csrw pmpaddr0, %0" : : "r"(x));
}

// use riscv's sv39 page table scheme.
#घोषणा SATP_SV39 (8L << 60)

#घोषणा MAKE_SATP(pagetable) (SATP_SV39 | (((uint64)pagetable) >> 12))

// supervisor address translation and protection;
// holds the address of the page table.
अटल अंतरभूत व्योम
w_satp(uint64 x)
{
  यंत्र अस्थिर("csrw satp, %0" : : "r"(x));
}

अटल अंतरभूत uint64
r_satp()
{
  uint64 x;
  यंत्र अस्थिर("csrr %0, satp" : "=r"(x));
  वापस x;
}

// Supervisor Trap Cause
अटल अंतरभूत uint64
r_scause()
{
  uint64 x;
  यंत्र अस्थिर("csrr %0, scause" : "=r"(x));
  वापस x;
}

// Supervisor Trap Value
अटल अंतरभूत uint64
r_stval()
{
  uint64 x;
  यंत्र अस्थिर("csrr %0, stval" : "=r"(x));
  वापस x;
}

// Machine-mode Counter-Enable
अटल अंतरभूत व्योम
w_mcounteren(uint64 x)
{
  यंत्र अस्थिर("csrw mcounteren, %0" : : "r"(x));
}

अटल अंतरभूत uint64
r_mcounteren()
{
  uint64 x;
  यंत्र अस्थिर("csrr %0, mcounteren" : "=r"(x));
  वापस x;
}

// machine-mode cycle counter
अटल अंतरभूत uint64
r_time()
{
  uint64 x;
  यंत्र अस्थिर("csrr %0, time" : "=r"(x));
  वापस x;
}

// enable device interrupts
अटल अंतरभूत व्योम
intr_on()
{
  s_sstatus(SSTATUS_SIE);
}

// disable device interrupts
अटल अंतरभूत व्योम
intr_off()
{
  c_sstatus(SSTATUS_SIE);
}

// are device interrupts enabled?
अटल अंतरभूत पूर्णांक
intr_get()
{
  uint64 x = r_sstatus();
  वापस (x & SSTATUS_SIE) != 0;
}

अटल अंतरभूत uint64
r_sp()
{
  uint64 x;
  यंत्र अस्थिर("mv %0, sp" : "=r"(x));
  वापस x;
}

// read and write tp, the thread pointer, which xv6 uses to hold
// this core's hartid (core number), the index into cpus[].
अटल अंतरभूत uint64
r_tp()
{
  uint64 x;
  यंत्र अस्थिर("mv %0, tp" : "=r"(x));
  वापस x;
}

अटल अंतरभूत व्योम
w_tp(uint64 x)
{
  यंत्र अस्थिर("mv tp, %0" : : "r"(x));
}

अटल अंतरभूत uint64
r_ra()
{
  uint64 x;
  यंत्र अस्थिर("mv %0, ra" : "=r"(x));
  वापस x;
}

// flush the TLB.
अटल अंतरभूत व्योम
sfence_vma()
{
  // the zero, zero means flush all TLB entries.
  यंत्र अस्थिर("sfence.vma zero, zero" ::: "memory");
}

// fence for memory-mapped IO
अटल अंतरभूत व्योम
io_fence()
{
  यंत्र अस्थिर("fence iorw, iorw" ::: "memory");
}

// fence for icache
अटल अंतरभूत व्योम
icache_fence()
{
  यंत्र अस्थिर("fence.i" ::: "memory");
}

प्रकार uint64 pte_t;
प्रकार uint64 *pagetable_t; // 512 PTEs

#पूर्ण_अगर // __ASSEMBLER__

#घोषणा PGSIZE  4096 // bytes per page
#घोषणा PGSHIFT 12   // bits of offset within a page

#घोषणा PGROUNDUP(sz)  (((sz) + PGSIZE - 1) & ~(PGSIZE - 1))
#घोषणा PGROUNDDOWN(a) (((a)) & ~(PGSIZE - 1))

#घोषणा PTE_V (1L << 0) // valid
#घोषणा PTE_R (1L << 1)
#घोषणा PTE_W (1L << 2)
#घोषणा PTE_X (1L << 3)
#घोषणा PTE_U (1L << 4) // user can access

// shift a physical address to the right place for a PTE.
#घोषणा PA2PTE(pa) ((((uint64)pa) >> 12) << 10)

#घोषणा PTE2PA(pte) (((pte) >> 10) << 12)

#घोषणा PTE_FLAGS(pte) ((pte) & 0x3FF)

// extract the three 9-bit page table indices from a virtual address.
#घोषणा PXMASK         0x1FF // 9 bits
#घोषणा PXSHIFT(level) (PGSHIFT + (9 * (level)))
#घोषणा PX(level, va)  ((((uint64)(va)) >> PXSHIFT(level)) & PXMASK)

// one beyond the highest possible virtual address.
// MAXVA is actually one bit less than the max allowed by
// Sv39, to avoid having to sign-extend virtual addresses
// that have the high bit set.
#घोषणा MAXVA (1L << (9 + 9 + 9 + 12 - 1))
