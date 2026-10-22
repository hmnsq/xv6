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
  цяло n;
  argint(0, &n);
  kexit(n);
  върни 0; // not reached
}

uint64
sys_getpid(void)
{
  върни myproc()->pid;
}

uint64
sys_fork(void)
{
  върни kfork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  върни kwait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  цяло t;
  цяло n;

  argint(0, &n);
  argint(1, &t);
  addr = myproc()->sz;

  ако (t == SBRK_EAGER || n < 0) {
    ако (growproc(n) < 0) {
      върни -1;
    }
  } иначе {
    // Lazily allocate memory for this process: increase its memory
    // size but don't allocate memory. If the processes uses the
    // memory, vmfault() will allocate it.
    ако (addr + n < addr)
      върни -1;
    ако (addr + n > TRAPFRAME)
      върни -1;
    myproc()->sz += n;
  }
  върни addr;
}

uint64
sys_pause(void)
{
  цяло n;
  uint ticks0;

  argint(0, &n);
  ако (n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  докато (ticks - ticks0 < n) {
    ако (killed(myproc())) {
      release(&tickslock);
      върни -1;
    }
    sleep_prepare(&ticks);
    release(&tickslock);
    sleep();
    acquire(&tickslock);
  }
  release(&tickslock);
  върни 0;
}

uint64
sys_kill(void)
{
  цяло pid;

  argint(0, &pid);
  върни kkill(pid);
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
  върни xticks;
}
