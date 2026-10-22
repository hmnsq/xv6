#شامل "types.h"
#شامل "param.h"
#شامل "memlayout.h"
#شامل "riscv.h"
#شامل "defs.h"

خالی مین();
خالی timerinit();

// entry.S needs one stack per CPU.
__attribute__((aligned(16))) حرف stack0[4096 * NCPU];

// entry.S jumps here in machine mode on stack0.
خالی
آغاز()
{
  // set M Previous Privilege mode to Supervisor, for mret.
  غیرعلامتی طویل x = r_mstatus();
  x &= ~MSTATUS_MPP_نقاب;
  x |= MSTATUS_MPP_S;
  w_mstatus(x);

  // set M Exception Program Counter to main, for mret.
  // requires gcc -mcmodel=medany
  w_mepc((uint64)مین);

  // disable paging for now.
  w_satp(0);

  // delegate all interrupts and exceptions to supervisor mode.
  w_medeleg(0xffff);
  w_mideleg(0xffff);
  w_sie(r_sie() | SIE_SEIE | SIE_STIE);

  // configure Physical Memory Protection to give supervisor mode
  // access to all of physical memory.
  w_pmpaddr0(0x3fffffffffffffull);
  w_pmpcfg0(0xf);

  // enable hardware updates of page table A and D bits
  w_menvcfg(r_menvcfg() | MENVCFG_ADUE);

  // ask for clock interrupts.
  timerinit();

  // keep each CPU's hartid in its tp register, for cpuid().
  صحیح id = r_mhartid();
  w_tp(id);

  // switch to supervisor mode and jump to main().
  asm متلون("mret");
}

// ask each hart to generate timer interrupts.
خالی
timerinit()
{
  // enable the sstc extension (i.e. stimecmp).
  w_menvcfg(r_menvcfg() | MENVCFG_STCE);

  // allow supervisor to use stimecmp and time.
  w_mcounteren(r_mcounteren() | 2);

  // ask for the very first timer interrupt.
  w_stimecmp(r_وقت() + 1000000);
}
