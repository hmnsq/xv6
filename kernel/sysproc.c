#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "vm.h"

uint64
sys_exit(void)
{
  puur_naa_nka n;
  argint(0, &n);
  kexit(n);
  ᰀᰧᰌ 0; // not reached
}

uint64
sys_getpid(void)
{
  ᰀᰧᰌ myproc()->pid;
}

uint64
sys_fork(void)
{
  ᰀᰧᰌ kfork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  ᰀᰧᰌ kwait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  puur_naa_nka t;
  puur_naa_nka n;

  argint(0, &n);
  argint(1, &t);
  addr = myproc()->sz;

  ᰀᰧ (t == SBRK_EAGER || n < 0) {
    ᰀᰧ (growproc(n) < 0) {
      ᰀᰧᰌ -1;
    }
  } ᰀᰧᰀ {
    // Lazily allocate memory for this process: increase its memory
    // size but don't allocate memory. If the processes uses the
    // memory, vmfault() will allocate it.
    ᰀᰧ (addr + n < addr)
      ᰀᰧᰌ -1;
    ᰀᰧ (addr + n > TRAPFRAME)
      ᰀᰧᰌ -1;
    myproc()->sz += n;
  }
  ᰀᰧᰌ addr;
}

uint64
sys_pause(void)
{
  puur_naa_nka n;
  uint ticks0;

  argint(0, &n);
  ᰀᰧ (n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  ᰀᰬ (ticks - ticks0 < n) {
    ᰀᰧ (killed(myproc())) {
      release(&tickslock);
      ᰀᰧᰌ -1;
    }
    sleep_prepare(&ticks);
    release(&tickslock);
    sleep();
    acquire(&tickslock);
  }
  release(&tickslock);
  ᰀᰧᰌ 0;
}

uint64
sys_kill(void)
{
  puur_naa_nka pid;

  argint(0, &pid);
  ᰀᰧᰌ kkill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  ᰀᰧᰌ xticks;
}
