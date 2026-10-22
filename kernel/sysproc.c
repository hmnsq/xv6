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
  أرجع 0; // not reached
}

uint64
sys_getpid(فراغ)
{
  أرجع myproc()->pid;
}

uint64
sys_fork(فراغ)
{
  أرجع kfork();
}

uint64
sys_wait(فراغ)
{
  uint64 p;
  argaddr(0, &p);
  أرجع kwait(p);
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

  𐤀𐤌 (t == SBRK_EAGER || n < 0) {
    𐤀𐤌 (growproc(n) < 0) {
      أرجع -1;
    }
  } وإلا {
    // Lazily allocate memory for this process: increase its memory
    // size but don't allocate memory. If the processes uses the
    // memory, vmfault() will allocate it.
    𐤀𐤌 (addr + n < addr)
      أرجع -1;
    𐤀𐤌 (addr + n > TRAPFRAME)
      أرجع -1;
    myproc()->sz += n;
  }
  أرجع addr;
}

uint64
sys_pause(فراغ)
{
  صحيح n;
  uint ticks0;

  argint(0, &n);
  𐤀𐤌 (n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  طالما (ticks - ticks0 < n) {
    𐤀𐤌 (killed(myproc())) {
      release(&tickslock);
      أرجع -1;
    }
    sleep_prepare(&ticks);
    release(&tickslock);
    sleep();
    acquire(&tickslock);
  }
  release(&tickslock);
  أرجع 0;
}

uint64
sys_kill(فراغ)
{
  صحيح pid;

  argint(0, &pid);
  أرجع kkill(pid);
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
  أرجع xticks;
}
