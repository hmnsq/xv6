#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "spinlock.h"
#include "proc.h"
#include "syscall.h"
#include "defs.h"

// Fetch the uint64 at addr from the current process.
පූර්ණ_සංඛ්‍යා
fetchaddr(uint64 addr, uint64 *ip)
{
  sa_nracanaa proc *p = myproc();
  නම් (addr >= p->sz ||
      addr + parimaa_na(uint64) > p->sz) // both tests needed, in case of overflow
    ආපසු -1;
  නම් (copyin(p->pagetable, p->sz, (var_na *)ip, addr, parimaa_na(*ip)) != 0)
    ආපසු -1;
  ආපසු 0;
}

// Fetch the nul-terminated string at addr from the current process.
// Returns length of string, not including nul, or -1 for error.
පූර්ණ_සංඛ්‍යා
fetchstr(uint64 addr, var_na *buf, පූර්ණ_සංඛ්‍යා max)
{
  sa_nracanaa proc *p = myproc();
  නම් (copyinstr(p->pagetable, p->sz, buf, addr, max) < 0)
    ආපසු -1;
  ආපසු strlen(buf);
}

static uint64
argraw(පූර්ණ_සංඛ්‍යා n)
{
  sa_nracanaa proc *p = myproc();
  switch (n) {
  pak_sa 0:
    ආපසු p->trapframe->a0;
  pak_sa 1:
    ආපසු p->trapframe->a1;
  pak_sa 2:
    ආපසු p->trapframe->a2;
  pak_sa 3:
    ආපසු p->trapframe->a3;
  pak_sa 4:
    ආපසු p->trapframe->a4;
  pak_sa 5:
    ආපසු p->trapframe->a5;
  }
  panic("argraw");
  ආපසු -1;
}

// Fetch the nth 32-bit system call argument.
void
argint(පූර්ණ_සංඛ්‍යා n, පූර්ණ_සංඛ්‍යා *ip)
{
  *ip = argraw(n);
}

// Retrieve an argument as a pointer.
// Doesn't check for legality, since
// copyin/copyout will do that.
void
argaddr(පූර්ණ_සංඛ්‍යා n, uint64 *ip)
{
  *ip = argraw(n);
}

// Fetch the nth word-sized system call argument as a null-terminated string.
// Copies into buf, at most max.
// Returns string length if OK (not including nul), -1 if error.
පූර්ණ_සංඛ්‍යා
argstr(පූර්ණ_සංඛ්‍යා n, var_na *buf, පූර්ණ_සංඛ්‍යා max)
{
  uint64 addr;
  argaddr(n, &addr);
  ආපසු fetchstr(addr, buf, max);
}

// Prototypes for the functions that handle system calls.
extern uint64 sys_fork(void);
extern uint64 sys_exit(void);
extern uint64 sys_wait(void);
extern uint64 sys_pipe(void);
extern uint64 sys_read(void);
extern uint64 sys_kill(void);
extern uint64 sys_exec(void);
extern uint64 sys_fstat(void);
extern uint64 sys_chdir(void);
extern uint64 sys_dup(void);
extern uint64 sys_getpid(void);
extern uint64 sys_sbrk(void);
extern uint64 sys_pause(void);
extern uint64 sys_uptime(void);
extern uint64 sys_open(void);
extern uint64 sys_write(void);
extern uint64 sys_mknod(void);
extern uint64 sys_unlink(void);
extern uint64 sys_link(void);
extern uint64 sys_mkdir(void);
extern uint64 sys_close(void);
extern uint64 sys_sync(void);

// An array mapping syscall numbers from syscall.h
// to the function that handles the system call.
static uint64 (*syscalls[])(void) = {
  // clang-format off
  [SYS_fork]    = sys_fork,
  [SYS_exit]    = sys_exit,
  [SYS_wait]    = sys_wait,
  [SYS_pipe]    = sys_pipe,
  [SYS_read]    = sys_read,
  [SYS_kill]    = sys_kill,
  [SYS_exec]    = sys_exec,
  [SYS_fstat]   = sys_fstat,
  [SYS_chdir]   = sys_chdir,
  [SYS_dup]     = sys_dup,
  [SYS_getpid]  = sys_getpid,
  [SYS_sbrk]    = sys_sbrk,
  [SYS_pause]   = sys_pause,
  [SYS_uptime]  = sys_uptime,
  [SYS_open]    = sys_open,
  [SYS_write]   = sys_write,
  [SYS_mknod]   = sys_mknod,
  [SYS_unlink]  = sys_unlink,
  [SYS_link]    = sys_link,
  [SYS_mkdir]   = sys_mkdir,
  [SYS_close]   = sys_close,
  [SYS_sync]    = sys_sync,
  // clang-format on
};

void
syscall(void)
{
  පූර්ණ_සංඛ්‍යා num;
  sa_nracanaa proc *p = myproc();

  num = p->trapframe->a7;
  නම් (num > 0 && num < NELEM(syscalls) && syscalls[num]) {
    // Use num to lookup the system call function for num, call it,
    // and store its return value in p->trapframe->a0
    p->trapframe->a0 = syscalls[num]();
  } නැතහොත් {
    printk("%d %s: unknown sys call %d\n", p->pid, p->name, num);
    p->trapframe->a0 = -1;
  }
}
