#അന്തര്ഭാവ "types.h"
#അന്തര്ഭാവ "riscv.h"
#അന്തര്ഭാവ "defs.h"
#അന്തര്ഭാവ "param.h"
#അന്തര്ഭാവ "memlayout.h"
#അന്തര്ഭാവ "spinlock.h"
#അന്തര്ഭാവ "proc.h"
#അന്തര്ഭാവ "vm.h"

uint64
sys_exit(ശൂന്യം)
{
  പൂർണ്ണസംഖ്യ n;
  argint(0, &n);
  kexit(n);
  തിരികെ 0; // not reached
}

uint64
sys_getpid(ശൂന്യം)
{
  തിരികെ myproc()->pid;
}

uint64
sys_fork(ശൂന്യം)
{
  തിരികെ kfork();
}

uint64
sys_wait(ശൂന്യം)
{
  uint64 p;
  argaddr(0, &p);
  തിരികെ kwait(p);
}

uint64
sys_sbrk(ശൂന്യം)
{
  uint64 addr;
  പൂർണ്ണസംഖ്യ t;
  പൂർണ്ണസംഖ്യ n;

  argint(0, &n);
  argint(1, &t);
  addr = myproc()->sz;

  എങ്കിൽ (t == SBRK_EAGER || n < 0) {
    എങ്കിൽ (growproc(n) < 0) {
      തിരികെ -1;
    }
  } അല്ലെങ്കിൽ {
    // Lazily allocate memory for this process: increase its memory
    // size but don't allocate memory. If the processes uses the
    // memory, vmfault() will allocate it.
    എങ്കിൽ (addr + n < addr)
      തിരികെ -1;
    എങ്കിൽ (addr + n > TRAPFRAME)
      തിരികെ -1;
    myproc()->sz += n;
  }
  തിരികെ addr;
}

uint64
sys_pause(ശൂന്യം)
{
  പൂർണ്ണസംഖ്യ n;
  uint ticks0;

  argint(0, &n);
  എങ്കിൽ (n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  ആകുമ്പോൾ (ticks - ticks0 < n) {
    എങ്കിൽ (killed(myproc())) {
      release(&tickslock);
      തിരികെ -1;
    }
    sleep_prepare(&ticks);
    release(&tickslock);
    sleep();
    acquire(&tickslock);
  }
  release(&tickslock);
  തിരികെ 0;
}

uint64
sys_kill(ശൂന്യം)
{
  പൂർണ്ണസംഖ്യ pid;

  argint(0, &pid);
  തിരികെ kkill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(ശൂന്യം)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  തിരികെ xticks;
}
