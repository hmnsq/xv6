#ضمن "types.h"
#ضمن "riscv.h"
#ضمن "defs.h"
#ضمن "param.h"
#ضمن "memlayout.h"
#ضمن "spinlock.h"
#ضمن "proc.h"
#ضمن "vm.h"

uint64
sys_exit(فراغ)
{
  صحيح n;
  argint(0, &n);
  kexit(n);
  ܦܢܐ 0; // not reached
}

uint64
sys_getpid(فراغ)
{
  ܦܢܐ myproc()->pid;
}

uint64
sys_fork(فراغ)
{
  ܦܢܐ kfork();
}

uint64
sys_wait(فراغ)
{
  uint64 p;
  argaddr(0, &p);
  ܦܢܐ kwait(p);
}

uint64
sys_sbrk(فراغ)
{
  uint64 addr;
  صحيح t;
  صحيح n;

  argint(0, &n);
  argint(1, &t);
  addr = myproc()->sz;

  ܐܢ (t == SBRK_EAGER || n < 0) {
    ܐܢ (growproc(n) < 0) {
      ܦܢܐ -1;
    }
  } ܐܚܪܢܐ {
    // Lazily allocate memory for this process: increase its memory
    // size but don't allocate memory. If the processes uses the
    // memory, vmfault() will allocate it.
    ܐܢ (addr + n < addr)
      ܦܢܐ -1;
    ܐܢ (addr + n > TRAPFRAME)
      ܦܢܐ -1;
    myproc()->sz += n;
  }
  ܦܢܐ addr;
}

uint64
sys_pause(فراغ)
{
  صحيح n;
  uint ticks0;

  argint(0, &n);
  ܐܢ (n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  ܟܡܐ (ticks - ticks0 < n) {
    ܐܢ (killed(myproc())) {
      release(&tickslock);
      ܦܢܐ -1;
    }
    sleep_prepare(&ticks);
    release(&tickslock);
    sleep();
    acquire(&tickslock);
  }
  release(&tickslock);
  ܦܢܐ 0;
}

uint64
sys_kill(فراغ)
{
  صحيح pid;

  argint(0, &pid);
  ܦܢܐ kkill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(فراغ)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  ܦܢܐ xticks;
}
