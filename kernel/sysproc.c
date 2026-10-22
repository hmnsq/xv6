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
  पूर्णाङ्क n;
  argint(0, &n);
  kexit(n);
  पटिनिवत्त 0; // not reached
}

uint64
sys_getpid(शून्य)
{
  पटिनिवत्त myproc()->pid;
}

uint64
sys_fork(शून्य)
{
  पटिनिवत्त kfork();
}

uint64
sys_wait(शून्य)
{
  uint64 p;
  argaddr(0, &p);
  पटिनिवत्त kwait(p);
}

uint64
sys_sbrk(शून्य)
{
  uint64 addr;
  पूर्णाङ्क t;
  पूर्णाङ्क n;

  argint(0, &n);
  argint(1, &t);
  addr = myproc()->sz;

  सचे (t == SBRK_EAGER || n < 0) {
    सचे (growproc(n) < 0) {
      पटिनिवत्त -1;
    }
  } अञ्ञथा {
    // Lazily allocate memory for this process: increase its memory
    // size but don't allocate memory. If the processes uses the
    // memory, vmfault() will allocate it.
    सचे (addr + n < addr)
      पटिनिवत्त -1;
    सचे (addr + n > TRAPFRAME)
      पटिनिवत्त -1;
    myproc()->sz += n;
  }
  पटिनिवत्त addr;
}

uint64
sys_pause(शून्य)
{
  पूर्णाङ्क n;
  uint ticks0;

  argint(0, &n);
  सचे (n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  याव (ticks - ticks0 < n) {
    सचे (killed(myproc())) {
      release(&tickslock);
      पटिनिवत्त -1;
    }
    sleep_prepare(&ticks);
    release(&tickslock);
    sleep();
    acquire(&tickslock);
  }
  release(&tickslock);
  पटिनिवत्त 0;
}

uint64
sys_kill(शून्य)
{
  पूर्णाङ्क pid;

  argint(0, &pid);
  पटिनिवत्त kkill(pid);
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
  पटिनिवत्त xticks;
}
