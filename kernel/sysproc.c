#شامل "types.h"
#شامل "riscv.h"
#شامل "defs.h"
#شامل "param.h"
#شامل "memlayout.h"
#شامل "spinlock.h"
#شامل "proc.h"
#شامل "vm.h"

uint64
sys_exit(تهی)
{
  صحیح n;
  argint(0, &n);
  kexit(n);
  بازگشت 0; // not reached
}

uint64
sys_getpid(تهی)
{
  بازگشت myproc()->pid;
}

uint64
sys_fork(تهی)
{
  بازگشت kfork();
}

uint64
sys_wait(تهی)
{
  uint64 p;
  argaddr(0, &p);
  بازگشت kwait(p);
}

uint64
sys_sbrk(تهی)
{
  uint64 addr;
  صحیح t;
  صحیح n;

  argint(0, &n);
  argint(1, &t);
  addr = myproc()->sz;

  اگر (t == SBRK_EAGER || n < 0) {
    اگر (growproc(n) < 0) {
      بازگشت -1;
    }
  } وگرنه {
    // Lazily allocate memory for this process: increase its memory
    // size but don't allocate memory. If the processes uses the
    // memory, vmfault() will allocate it.
    اگر (addr + n < addr)
      بازگشت -1;
    اگر (addr + n > TRAPFRAME)
      بازگشت -1;
    myproc()->sz += n;
  }
  بازگشت addr;
}

uint64
sys_pause(تهی)
{
  صحیح n;
  uint ticks0;

  argint(0, &n);
  اگر (n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  وقتی (ticks - ticks0 < n) {
    اگر (killed(myproc())) {
      release(&tickslock);
      بازگشت -1;
    }
    sleep_prepare(&ticks);
    release(&tickslock);
    sleep();
    acquire(&tickslock);
  }
  release(&tickslock);
  بازگشت 0;
}

uint64
sys_kill(تهی)
{
  صحیح pid;

  argint(0, &pid);
  بازگشت kkill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(تهی)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  بازگشت xticks;
}
