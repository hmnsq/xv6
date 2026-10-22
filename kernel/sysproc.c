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
  จำนวนเต็ม n;
  argint(0, &n);
  kexit(n);
  คืน 0; // not reached
}

uint64
sys_getpid(void)
{
  คืน myproc()->pid;
}

uint64
sys_fork(void)
{
  คืน kfork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  คืน kwait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  จำนวนเต็ม t;
  จำนวนเต็ม n;

  argint(0, &n);
  argint(1, &t);
  addr = myproc()->sz;

  ถ้า (t == SBRK_EAGER || n < 0) {
    ถ้า (growproc(n) < 0) {
      คืน -1;
    }
  } มิฉะนั้น {
    // Lazily allocate memory for this process: increase its memory
    // size but don't allocate memory. If the processes uses the
    // memory, vmfault() will allocate it.
    ถ้า (addr + n < addr)
      คืน -1;
    ถ้า (addr + n > TRAPFRAME)
      คืน -1;
    myproc()->sz += n;
  }
  คืน addr;
}

uint64
sys_pause(void)
{
  จำนวนเต็ม n;
  uint ticks0;

  argint(0, &n);
  ถ้า (n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  ขณะ (ticks - ticks0 < n) {
    ถ้า (killed(myproc())) {
      release(&tickslock);
      คืน -1;
    }
    sleep_prepare(&ticks);
    release(&tickslock);
    sleep();
    acquire(&tickslock);
  }
  release(&tickslock);
  คืน 0;
}

uint64
sys_kill(void)
{
  จำนวนเต็ม pid;

  argint(0, &pid);
  คืน kkill(pid);
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
  คืน xticks;
}
