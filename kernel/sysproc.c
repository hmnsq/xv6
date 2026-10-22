#شامل "types.h"
#شامل "riscv.h"
#شامل "defs.h"
#شامل "param.h"
#شامل "memlayout.h"
#شامل "spinlock.h"
#شامل "proc.h"
#شامل "vm.h"

uint64
sys_exit(خالی)
{
  صحیح n;
  argint(0, &n);
  kexit(n);
  واپس 0; // not reached
}

uint64
sys_getpid(خالی)
{
  واپس myproc()->pid;
}

uint64
sys_fork(خالی)
{
  واپس kfork();
}

uint64
sys_wait(خالی)
{
  uint64 p;
  argaddr(0, &p);
  واپس kwait(p);
}

uint64
sys_sbrk(خالی)
{
  uint64 addr;
  صحیح t;
  صحیح n;

  argint(0, &n);
  argint(1, &t);
  addr = myproc()->sz;

  اگر (t == SBRK_EAGER || n < 0) {
    اگر (growproc(n) < 0) {
      واپس -1;
    }
  } ورنہ {
    // Lazily allocate memory for this process: increase its memory
    // size but don't allocate memory. If the processes uses the
    // memory, vmfault() will allocate it.
    اگر (addr + n < addr)
      واپس -1;
    اگر (addr + n > TRAPFRAME)
      واپس -1;
    myproc()->sz += n;
  }
  واپس addr;
}

uint64
sys_pause(خالی)
{
  صحیح n;
  uint ticks0;

  argint(0, &n);
  اگر (n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  جب (ticks - ticks0 < n) {
    اگر (killed(myproc())) {
      release(&tickslock);
      واپس -1;
    }
    sleep_prepare(&ticks);
    release(&tickslock);
    sleep();
    acquire(&tickslock);
  }
  release(&tickslock);
  واپس 0;
}

uint64
sys_kill(خالی)
{
  صحیح pid;

  argint(0, &pid);
  واپس kkill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(خالی)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  واپس xticks;
}
