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
  ಪೂರ್ಣಾಂಕ n;
  argint(0, &n);
  kexit(n);
  ಹಿಂತಿರುಗಿ 0; // not reached
}

uint64
sys_getpid(ಶೂನ್ಯ)
{
  ಹಿಂತಿರುಗಿ myproc()->pid;
}

uint64
sys_fork(ಶೂನ್ಯ)
{
  ಹಿಂತಿರುಗಿ kfork();
}

uint64
sys_wait(ಶೂನ್ಯ)
{
  uint64 p;
  argaddr(0, &p);
  ಹಿಂತಿರುಗಿ kwait(p);
}

uint64
sys_sbrk(ಶೂನ್ಯ)
{
  uint64 addr;
  ಪೂರ್ಣಾಂಕ t;
  ಪೂರ್ಣಾಂಕ n;

  argint(0, &n);
  argint(1, &t);
  addr = myproc()->sz;

  ಆದರೆ (t == SBRK_EAGER || n < 0) {
    ಆದರೆ (growproc(n) < 0) {
      ಹಿಂತಿರುಗಿ -1;
    }
  } ಇಲ್ಲದಿದ್ದರೆ {
    // Lazily allocate memory for this process: increase its memory
    // size but don't allocate memory. If the processes uses the
    // memory, vmfault() will allocate it.
    ಆದರೆ (addr + n < addr)
      ಹಿಂತಿರುಗಿ -1;
    ಆದರೆ (addr + n > TRAPFRAME)
      ಹಿಂತಿರುಗಿ -1;
    myproc()->sz += n;
  }
  ಹಿಂತಿರುಗಿ addr;
}

uint64
sys_pause(ಶೂನ್ಯ)
{
  ಪೂರ್ಣಾಂಕ n;
  uint ticks0;

  argint(0, &n);
  ಆದರೆ (n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  ಆಗ (ticks - ticks0 < n) {
    ಆದರೆ (killed(myproc())) {
      release(&tickslock);
      ಹಿಂತಿರುಗಿ -1;
    }
    sleep_prepare(&ticks);
    release(&tickslock);
    sleep();
    acquire(&tickslock);
  }
  release(&tickslock);
  ಹಿಂತಿರುಗಿ 0;
}

uint64
sys_kill(ಶೂನ್ಯ)
{
  ಪೂರ್ಣಾಂಕ pid;

  argint(0, &pid);
  ಹಿಂತಿರುಗಿ kkill(pid);
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
  ಹಿಂತಿರುಗಿ xticks;
}
