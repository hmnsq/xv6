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
  ຈຳນວນເຕັມ n;
  argint(0, &n);
  kexit(n);
  ສົ່ງຄືນ 0; // not reached
}

uint64
sys_getpid(void)
{
  ສົ່ງຄືນ myproc()->pid;
}

uint64
sys_fork(void)
{
  ສົ່ງຄືນ kfork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  ສົ່ງຄືນ kwait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  ຈຳນວນເຕັມ t;
  ຈຳນວນເຕັມ n;

  argint(0, &n);
  argint(1, &t);
  addr = myproc()->sz;

  ຖ້າ (t == SBRK_EAGER || n < 0) {
    ຖ້າ (growproc(n) < 0) {
      ສົ່ງຄືນ -1;
    }
  } ບໍ່ດັ່ງນັ້ນ {
    // Lazily allocate memory for this process: increase its memory
    // size but don't allocate memory. If the processes uses the
    // memory, vmfault() will allocate it.
    ຖ້າ (addr + n < addr)
      ສົ່ງຄືນ -1;
    ຖ້າ (addr + n > TRAPFRAME)
      ສົ່ງຄືນ -1;
    myproc()->sz += n;
  }
  ສົ່ງຄືນ addr;
}

uint64
sys_pause(void)
{
  ຈຳນວນເຕັມ n;
  uint ticks0;

  argint(0, &n);
  ຖ້າ (n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  ໃນຂະນະທີ່ (ticks - ticks0 < n) {
    ຖ້າ (killed(myproc())) {
      release(&tickslock);
      ສົ່ງຄືນ -1;
    }
    sleep_prepare(&ticks);
    release(&tickslock);
    sleep();
    acquire(&tickslock);
  }
  release(&tickslock);
  ສົ່ງຄືນ 0;
}

uint64
sys_kill(void)
{
  ຈຳນວນເຕັມ pid;

  argint(0, &pid);
  ສົ່ງຄືນ kkill(pid);
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
  ສົ່ງຄືນ xticks;
}
