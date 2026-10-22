#अन्तर्भाव "types.h"
#अन्तर्भाव "riscv.h"
#अन्तर्भाव "defs.h"
#अन्तर्भाव "param.h"
#अन्तर्भाव "memlayout.h"
#अन्तर्भाव "spinlock.h"
#अन्तर्भाव "proc.h"
#अन्तर्भाव "vm.h"

uint64
sys_exit(शून्य)
{
  पूर्णांक n;
  argint(0, &n);
  kexit(n);
  परत 0; // not reached
}

uint64
sys_getpid(शून्य)
{
  परत myproc()->pid;
}

uint64
sys_fork(शून्य)
{
  परत kfork();
}

uint64
sys_wait(शून्य)
{
  uint64 p;
  argaddr(0, &p);
  परत kwait(p);
}

uint64
sys_sbrk(शून्य)
{
  uint64 addr;
  पूर्णांक t;
  पूर्णांक n;

  argint(0, &n);
  argint(1, &t);
  addr = myproc()->sz;

  जर (t == SBRK_EAGER || n < 0) {
    जर (growproc(n) < 0) {
      परत -1;
    }
  } अन्यथा {
    // Lazily allocate memory for this process: increase its memory
    // size but don't allocate memory. If the processes uses the
    // memory, vmfault() will allocate it.
    जर (addr + n < addr)
      परत -1;
    जर (addr + n > TRAPFRAME)
      परत -1;
    myproc()->sz += n;
  }
  परत addr;
}

uint64
sys_pause(शून्य)
{
  पूर्णांक n;
  uint ticks0;

  argint(0, &n);
  जर (n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  पर्यंत (ticks - ticks0 < n) {
    जर (killed(myproc())) {
      release(&tickslock);
      परत -1;
    }
    sleep_prepare(&ticks);
    release(&tickslock);
    sleep();
    acquire(&tickslock);
  }
  release(&tickslock);
  परत 0;
}

uint64
sys_kill(शून्य)
{
  पूर्णांक pid;

  argint(0, &pid);
  परत kkill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(शून्य)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  परत xticks;
}
