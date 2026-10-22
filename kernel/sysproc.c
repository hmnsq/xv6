#অন্তর্ভাৱ "types.h"
#অন্তর্ভাৱ "riscv.h"
#অন্তর্ভাৱ "defs.h"
#অন্তর্ভাৱ "param.h"
#অন্তর্ভাৱ "memlayout.h"
#অন্তর্ভাৱ "spinlock.h"
#অন্তর্ভাৱ "proc.h"
#অন্তর্ভাৱ "vm.h"

uint64
sys_exit(শূন্য)
{
  পূর্ণাঙ্ক n;
  argint(0, &n);
  kexit(n);
  ঘূৰাই 0; // not reached
}

uint64
sys_getpid(শূন্য)
{
  ঘূৰাই myproc()->pid;
}

uint64
sys_fork(শূন্য)
{
  ঘূৰাই kfork();
}

uint64
sys_wait(শূন্য)
{
  uint64 p;
  argaddr(0, &p);
  ঘূৰাই kwait(p);
}

uint64
sys_sbrk(শূন্য)
{
  uint64 addr;
  পূর্ণাঙ্ক t;
  পূর্ণাঙ্ক n;

  argint(0, &n);
  argint(1, &t);
  addr = myproc()->sz;

  যদি (t == SBRK_EAGER || n < 0) {
    যদি (growproc(n) < 0) {
      ঘূৰাই -1;
    }
  } অন্যথা {
    // Lazily allocate memory for this process: increase its memory
    // size but don't allocate memory. If the processes uses the
    // memory, vmfault() will allocate it.
    যদি (addr + n < addr)
      ঘূৰাই -1;
    যদি (addr + n > TRAPFRAME)
      ঘূৰাই -1;
    myproc()->sz += n;
  }
  ঘূৰাই addr;
}

uint64
sys_pause(শূন্য)
{
  পূর্ণাঙ্ক n;
  uint ticks0;

  argint(0, &n);
  যদি (n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  যেতিয়ালৈ (ticks - ticks0 < n) {
    যদি (killed(myproc())) {
      release(&tickslock);
      ঘূৰাই -1;
    }
    sleep_prepare(&ticks);
    release(&tickslock);
    sleep();
    acquire(&tickslock);
  }
  release(&tickslock);
  ঘূৰাই 0;
}

uint64
sys_kill(শূন্য)
{
  পূর্ণাঙ্ক pid;

  argint(0, &pid);
  ঘূৰাই kkill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(শূন্য)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  ঘূৰাই xticks;
}
