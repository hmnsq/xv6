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
  целое n;
  argint(0, &n);
  kexit(n);
  вернуть 0; // not reached
}

uint64
sys_getpid(void)
{
  вернуть myproc()->pid;
}

uint64
sys_fork(void)
{
  вернуть kfork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  вернуть kwait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  целое t;
  целое n;

  argint(0, &n);
  argint(1, &t);
  addr = myproc()->sz;

  если (t == SBRK_EAGER || n < 0) {
    если (growproc(n) < 0) {
      вернуть -1;
    }
  } иначе {
    // Lazily allocate memory for this process: increase its memory
    // size but don't allocate memory. If the processes uses the
    // memory, vmfault() will allocate it.
    если (addr + n < addr)
      вернуть -1;
    если (addr + n > TRAPFRAME)
      вернуть -1;
    myproc()->sz += n;
  }
  вернуть addr;
}

uint64
sys_pause(void)
{
  целое n;
  uint ticks0;

  argint(0, &n);
  если (n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  пока (ticks - ticks0 < n) {
    если (killed(myproc())) {
      release(&tickslock);
      вернуть -1;
    }
    sleep_prepare(&ticks);
    release(&tickslock);
    sleep();
    acquire(&tickslock);
  }
  release(&tickslock);
  вернуть 0;
}

uint64
sys_kill(void)
{
  целое pid;

  argint(0, &pid);
  вернуть kkill(pid);
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
  вернуть xticks;
}
