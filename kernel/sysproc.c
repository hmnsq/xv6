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
  පූර්ණ_සංඛ්‍යා n;
  argint(0, &n);
  kexit(n);
  ආපසු 0; // not reached
}

uint64
sys_getpid(void)
{
  ආපසු myproc()->pid;
}

uint64
sys_fork(void)
{
  ආපසු kfork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  ආපසු kwait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  පූර්ණ_සංඛ්‍යා t;
  පූර්ණ_සංඛ්‍යා n;

  argint(0, &n);
  argint(1, &t);
  addr = myproc()->sz;

  නම් (t == SBRK_EAGER || n < 0) {
    නම් (growproc(n) < 0) {
      ආපසු -1;
    }
  } නැතහොත් {
    // Lazily allocate memory for this process: increase its memory
    // size but don't allocate memory. If the processes uses the
    // memory, vmfault() will allocate it.
    නම් (addr + n < addr)
      ආපසු -1;
    නම් (addr + n > TRAPFRAME)
      ආපසු -1;
    myproc()->sz += n;
  }
  ආපසු addr;
}

uint64
sys_pause(void)
{
  පූර්ණ_සංඛ්‍යා n;
  uint ticks0;

  argint(0, &n);
  නම් (n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  අතරතුර (ticks - ticks0 < n) {
    නම් (killed(myproc())) {
      release(&tickslock);
      ආපසු -1;
    }
    sleep_prepare(&ticks);
    release(&tickslock);
    sleep();
    acquire(&tickslock);
  }
  release(&tickslock);
  ආපසු 0;
}

uint64
sys_kill(void)
{
  පූර්ණ_සංඛ්‍යා pid;

  argint(0, &pid);
  ආපසු kkill(pid);
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
  ආපසු xticks;
}
