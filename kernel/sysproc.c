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
  ᤀᤡᤴ 0; // not reached
}

uint64
sys_getpid(void)
{
  ᤀᤡᤴ myproc()->pid;
}

uint64
sys_fork(void)
{
  ᤀᤡᤴ kfork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  ᤀᤡᤴ kwait(p);
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

  ᤀᤡ᤺ (t == SBRK_EAGER || n < 0) {
    ᤀᤡ᤺ (growproc(n) < 0) {
      ᤀᤡᤴ -1;
    }
  } ᤀᤡ᤺ᤆᤡ {
    // Lazily allocate memory for this process: increase its memory
    // size but don't allocate memory. If the processes uses the
    // memory, vmfault() will allocate it.
    ᤀᤡ᤺ (addr + n < addr)
      ᤀᤡᤴ -1;
    ᤀᤡ᤺ (addr + n > TRAPFRAME)
      ᤀᤡᤴ -1;
    myproc()->sz += n;
  }
  ᤀᤡᤴ addr;
}

uint64
sys_pause(void)
{
  puur_naa_nka n;
  uint ticks0;

  argint(0, &n);
  ᤀᤡ᤺ (n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  ᤀᤠ᤺ (ticks - ticks0 < n) {
    ᤀᤡ᤺ (killed(myproc())) {
      release(&tickslock);
      ᤀᤡᤴ -1;
    }
    sleep_prepare(&ticks);
    release(&tickslock);
    sleep();
    acquire(&tickslock);
  }
  release(&tickslock);
  ᤀᤡᤴ 0;
}

uint64
sys_kill(void)
{
  puur_naa_nka pid;

  argint(0, &pid);
  ᤀᤡᤴ kkill(pid);
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
  ᤀᤡᤴ xticks;
}
