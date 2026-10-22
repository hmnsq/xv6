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
  ціле n;
  argint(0, &n);
  kexit(n);
  повернути 0; // not reached
}

uint64
sys_getpid(void)
{
  повернути myproc()->pid;
}

uint64
sys_fork(void)
{
  повернути kfork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  повернути kwait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  ціле t;
  ціле n;

  argint(0, &n);
  argint(1, &t);
  addr = myproc()->sz;

  якщо (t == SBRK_EAGER || n < 0) {
    якщо (growproc(n) < 0) {
      повернути -1;
    }
  } інакше {
    // Lazily allocate memory for this process: increase its memory
    // size but don't allocate memory. If the processes uses the
    // memory, vmfault() will allocate it.
    якщо (addr + n < addr)
      повернути -1;
    якщо (addr + n > TRAPFRAME)
      повернути -1;
    myproc()->sz += n;
  }
  повернути addr;
}

uint64
sys_pause(void)
{
  ціле n;
  uint ticks0;

  argint(0, &n);
  якщо (n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  поки (ticks - ticks0 < n) {
    якщо (killed(myproc())) {
      release(&tickslock);
      повернути -1;
    }
    sleep_prepare(&ticks);
    release(&tickslock);
    sleep();
    acquire(&tickslock);
  }
  release(&tickslock);
  повернути 0;
}

uint64
sys_kill(void)
{
  ціле pid;

  argint(0, &pid);
  повернути kkill(pid);
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
  повернути xticks;
}
