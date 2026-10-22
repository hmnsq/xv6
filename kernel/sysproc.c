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
  整数 n;
  argint(0, &n);
  kexit(n);
  返回 0; // not reached
}

uint64
sys_getpid(void)
{
  返回 myproc()->pid;
}

uint64
sys_fork(void)
{
  返回 kfork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  返回 kwait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  整数 t;
  整数 n;

  argint(0, &n);
  argint(1, &t);
  addr = myproc()->sz;

  如果 (t == SBRK_EAGER || n < 0) {
    如果 (growproc(n) < 0) {
      返回 -1;
    }
  } 否则 {
    // Lazily allocate memory for this process: increase its memory
    // size but don't allocate memory. If the processes uses the
    // memory, vmfault() will allocate it.
    如果 (addr + n < addr)
      返回 -1;
    如果 (addr + n > TRAPFRAME)
      返回 -1;
    myproc()->sz += n;
  }
  返回 addr;
}

uint64
sys_pause(void)
{
  整数 n;
  uint ticks0;

  argint(0, &n);
  如果 (n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  当 (ticks - ticks0 < n) {
    如果 (killed(myproc())) {
      release(&tickslock);
      返回 -1;
    }
    sleep_prepare(&ticks);
    release(&tickslock);
    sleep();
    acquire(&tickslock);
  }
  release(&tickslock);
  返回 0;
}

uint64
sys_kill(void)
{
  整数 pid;

  argint(0, &pid);
  返回 kkill(pid);
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
  返回 xticks;
}
