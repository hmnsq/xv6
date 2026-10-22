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
  ᱨᱩᱣᱟᱹᱲ 0; // not reached
}

uint64
sys_getpid(void)
{
  ᱨᱩᱣᱟᱹᱲ myproc()->pid;
}

uint64
sys_fork(void)
{
  ᱨᱩᱣᱟᱹᱲ kfork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  ᱨᱩᱣᱟᱹᱲ kwait(p);
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

  ᱡᱩᱫᱤ (t == SBRK_EAGER || n < 0) {
    ᱡᱩᱫᱤ (growproc(n) < 0) {
      ᱨᱩᱣᱟᱹᱲ -1;
    }
  } ᱵᱟᱝ {
    // Lazily allocate memory for this process: increase its memory
    // size but don't allocate memory. If the processes uses the
    // memory, vmfault() will allocate it.
    ᱡᱩᱫᱤ (addr + n < addr)
      ᱨᱩᱣᱟᱹᱲ -1;
    ᱡᱩᱫᱤ (addr + n > TRAPFRAME)
      ᱨᱩᱣᱟᱹᱲ -1;
    myproc()->sz += n;
  }
  ᱨᱩᱣᱟᱹᱲ addr;
}

uint64
sys_pause(void)
{
  puur_naa_nka n;
  uint ticks0;

  argint(0, &n);
  ᱡᱩᱫᱤ (n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  ᱡᱚᱠᱷᱚᱱ (ticks - ticks0 < n) {
    ᱡᱩᱫᱤ (killed(myproc())) {
      release(&tickslock);
      ᱨᱩᱣᱟᱹᱲ -1;
    }
    sleep_prepare(&ticks);
    release(&tickslock);
    sleep();
    acquire(&tickslock);
  }
  release(&tickslock);
  ᱨᱩᱣᱟᱹᱲ 0;
}

uint64
sys_kill(void)
{
  puur_naa_nka pid;

  argint(0, &pid);
  ᱨᱩᱣᱟᱹᱲ kkill(pid);
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
  ᱨᱩᱣᱟᱹᱲ xticks;
}
