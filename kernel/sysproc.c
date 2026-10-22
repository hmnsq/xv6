#ਅਨ੍ਤਰ੍ਭਾਵ "types.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "riscv.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "defs.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "param.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "memlayout.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "spinlock.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "proc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vm.h"

uint64
sys_exit(ਖਾਲੀ)
{
  ਪੂਰਨ n;
  argint(0, &n);
  kexit(n);
  ਵਾਪਸ 0; // not reached
}

uint64
sys_getpid(ਖਾਲੀ)
{
  ਵਾਪਸ myproc()->pid;
}

uint64
sys_fork(ਖਾਲੀ)
{
  ਵਾਪਸ kfork();
}

uint64
sys_wait(ਖਾਲੀ)
{
  uint64 p;
  argaddr(0, &p);
  ਵਾਪਸ kwait(p);
}

uint64
sys_sbrk(ਖਾਲੀ)
{
  uint64 addr;
  ਪੂਰਨ t;
  ਪੂਰਨ n;

  argint(0, &n);
  argint(1, &t);
  addr = myproc()->sz;

  ਜੇ (t == SBRK_EAGER || n < 0) {
    ਜੇ (growproc(n) < 0) {
      ਵਾਪਸ -1;
    }
  } ਨਹੀਂਤਾਂ {
    // Lazily allocate memory for this process: increase its memory
    // size but don't allocate memory. If the processes uses the
    // memory, vmfault() will allocate it.
    ਜੇ (addr + n < addr)
      ਵਾਪਸ -1;
    ਜੇ (addr + n > TRAPFRAME)
      ਵਾਪਸ -1;
    myproc()->sz += n;
  }
  ਵਾਪਸ addr;
}

uint64
sys_pause(ਖਾਲੀ)
{
  ਪੂਰਨ n;
  uint ticks0;

  argint(0, &n);
  ਜੇ (n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  ਜਦੋਂ (ticks - ticks0 < n) {
    ਜੇ (killed(myproc())) {
      release(&tickslock);
      ਵਾਪਸ -1;
    }
    sleep_prepare(&ticks);
    release(&tickslock);
    sleep();
    acquire(&tickslock);
  }
  release(&tickslock);
  ਵਾਪਸ 0;
}

uint64
sys_kill(ਖਾਲੀ)
{
  ਪੂਰਨ pid;

  argint(0, &pid);
  ਵਾਪਸ kkill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(ਖਾਲੀ)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  ਵਾਪਸ xticks;
}
