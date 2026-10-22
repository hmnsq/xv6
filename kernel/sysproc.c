#समावेश "types.h"
#समावेश "riscv.h"
#समावेश "defs.h"
#समावेश "param.h"
#समावेश "memlayout.h"
#समावेश "spinlock.h"
#समावेश "proc.h"
#समावेश "vm.h"

uint64
sys_exit(व्योम)
{
  पूर्णांक n;
  argint(0, &n);
  kexit(n);
  वापस 0; // not reached
}

uint64
sys_getpid(व्योम)
{
  वापस myproc()->pid;
}

uint64
sys_fork(व्योम)
{
  वापस kfork();
}

uint64
sys_wait(व्योम)
{
  uint64 p;
  argaddr(0, &p);
  वापस kwait(p);
}

uint64
sys_sbrk(व्योम)
{
  uint64 addr;
  पूर्णांक t;
  पूर्णांक n;

  argint(0, &n);
  argint(1, &t);
  addr = myproc()->sz;

  अगर (t == SBRK_EAGER || n < 0) {
    अगर (growproc(n) < 0) {
      वापस -1;
    }
  } अन्यथा {
    // Lazily allocate memory for this process: increase its memory
    // size but don't allocate memory. If the processes uses the
    // memory, vmfault() will allocate it.
    अगर (addr + n < addr)
      वापस -1;
    अगर (addr + n > TRAPFRAME)
      वापस -1;
    myproc()->sz += n;
  }
  वापस addr;
}

uint64
sys_pause(व्योम)
{
  पूर्णांक n;
  uint ticks0;

  argint(0, &n);
  अगर (n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  जबतक (ticks - ticks0 < n) {
    अगर (killed(myproc())) {
      release(&tickslock);
      वापस -1;
    }
    sleep_prepare(&ticks);
    release(&tickslock);
    sleep();
    acquire(&tickslock);
  }
  release(&tickslock);
  वापस 0;
}

uint64
sys_kill(व्योम)
{
  पूर्णांक pid;

  argint(0, &pid);
  वापस kkill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(व्योम)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  वापस xticks;
}
