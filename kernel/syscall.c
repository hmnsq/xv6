#ଅନ୍ତର୍ଭାଵ "types.h"
#ଅନ୍ତର୍ଭାଵ "param.h"
#ଅନ୍ତର୍ଭାଵ "memlayout.h"
#ଅନ୍ତର୍ଭାଵ "riscv.h"
#ଅନ୍ତର୍ଭାଵ "spinlock.h"
#ଅନ୍ତର୍ଭାଵ "proc.h"
#ଅନ୍ତର୍ଭାଵ "syscall.h"
#ଅନ୍ତର୍ଭାଵ "defs.h"

// Fetch the uint64 at addr from the current process.
ପୂର୍ଣ୍ଣସଂଖ୍ୟା
fetchaddr(uint64 addr, uint64 *ip)
{
  ସଂରଚନା proc *p = myproc();
  ଯଦି (addr >= p->sz ||
      addr + ଆକାର(uint64) > p->sz) // both tests needed, in case of overflow
    ଫେରନ୍ତୁ -1;
  ଯଦି (copyin(p->pagetable, p->sz, (ଅକ୍ଷର *)ip, addr, ଆକାର(*ip)) != 0)
    ଫେରନ୍ତୁ -1;
  ଫେରନ୍ତୁ 0;
}

// Fetch the nul-terminated string at addr from the current process.
// Returns length of string, not including nul, or -1 for error.
ପୂର୍ଣ୍ଣସଂଖ୍ୟା
fetchstr(uint64 addr, ଅକ୍ଷର *buf, ପୂର୍ଣ୍ଣସଂଖ୍ୟା max)
{
  ସଂରଚନା proc *p = myproc();
  ଯଦି (copyinstr(p->pagetable, p->sz, buf, addr, max) < 0)
    ଫେରନ୍ତୁ -1;
  ଫେରନ୍ତୁ strlen(buf);
}

ସ୍ଥିର uint64
argraw(ପୂର୍ଣ୍ଣସଂଖ୍ୟା n)
{
  ସଂରଚନା proc *p = myproc();
  ବାଛ (n) {
  ସ୍ଥିତି 0:
    ଫେରନ୍ତୁ p->trapframe->a0;
  ସ୍ଥିତି 1:
    ଫେରନ୍ତୁ p->trapframe->a1;
  ସ୍ଥିତି 2:
    ଫେରନ୍ତୁ p->trapframe->a2;
  ସ୍ଥିତି 3:
    ଫେରନ୍ତୁ p->trapframe->a3;
  ସ୍ଥିତି 4:
    ଫେରନ୍ତୁ p->trapframe->a4;
  ସ୍ଥିତି 5:
    ଫେରନ୍ତୁ p->trapframe->a5;
  }
  panic("argraw");
  ଫେରନ୍ତୁ -1;
}

// Fetch the nth 32-bit system call argument.
ଶୂନ୍ୟ
argint(ପୂର୍ଣ୍ଣସଂଖ୍ୟା n, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ip)
{
  *ip = argraw(n);
}

// Retrieve an argument as a pointer.
// Doesn't check for legality, since
// copyin/copyout will do that.
ଶୂନ୍ୟ
argaddr(ପୂର୍ଣ୍ଣସଂଖ୍ୟା n, uint64 *ip)
{
  *ip = argraw(n);
}

// Fetch the nth word-sized system call argument as a null-terminated string.
// Copies into buf, at most max.
// Returns string length if OK (not including nul), -1 if error.
ପୂର୍ଣ୍ଣସଂଖ୍ୟା
argstr(ପୂର୍ଣ୍ଣସଂଖ୍ୟା n, ଅକ୍ଷର *buf, ପୂର୍ଣ୍ଣସଂଖ୍ୟା max)
{
  uint64 addr;
  argaddr(n, &addr);
  ଫେରନ୍ତୁ fetchstr(addr, buf, max);
}

// Prototypes for the functions that handle system calls.
ବାହ୍ୟ uint64 sys_fork(ଶୂନ୍ୟ);
ବାହ୍ୟ uint64 sys_exit(ଶୂନ୍ୟ);
ବାହ୍ୟ uint64 sys_wait(ଶୂନ୍ୟ);
ବାହ୍ୟ uint64 sys_pipe(ଶୂନ୍ୟ);
ବାହ୍ୟ uint64 sys_read(ଶୂନ୍ୟ);
ବାହ୍ୟ uint64 sys_kill(ଶୂନ୍ୟ);
ବାହ୍ୟ uint64 sys_exec(ଶୂନ୍ୟ);
ବାହ୍ୟ uint64 sys_fstat(ଶୂନ୍ୟ);
ବାହ୍ୟ uint64 sys_chdir(ଶୂନ୍ୟ);
ବାହ୍ୟ uint64 sys_dup(ଶୂନ୍ୟ);
ବାହ୍ୟ uint64 sys_getpid(ଶୂନ୍ୟ);
ବାହ୍ୟ uint64 sys_sbrk(ଶୂନ୍ୟ);
ବାହ୍ୟ uint64 sys_pause(ଶୂନ୍ୟ);
ବାହ୍ୟ uint64 sys_uptime(ଶୂନ୍ୟ);
ବାହ୍ୟ uint64 sys_open(ଶୂନ୍ୟ);
ବାହ୍ୟ uint64 sys_write(ଶୂନ୍ୟ);
ବାହ୍ୟ uint64 sys_mknod(ଶୂନ୍ୟ);
ବାହ୍ୟ uint64 sys_unlink(ଶୂନ୍ୟ);
ବାହ୍ୟ uint64 sys_link(ଶୂନ୍ୟ);
ବାହ୍ୟ uint64 sys_mkdir(ଶୂନ୍ୟ);
ବାହ୍ୟ uint64 sys_close(ଶୂନ୍ୟ);
ବାହ୍ୟ uint64 sys_sync(ଶୂନ୍ୟ);

// An array mapping syscall numbers from syscall.h
// to the function that handles the system call.
ସ୍ଥିର uint64 (*syscalls[])(ଶୂନ୍ୟ) = {
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

ଶୂନ୍ୟ
syscall(ଶୂନ୍ୟ)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ;
  ସଂରଚନା proc *p = myproc();

  ଅଙ୍କ = p->trapframe->a7;
  ଯଦି (ଅଙ୍କ > 0 && ଅଙ୍କ < NELEM(syscalls) && syscalls[ଅଙ୍କ]) {
    // Use num to lookup the system call function for num, call it,
    // and store its return value in p->trapframe->a0
    p->trapframe->a0 = syscalls[ଅଙ୍କ]();
  } ଅନ୍ୟଥା {
    printk("%d %s: unknown sys call %d\n", p->pid, p->name, ଅଙ୍କ);
    p->trapframe->a0 = -1;
  }
}
