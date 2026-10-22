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
  ចំនួនគត់ n;
  argint(0, &n);
  kexit(n);
  ត្រឡប់ 0; // not reached
}

uint64
sys_getpid(void)
{
  ត្រឡប់ myproc()->pid;
}

uint64
sys_fork(void)
{
  ត្រឡប់ kfork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  ត្រឡប់ kwait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  ចំនួនគត់ t;
  ចំនួនគត់ n;

  argint(0, &n);
  argint(1, &t);
  addr = myproc()->sz;

  ប្រសិនបើ (t == SBRK_EAGER || n < 0) {
    ប្រសិនបើ (growproc(n) < 0) {
      ត្រឡប់ -1;
    }
  } បើមិនដូច្នេះ {
    // Lazily allocate memory for this process: increase its memory
    // size but don't allocate memory. If the processes uses the
    // memory, vmfault() will allocate it.
    ប្រសិនបើ (addr + n < addr)
      ត្រឡប់ -1;
    ប្រសិនបើ (addr + n > TRAPFRAME)
      ត្រឡប់ -1;
    myproc()->sz += n;
  }
  ត្រឡប់ addr;
}

uint64
sys_pause(void)
{
  ចំនួនគត់ n;
  uint ticks0;

  argint(0, &n);
  ប្រសិនបើ (n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  ខណៈពេល (ticks - ticks0 < n) {
    ប្រសិនបើ (killed(myproc())) {
      release(&tickslock);
      ត្រឡប់ -1;
    }
    sleep_prepare(&ticks);
    release(&tickslock);
    sleep();
    acquire(&tickslock);
  }
  release(&tickslock);
  ត្រឡប់ 0;
}

uint64
sys_kill(void)
{
  ចំនួនគត់ pid;

  argint(0, &pid);
  ត្រឡប់ kkill(pid);
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
  ត្រឡប់ xticks;
}
