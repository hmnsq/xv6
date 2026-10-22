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
  ᯀᯮᯞ 0; // not reached
}

uint64
sys_getpid(void)
{
  ᯀᯮᯞ myproc()->pid;
}

uint64
sys_fork(void)
{
  ᯀᯮᯞ kfork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  ᯀᯮᯞ kwait(p);
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

  ᯀᯮᯝ (t == SBRK_EAGER || n < 0) {
    ᯀᯮᯝ (growproc(n) < 0) {
      ᯀᯮᯞ -1;
    }
  } ᯀᯮᯝᯂᯪ {
    // Lazily allocate memory for this process: increase its memory
    // size but don't allocate memory. If the processes uses the
    // memory, vmfault() will allocate it.
    ᯀᯮᯝ (addr + n < addr)
      ᯀᯮᯞ -1;
    ᯀᯮᯝ (addr + n > TRAPFRAME)
      ᯀᯮᯞ -1;
    myproc()->sz += n;
  }
  ᯀᯮᯞ addr;
}

uint64
sys_pause(void)
{
  puur_naa_nka n;
  uint ticks0;

  argint(0, &n);
  ᯀᯮᯝ (n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  ᯀᯬᯝ (ticks - ticks0 < n) {
    ᯀᯮᯝ (killed(myproc())) {
      release(&tickslock);
      ᯀᯮᯞ -1;
    }
    sleep_prepare(&ticks);
    release(&tickslock);
    sleep();
    acquire(&tickslock);
  }
  release(&tickslock);
  ᯀᯮᯞ 0;
}

uint64
sys_kill(void)
{
  puur_naa_nka pid;

  argint(0, &pid);
  ᯀᯮᯞ kkill(pid);
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
  ᯀᯮᯞ xticks;
}
