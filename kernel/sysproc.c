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
  ꦮꦭꦶ 0; // not reached
}

uint64
sys_getpid(void)
{
  ꦮꦭꦶ myproc()->pid;
}

uint64
sys_fork(void)
{
  ꦮꦭꦶ kfork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  ꦮꦭꦶ kwait(p);
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

  ꦪꦼꦤ꧀ (t == SBRK_EAGER || n < 0) {
    ꦪꦼꦤ꧀ (growproc(n) < 0) {
      ꦮꦭꦶ -1;
    }
  } ꦭꦶꦪ {
    // Lazily allocate memory for this process: increase its memory
    // size but don't allocate memory. If the processes uses the
    // memory, vmfault() will allocate it.
    ꦪꦼꦤ꧀ (addr + n < addr)
      ꦮꦭꦶ -1;
    ꦪꦼꦤ꧀ (addr + n > TRAPFRAME)
      ꦮꦭꦶ -1;
    myproc()->sz += n;
  }
  ꦮꦭꦶ addr;
}

uint64
sys_pause(void)
{
  puur_naa_nka n;
  uint ticks0;

  argint(0, &n);
  ꦪꦼꦤ꧀ (n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  ꦱꦼꦢꦼꦭ (ticks - ticks0 < n) {
    ꦪꦼꦤ꧀ (killed(myproc())) {
      release(&tickslock);
      ꦮꦭꦶ -1;
    }
    sleep_prepare(&ticks);
    release(&tickslock);
    sleep();
    acquire(&tickslock);
  }
  release(&tickslock);
  ꦮꦭꦶ 0;
}

uint64
sys_kill(void)
{
  puur_naa_nka pid;

  argint(0, &pid);
  ꦮꦭꦶ kkill(pid);
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
  ꦮꦭꦶ xticks;
}
