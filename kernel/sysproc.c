#அந்தர்பாவ "types.h"
#அந்தர்பாவ "riscv.h"
#அந்தர்பாவ "defs.h"
#அந்தர்பாவ "param.h"
#அந்தர்பாவ "memlayout.h"
#அந்தர்பாவ "spinlock.h"
#அந்தர்பாவ "proc.h"
#அந்தர்பாவ "vm.h"

uint64
sys_exit(வெறுமை)
{
  முழு n;
  argint(0, &n);
  kexit(n);
  திரும்பு 0; // not reached
}

uint64
sys_getpid(வெறுமை)
{
  திரும்பு myproc()->pid;
}

uint64
sys_fork(வெறுமை)
{
  திரும்பு kfork();
}

uint64
sys_wait(வெறுமை)
{
  uint64 p;
  argaddr(0, &p);
  திரும்பு kwait(p);
}

uint64
sys_sbrk(வெறுமை)
{
  uint64 addr;
  முழு t;
  முழு n;

  argint(0, &n);
  argint(1, &t);
  addr = myproc()->sz;

  எனில் (t == SBRK_EAGER || n < 0) {
    எனில் (growproc(n) < 0) {
      திரும்பு -1;
    }
  } இல்லையெனில் {
    // Lazily allocate memory for this process: increase its memory
    // size but don't allocate memory. If the processes uses the
    // memory, vmfault() will allocate it.
    எனில் (addr + n < addr)
      திரும்பு -1;
    எனில் (addr + n > TRAPFRAME)
      திரும்பு -1;
    myproc()->sz += n;
  }
  திரும்பு addr;
}

uint64
sys_pause(வெறுமை)
{
  முழு n;
  uint ticks0;

  argint(0, &n);
  எனில் (n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  போது (ticks - ticks0 < n) {
    எனில் (killed(myproc())) {
      release(&tickslock);
      திரும்பு -1;
    }
    sleep_prepare(&ticks);
    release(&tickslock);
    sleep();
    acquire(&tickslock);
  }
  release(&tickslock);
  திரும்பு 0;
}

uint64
sys_kill(வெறுமை)
{
  முழு pid;

  argint(0, &pid);
  திரும்பு kkill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(வெறுமை)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  திரும்பு xticks;
}
