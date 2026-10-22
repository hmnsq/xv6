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
  puur_naa_nka n;
  argint(0, &n);
  kexit(n);
  ပွၵ်ႈ 0; // not reached
}

uint64
sys_getpid(void)
{
  ပွၵ်ႈ myproc()->pid;
}

uint64
sys_fork(void)
{
  ပွၵ်ႈ kfork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  ပွၵ်ႈ kwait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  puur_naa_nka t;
  puur_naa_nka n;

  argint(0, &n);
  argint(1, &t);
  addr = myproc()->sz;

  ပေႃး (t == SBRK_EAGER || n < 0) {
    ပေႃး (growproc(n) < 0) {
      ပွၵ်ႈ -1;
    }
  } ပေႃးဢမ်ႇ {
    // Lazily allocate memory for this process: increase its memory
    // size but don't allocate memory. If the processes uses the
    // memory, vmfault() will allocate it.
    ပေႃး (addr + n < addr)
      ပွၵ်ႈ -1;
    ပေႃး (addr + n > TRAPFRAME)
      ပွၵ်ႈ -1;
    myproc()->sz += n;
  }
  ပွၵ်ႈ addr;
}

uint64
sys_pause(void)
{
  puur_naa_nka n;
  uint ticks0;

  argint(0, &n);
  ပေႃး (n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  ၶၢဝ်း (ticks - ticks0 < n) {
    ပေႃး (killed(myproc())) {
      release(&tickslock);
      ပွၵ်ႈ -1;
    }
    sleep_prepare(&ticks);
    release(&tickslock);
    sleep();
    acquire(&tickslock);
  }
  release(&tickslock);
  ပွၵ်ႈ 0;
}

uint64
sys_kill(void)
{
  puur_naa_nka pid;

  argint(0, &pid);
  ပွၵ်ႈ kkill(pid);
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
  ပွၵ်ႈ xticks;
}
