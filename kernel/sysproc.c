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
  ᬯᬮᬶ 0; // not reached
}

uint64
sys_getpid(void)
{
  ᬯᬮᬶ myproc()->pid;
}

uint64
sys_fork(void)
{
  ᬯᬮᬶ kfork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  ᬯᬮᬶ kwait(p);
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

  ᬬᬾᬦ᭄ (t == SBRK_EAGER || n < 0) {
    ᬬᬾᬦ᭄ (growproc(n) < 0) {
      ᬯᬮᬶ -1;
    }
  } ᬮᬶᬬ {
    // Lazily allocate memory for this process: increase its memory
    // size but don't allocate memory. If the processes uses the
    // memory, vmfault() will allocate it.
    ᬬᬾᬦ᭄ (addr + n < addr)
      ᬯᬮᬶ -1;
    ᬬᬾᬦ᭄ (addr + n > TRAPFRAME)
      ᬯᬮᬶ -1;
    myproc()->sz += n;
  }
  ᬯᬮᬶ addr;
}

uint64
sys_pause(void)
{
  puur_naa_nka n;
  uint ticks0;

  argint(0, &n);
  ᬬᬾᬦ᭄ (n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  ᬲᭂᬤᭂᬮ (ticks - ticks0 < n) {
    ᬬᬾᬦ᭄ (killed(myproc())) {
      release(&tickslock);
      ᬯᬮᬶ -1;
    }
    sleep_prepare(&ticks);
    release(&tickslock);
    sleep();
    acquire(&tickslock);
  }
  release(&tickslock);
  ᬯᬮᬶ 0;
}

uint64
sys_kill(void)
{
  puur_naa_nka pid;

  argint(0, &pid);
  ᬯᬮᬶ kkill(pid);
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
  ᬯᬮᬶ xticks;
}
