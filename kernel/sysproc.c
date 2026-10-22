#ଅନ୍ତର୍ଭାଵ "types.h"
#ଅନ୍ତର୍ଭାଵ "riscv.h"
#ଅନ୍ତର୍ଭାଵ "defs.h"
#ଅନ୍ତର୍ଭାଵ "param.h"
#ଅନ୍ତର୍ଭାଵ "memlayout.h"
#ଅନ୍ତର୍ଭାଵ "spinlock.h"
#ଅନ୍ତର୍ଭାଵ "proc.h"
#ଅନ୍ତର୍ଭାଵ "vm.h"

uint64
sys_exit(ଶୂନ୍ୟ)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା n;
  argint(0, &n);
  kexit(n);
  ଫେରନ୍ତୁ 0; // not reached
}

uint64
sys_getpid(ଶୂନ୍ୟ)
{
  ଫେରନ୍ତୁ myproc()->pid;
}

uint64
sys_fork(ଶୂନ୍ୟ)
{
  ଫେରନ୍ତୁ kfork();
}

uint64
sys_wait(ଶୂନ୍ୟ)
{
  uint64 p;
  argaddr(0, &p);
  ଫେରନ୍ତୁ kwait(p);
}

uint64
sys_sbrk(ଶୂନ୍ୟ)
{
  uint64 addr;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା t;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା n;

  argint(0, &n);
  argint(1, &t);
  addr = myproc()->sz;

  ଯଦି (t == SBRK_EAGER || n < 0) {
    ଯଦି (growproc(n) < 0) {
      ଫେରନ୍ତୁ -1;
    }
  } ଅନ୍ୟଥା {
    // Lazily allocate memory for this process: increase its memory
    // size but don't allocate memory. If the processes uses the
    // memory, vmfault() will allocate it.
    ଯଦି (addr + n < addr)
      ଫେରନ୍ତୁ -1;
    ଯଦି (addr + n > TRAPFRAME)
      ଫେରନ୍ତୁ -1;
    myproc()->sz += n;
  }
  ଫେରନ୍ତୁ addr;
}

uint64
sys_pause(ଶୂନ୍ୟ)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା n;
  uint ticks0;

  argint(0, &n);
  ଯଦି (n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  ଯେତେବେଳେ (ticks - ticks0 < n) {
    ଯଦି (killed(myproc())) {
      release(&tickslock);
      ଫେରନ୍ତୁ -1;
    }
    sleep_prepare(&ticks);
    release(&tickslock);
    sleep();
    acquire(&tickslock);
  }
  release(&tickslock);
  ଫେରନ୍ତୁ 0;
}

uint64
sys_kill(ଶୂନ୍ୟ)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା pid;

  argint(0, &pid);
  ଫେରନ୍ତୁ kkill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(ଶୂନ୍ୟ)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  ଫେରନ୍ତୁ xticks;
}
