#ಅನ್ತರ್ಭಾವ "types.h"
#ಅನ್ತರ್ಭಾವ "riscv.h"
#ಅನ್ತರ್ಭಾವ "defs.h"
#ಅನ್ತರ್ಭಾವ "param.h"
#ಅನ್ತರ್ಭಾವ "memlayout.h"
#ಅನ್ತರ್ಭಾವ "spinlock.h"
#ಅನ್ತರ್ಭಾವ "proc.h"
#ಅನ್ತರ್ಭಾವ "vm.h"

uint64
sys_exit(ಶೂನ್ಯ)
{
  ಪೂರ್ಣಾಙ್ಕ n;
  argint(0, &n);
  kexit(n);
  ಪ್ರತ್ಯಾಗಮ 0; // not reached
}

uint64
sys_getpid(ಶೂನ್ಯ)
{
  ಪ್ರತ್ಯಾಗಮ myproc()->pid;
}

uint64
sys_fork(ಶೂನ್ಯ)
{
  ಪ್ರತ್ಯಾಗಮ kfork();
}

uint64
sys_wait(ಶೂನ್ಯ)
{
  uint64 p;
  argaddr(0, &p);
  ಪ್ರತ್ಯಾಗಮ kwait(p);
}

uint64
sys_sbrk(ಶೂನ್ಯ)
{
  uint64 addr;
  ಪೂರ್ಣಾಙ್ಕ t;
  ಪೂರ್ಣಾಙ್ಕ n;

  argint(0, &n);
  argint(1, &t);
  addr = myproc()->sz;

  ಯದಿ (t == SBRK_EAGER || n < 0) {
    ಯದಿ (growproc(n) < 0) {
      ಪ್ರತ್ಯಾಗಮ -1;
    }
  } ಅನ್ಯಥಾ {
    // Lazily allocate memory for this process: increase its memory
    // size but don't allocate memory. If the processes uses the
    // memory, vmfault() will allocate it.
    ಯದಿ (addr + n < addr)
      ಪ್ರತ್ಯಾಗಮ -1;
    ಯದಿ (addr + n > TRAPFRAME)
      ಪ್ರತ್ಯಾಗಮ -1;
    myproc()->sz += n;
  }
  ಪ್ರತ್ಯಾಗಮ addr;
}

uint64
sys_pause(ಶೂನ್ಯ)
{
  ಪೂರ್ಣಾಙ್ಕ n;
  uint ticks0;

  argint(0, &n);
  ಯದಿ (n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  ಯಾವತ್ (ticks - ticks0 < n) {
    ಯದಿ (killed(myproc())) {
      release(&tickslock);
      ಪ್ರತ್ಯಾಗಮ -1;
    }
    sleep_prepare(&ticks);
    release(&tickslock);
    sleep();
    acquire(&tickslock);
  }
  release(&tickslock);
  ಪ್ರತ್ಯಾಗಮ 0;
}

uint64
sys_kill(ಶೂನ್ಯ)
{
  ಪೂರ್ಣಾಙ್ಕ pid;

  argint(0, &pid);
  ಪ್ರತ್ಯಾಗಮ kkill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(ಶೂನ್ಯ)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  ಪ್ರತ್ಯಾಗಮ xticks;
}
