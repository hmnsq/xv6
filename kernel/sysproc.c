#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "vm.h"

uint64
sys_exit(void)
{
  ཧྲིལ n;
  argint(0, &n);
  kexit(n);
  ཕྱིར 0; // not reached
}

uint64
sys_getpid(void)
{
  ཕྱིར myproc()->pid;
}

uint64
sys_fork(void)
{
  ཕྱིར kfork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  ཕྱིར kwait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  ཧྲིལ t;
  ཧྲིལ n;

  argint(0, &n);
  argint(1, &t);
  addr = myproc()->sz;

  གལ་ཏེ (t == SBRK_EAGER || n < 0) {
    གལ་ཏེ (growproc(n) < 0) {
      ཕྱིར -1;
    }
  } དེ་མིན {
    // Lazily allocate memory for this process: increase its memory
    // size but don't allocate memory. If the processes uses the
    // memory, vmfault() will allocate it.
    གལ་ཏེ (addr + n < addr)
      ཕྱིར -1;
    གལ་ཏེ (addr + n > TRAPFRAME)
      ཕྱིར -1;
    myproc()->sz += n;
  }
  ཕྱིར addr;
}

uint64
sys_pause(void)
{
  ཧྲིལ n;
  uint ticks0;

  argint(0, &n);
  གལ་ཏེ (n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  སྐབས (ticks - ticks0 < n) {
    གལ་ཏེ (killed(myproc())) {
      release(&tickslock);
      ཕྱིར -1;
    }
    sleep_prepare(&ticks);
    release(&tickslock);
    sleep();
    acquire(&tickslock);
  }
  release(&tickslock);
  ཕྱིར 0;
}

uint64
sys_kill(void)
{
  ཧྲིལ pid;

  argint(0, &pid);
  ཕྱིར kkill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  ཕྱིར xticks;
}
