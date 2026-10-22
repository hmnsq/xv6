#અન્તર્ભાવ "types.h"
#અન્તર્ભાવ "riscv.h"
#અન્તર્ભાવ "defs.h"
#અન્તર્ભાવ "param.h"
#અન્તર્ભાવ "memlayout.h"
#અન્તર્ભાવ "spinlock.h"
#અન્તર્ભાવ "proc.h"
#અન્તર્ભાવ "vm.h"

uint64
sys_exit(શૂન્ય)
{
  પૂર્ણાંક n;
  argint(0, &n);
  kexit(n);
  પાછા 0; // not reached
}

uint64
sys_getpid(શૂન્ય)
{
  પાછા myproc()->pid;
}

uint64
sys_fork(શૂન્ય)
{
  પાછા kfork();
}

uint64
sys_wait(શૂન્ય)
{
  uint64 p;
  argaddr(0, &p);
  પાછા kwait(p);
}

uint64
sys_sbrk(શૂન્ય)
{
  uint64 addr;
  પૂર્ણાંક t;
  પૂર્ણાંક n;

  argint(0, &n);
  argint(1, &t);
  addr = myproc()->sz;

  જો (t == SBRK_EAGER || n < 0) {
    જો (growproc(n) < 0) {
      પાછા -1;
    }
  } નહીંતર {
    // Lazily allocate memory for this process: increase its memory
    // size but don't allocate memory. If the processes uses the
    // memory, vmfault() will allocate it.
    જો (addr + n < addr)
      પાછા -1;
    જો (addr + n > TRAPFRAME)
      પાછા -1;
    myproc()->sz += n;
  }
  પાછા addr;
}

uint64
sys_pause(શૂન્ય)
{
  પૂર્ણાંક n;
  uint ticks0;

  argint(0, &n);
  જો (n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  જ્યારે (ticks - ticks0 < n) {
    જો (killed(myproc())) {
      release(&tickslock);
      પાછા -1;
    }
    sleep_prepare(&ticks);
    release(&tickslock);
    sleep();
    acquire(&tickslock);
  }
  release(&tickslock);
  પાછા 0;
}

uint64
sys_kill(શૂન્ય)
{
  પૂર્ણાંક pid;

  argint(0, &pid);
  પાછા kkill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(શૂન્ય)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  પાછા xticks;
}
