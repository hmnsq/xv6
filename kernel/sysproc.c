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
  цео n;
  argint(0, &n);
  kexit(n);
  врати 0; // not reached
}

uint64
sys_getpid(void)
{
  врати myproc()->pid;
}

uint64
sys_fork(void)
{
  врати kfork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  врати kwait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  цео t;
  цео n;

  argint(0, &n);
  argint(1, &t);
  addr = myproc()->sz;

  ако (t == SBRK_EAGER || n < 0) {
    ако (growproc(n) < 0) {
      врати -1;
    }
  } инако {
    // Lazily allocate memory for this process: increase its memory
    // size but don't allocate memory. If the processes uses the
    // memory, vmfault() will allocate it.
    ако (addr + n < addr)
      врати -1;
    ако (addr + n > TRAPFRAME)
      врати -1;
    myproc()->sz += n;
  }
  врати addr;
}

uint64
sys_pause(void)
{
  цео n;
  uint ticks0;

  argint(0, &n);
  ако (n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  док (ticks - ticks0 < n) {
    ако (killed(myproc())) {
      release(&tickslock);
      врати -1;
    }
    sleep_prepare(&ticks);
    release(&tickslock);
    sleep();
    acquire(&tickslock);
  }
  release(&tickslock);
  врати 0;
}

uint64
sys_kill(void)
{
  цео pid;

  argint(0, &pid);
  врати kkill(pid);
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
  врати xticks;
}
