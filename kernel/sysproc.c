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
  החזר 0; // not reached
}

uint64
sys_getpid(فراغ)
{
  החזר myproc()->pid;
}

uint64
sys_fork(فراغ)
{
  החזר kfork();
}

uint64
sys_wait(فراغ)
{
  uint64 p;
  argaddr(0, &p);
  החזר kwait(p);
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

  אם (t == SBRK_EAGER || n < 0) {
    אם (growproc(n) < 0) {
      החזר -1;
    }
  } אחרת {
    // Lazily allocate memory for this process: increase its memory
    // size but don't allocate memory. If the processes uses the
    // memory, vmfault() will allocate it.
    אם (addr + n < addr)
      החזר -1;
    אם (addr + n > TRAPFRAME)
      החזר -1;
    myproc()->sz += n;
  }
  החזר addr;
}

uint64
sys_pause(فراغ)
{
  صحيح n;
  uint ticks0;

  argint(0, &n);
  אם (n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  כלעוד (ticks - ticks0 < n) {
    אם (killed(myproc())) {
      release(&tickslock);
      החזר -1;
    }
    sleep_prepare(&ticks);
    release(&tickslock);
    sleep();
    acquire(&tickslock);
  }
  release(&tickslock);
  החזר 0;
}

uint64
sys_kill(فراغ)
{
  صحيح pid;

  argint(0, &pid);
  החזר kkill(pid);
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
  החזר xticks;
}
