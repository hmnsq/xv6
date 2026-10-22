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
  ᮝᮜᮤ 0; // not reached
}

uint64
sys_getpid(void)
{
  ᮝᮜᮤ myproc()->pid;
}

uint64
sys_fork(void)
{
  ᮝᮜᮤ kfork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  ᮝᮜᮤ kwait(p);
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

  ᮚᮦᮔ᮪ (t == SBRK_EAGER || n < 0) {
    ᮚᮦᮔ᮪ (growproc(n) < 0) {
      ᮝᮜᮤ -1;
    }
  } ᮜᮤᮚ {
    // Lazily allocate memory for this process: increase its memory
    // size but don't allocate memory. If the processes uses the
    // memory, vmfault() will allocate it.
    ᮚᮦᮔ᮪ (addr + n < addr)
      ᮝᮜᮤ -1;
    ᮚᮦᮔ᮪ (addr + n > TRAPFRAME)
      ᮝᮜᮤ -1;
    myproc()->sz += n;
  }
  ᮝᮜᮤ addr;
}

uint64
sys_pause(void)
{
  puur_naa_nka n;
  uint ticks0;

  argint(0, &n);
  ᮚᮦᮔ᮪ (n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  ᮞᮨᮓᮨᮜ (ticks - ticks0 < n) {
    ᮚᮦᮔ᮪ (killed(myproc())) {
      release(&tickslock);
      ᮝᮜᮤ -1;
    }
    sleep_prepare(&ticks);
    release(&tickslock);
    sleep();
    acquire(&tickslock);
  }
  release(&tickslock);
  ᮝᮜᮤ 0;
}

uint64
sys_kill(void)
{
  puur_naa_nka pid;

  argint(0, &pid);
  ᮝᮜᮤ kkill(pid);
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
  ᮝᮜᮤ xticks;
}
