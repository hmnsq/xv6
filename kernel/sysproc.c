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
  số_nguyên n;
  argint(0, &n);
  kexit(n);
  trả_về 0; // not reached
}

uint64
sys_getpid(void)
{
  trả_về myproc()->pid;
}

uint64
sys_fork(void)
{
  trả_về kfork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  trả_về kwait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  số_nguyên t;
  số_nguyên n;

  argint(0, &n);
  argint(1, &t);
  addr = myproc()->sz;

  nếu (t == SBRK_EAGER || n < 0) {
    nếu (growproc(n) < 0) {
      trả_về -1;
    }
  } không_thì {
    // Lazily allocate memory for this process: increase its memory
    // size but don't allocate memory. If the processes uses the
    // memory, vmfault() will allocate it.
    nếu (addr + n < addr)
      trả_về -1;
    nếu (addr + n > TRAPFRAME)
      trả_về -1;
    myproc()->sz += n;
  }
  trả_về addr;
}

uint64
sys_pause(void)
{
  số_nguyên n;
  uint ticks0;

  argint(0, &n);
  nếu (n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  trong_khi (ticks - ticks0 < n) {
    nếu (killed(myproc())) {
      release(&tickslock);
      trả_về -1;
    }
    sleep_prepare(&ticks);
    release(&tickslock);
    sleep();
    acquire(&tickslock);
  }
  release(&tickslock);
  trả_về 0;
}

uint64
sys_kill(void)
{
  số_nguyên pid;

  argint(0, &pid);
  trả_về kkill(pid);
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
  trả_về xticks;
}
