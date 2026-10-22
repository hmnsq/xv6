#అన్తర్భావ "types.h"
#అన్తర్భావ "riscv.h"
#అన్తర్భావ "defs.h"
#అన్తర్భావ "param.h"
#అన్తర్భావ "memlayout.h"
#అన్తర్భావ "spinlock.h"
#అన్తర్భావ "proc.h"
#అన్తర్భావ "vm.h"

uint64
sys_exit(శూన్య)
{
  పూర్ణాంకం n;
  argint(0, &n);
  kexit(n);
  తిరిగి 0; // not reached
}

uint64
sys_getpid(శూన్య)
{
  తిరిగి myproc()->pid;
}

uint64
sys_fork(శూన్య)
{
  తిరిగి kfork();
}

uint64
sys_wait(శూన్య)
{
  uint64 p;
  argaddr(0, &p);
  తిరిగి kwait(p);
}

uint64
sys_sbrk(శూన్య)
{
  uint64 addr;
  పూర్ణాంకం t;
  పూర్ణాంకం n;

  argint(0, &n);
  argint(1, &t);
  addr = myproc()->sz;

  ఒకవేళ (t == SBRK_EAGER || n < 0) {
    ఒకవేళ (growproc(n) < 0) {
      తిరిగి -1;
    }
  } లేకపోతే {
    // Lazily allocate memory for this process: increase its memory
    // size but don't allocate memory. If the processes uses the
    // memory, vmfault() will allocate it.
    ఒకవేళ (addr + n < addr)
      తిరిగి -1;
    ఒకవేళ (addr + n > TRAPFRAME)
      తిరిగి -1;
    myproc()->sz += n;
  }
  తిరిగి addr;
}

uint64
sys_pause(శూన్య)
{
  పూర్ణాంకం n;
  uint ticks0;

  argint(0, &n);
  ఒకవేళ (n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  అయితే (ticks - ticks0 < n) {
    ఒకవేళ (killed(myproc())) {
      release(&tickslock);
      తిరిగి -1;
    }
    sleep_prepare(&ticks);
    release(&tickslock);
    sleep();
    acquire(&tickslock);
  }
  release(&tickslock);
  తిరిగి 0;
}

uint64
sys_kill(శూన్య)
{
  పూర్ణాంకం pid;

  argint(0, &pid);
  తిరిగి kkill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(శూన్య)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  తిరిగి xticks;
}
