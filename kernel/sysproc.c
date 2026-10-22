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
  정수 n;
  argint(0, &n);
  kexit(n);
  반환 0; // not reached
}

uint64
sys_getpid(void)
{
  반환 myproc()->pid;
}

uint64
sys_fork(void)
{
  반환 kfork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  반환 kwait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  정수 t;
  정수 n;

  argint(0, &n);
  argint(1, &t);
  addr = myproc()->sz;

  만약 (t == SBRK_EAGER || n < 0) {
    만약 (growproc(n) < 0) {
      반환 -1;
    }
  } 아니면 {
    // Lazily allocate memory for this process: increase its memory
    // size but don't allocate memory. If the processes uses the
    // memory, vmfault() will allocate it.
    만약 (addr + n < addr)
      반환 -1;
    만약 (addr + n > TRAPFRAME)
      반환 -1;
    myproc()->sz += n;
  }
  반환 addr;
}

uint64
sys_pause(void)
{
  정수 n;
  uint ticks0;

  argint(0, &n);
  만약 (n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  동안 (ticks - ticks0 < n) {
    만약 (killed(myproc())) {
      release(&tickslock);
      반환 -1;
    }
    sleep_prepare(&ticks);
    release(&tickslock);
    sleep();
    acquire(&tickslock);
  }
  release(&tickslock);
  반환 0;
}

uint64
sys_kill(void)
{
  정수 pid;

  argint(0, &pid);
  반환 kkill(pid);
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
  반환 xticks;
}
