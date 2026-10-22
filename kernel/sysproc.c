#অন্তর্ভাব "types.h"
#অন্তর্ভাব "riscv.h"
#অন্তর্ভাব "defs.h"
#অন্তর্ভাব "param.h"
#অন্তর্ভাব "memlayout.h"
#অন্তর্ভাব "spinlock.h"
#অন্তর্ভাব "proc.h"
#অন্তর্ভাব "vm.h"

uint64
sys_exit(শূন্য)
{
  পূর্ণসংখ্যা n;
  argint(0, &n);
  kexit(n);
  ফেরত 0; // not reached
}

uint64
sys_getpid(শূন্য)
{
  ফেরত myproc()->pid;
}

uint64
sys_fork(শূন্য)
{
  ফেরত kfork();
}

uint64
sys_wait(শূন্য)
{
  uint64 p;
  argaddr(0, &p);
  ফেরত kwait(p);
}

uint64
sys_sbrk(শূন্য)
{
  uint64 addr;
  পূর্ণসংখ্যা t;
  পূর্ণসংখ্যা n;

  argint(0, &n);
  argint(1, &t);
  addr = myproc()->sz;

  যদি (t == SBRK_EAGER || n < 0) {
    যদি (growproc(n) < 0) {
      ফেরত -1;
    }
  } অন্যথা {
    // Lazily allocate memory for this process: increase its memory
    // size but don't allocate memory. If the processes uses the
    // memory, vmfault() will allocate it.
    যদি (addr + n < addr)
      ফেরত -1;
    যদি (addr + n > TRAPFRAME)
      ফেরত -1;
    myproc()->sz += n;
  }
  ফেরত addr;
}

uint64
sys_pause(শূন্য)
{
  পূর্ণসংখ্যা n;
  uint ticks0;

  argint(0, &n);
  যদি (n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  যখন (ticks - ticks0 < n) {
    যদি (killed(myproc())) {
      release(&tickslock);
      ফেরত -1;
    }
    sleep_prepare(&ticks);
    release(&tickslock);
    sleep();
    acquire(&tickslock);
  }
  release(&tickslock);
  ফেরত 0;
}

uint64
sys_kill(শূন্য)
{
  পূর্ণসংখ্যা pid;

  argint(0, &pid);
  ফেরত kkill(pid);
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
  ফেরত xticks;
}
