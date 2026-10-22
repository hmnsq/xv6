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
  ကိန်းပြည့် n;
  argint(0, &n);
  kexit(n);
  ပြန်ပေး 0; // not reached
}

uint64
sys_getpid(void)
{
  ပြန်ပေး myproc()->pid;
}

uint64
sys_fork(void)
{
  ပြန်ပေး kfork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  ပြန်ပေး kwait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  ကိန်းပြည့် t;
  ကိန်းပြည့် n;

  argint(0, &n);
  argint(1, &t);
  addr = myproc()->sz;

  အကယ်၍ (t == SBRK_EAGER || n < 0) {
    အကယ်၍ (growproc(n) < 0) {
      ပြန်ပေး -1;
    }
  } မဟုတ်ပါက {
    // Lazily allocate memory for this process: increase its memory
    // size but don't allocate memory. If the processes uses the
    // memory, vmfault() will allocate it.
    အကယ်၍ (addr + n < addr)
      ပြန်ပေး -1;
    အကယ်၍ (addr + n > TRAPFRAME)
      ပြန်ပေး -1;
    myproc()->sz += n;
  }
  ပြန်ပေး addr;
}

uint64
sys_pause(void)
{
  ကိန်းပြည့် n;
  uint ticks0;

  argint(0, &n);
  အကယ်၍ (n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  နေစဉ် (ticks - ticks0 < n) {
    အကယ်၍ (killed(myproc())) {
      release(&tickslock);
      ပြန်ပေး -1;
    }
    sleep_prepare(&ticks);
    release(&tickslock);
    sleep();
    acquire(&tickslock);
  }
  release(&tickslock);
  ပြန်ပေး 0;
}

uint64
sys_kill(void)
{
  ကိန်းပြည့် pid;

  argint(0, &pid);
  ပြန်ပေး kkill(pid);
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
  ပြန်ပေး xticks;
}
