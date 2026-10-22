#అన్తర్భావ "types.h"
#అన్తర్భావ "param.h"
#అన్తర్భావ "memlayout.h"
#అన్తర్భావ "riscv.h"
#అన్తర్భావ "spinlock.h"
#అన్తర్భావ "proc.h"
#అన్తర్భావ "syscall.h"
#అన్తర్భావ "defs.h"

// Fetch the uint64 at addr from the current process.
పూర్ణాంకం
fetchaddr(uint64 addr, uint64 *ip)
{
  నిర్మాణం proc *p = myproc();
  ఒకవేళ (addr >= p->sz ||
      addr + పరిమాణం(uint64) > p->sz) // both tests needed, in case of overflow
    తిరిగి -1;
  ఒకవేళ (copyin(p->pagetable, p->sz, (అక్షరం *)ip, addr, పరిమాణం(*ip)) != 0)
    తిరిగి -1;
  తిరిగి 0;
}

// Fetch the nul-terminated string at addr from the current process.
// Returns length of string, not including nul, or -1 for error.
పూర్ణాంకం
fetchstr(uint64 addr, అక్షరం *buf, పూర్ణాంకం max)
{
  నిర్మాణం proc *p = myproc();
  ఒకవేళ (copyinstr(p->pagetable, p->sz, buf, addr, max) < 0)
    తిరిగి -1;
  తిరిగి అక్షరశ్రేణి_పొడవు(buf);
}

స్థిర uint64
argraw(పూర్ణాంకం n)
{
  నిర్మాణం proc *p = myproc();
  ఎంపిక (n) {
  స్థితి 0:
    తిరిగి p->trapframe->a0;
  స్థితి 1:
    తిరిగి p->trapframe->a1;
  స్థితి 2:
    తిరిగి p->trapframe->a2;
  స్థితి 3:
    తిరిగి p->trapframe->a3;
  స్థితి 4:
    తిరిగి p->trapframe->a4;
  స్థితి 5:
    తిరిగి p->trapframe->a5;
  }
  panic("argraw");
  తిరిగి -1;
}

// Fetch the nth 32-bit system call argument.
శూన్య
argint(పూర్ణాంకం n, పూర్ణాంకం *ip)
{
  *ip = argraw(n);
}

// Retrieve an argument as a pointer.
// Doesn't check for legality, since
// copyin/copyout will do that.
శూన్య
argaddr(పూర్ణాంకం n, uint64 *ip)
{
  *ip = argraw(n);
}

// Fetch the nth word-sized system call argument as a null-terminated string.
// Copies into buf, at most max.
// Returns string length if OK (not including nul), -1 if error.
పూర్ణాంకం
argstr(పూర్ణాంకం n, అక్షరం *buf, పూర్ణాంకం max)
{
  uint64 addr;
  argaddr(n, &addr);
  తిరిగి fetchstr(addr, buf, max);
}

// Prototypes for the functions that handle system calls.
బాహ్య uint64 sys_fork(శూన్య);
బాహ్య uint64 sys_exit(శూన్య);
బాహ్య uint64 sys_wait(శూన్య);
బాహ్య uint64 sys_pipe(శూన్య);
బాహ్య uint64 sys_read(శూన్య);
బాహ్య uint64 sys_kill(శూన్య);
బాహ్య uint64 sys_exec(శూన్య);
బాహ్య uint64 sys_fstat(శూన్య);
బాహ్య uint64 sys_chdir(శూన్య);
బాహ్య uint64 sys_dup(శూన్య);
బాహ్య uint64 sys_getpid(శూన్య);
బాహ్య uint64 sys_sbrk(శూన్య);
బాహ్య uint64 sys_pause(శూన్య);
బాహ్య uint64 sys_uptime(శూన్య);
బాహ్య uint64 sys_open(శూన్య);
బాహ్య uint64 sys_write(శూన్య);
బాహ్య uint64 sys_mknod(శూన్య);
బాహ్య uint64 sys_unlink(శూన్య);
బాహ్య uint64 sys_link(శూన్య);
బాహ్య uint64 sys_mkdir(శూన్య);
బాహ్య uint64 sys_close(శూన్య);
బాహ్య uint64 sys_sync(శూన్య);

// An array mapping syscall numbers from syscall.h
// to the function that handles the system call.
స్థిర uint64 (*syscalls[])(శూన్య) = {
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

శూన్య
syscall(శూన్య)
{
  పూర్ణాంకం అంకె;
  నిర్మాణం proc *p = myproc();

  అంకె = p->trapframe->a7;
  ఒకవేళ (అంకె > 0 && అంకె < NELEM(syscalls) && syscalls[అంకె]) {
    // Use num to lookup the system call function for num, call it,
    // and store its return value in p->trapframe->a0
    p->trapframe->a0 = syscalls[అంకె]();
  } లేకపోతే {
    printk("%d %s: unknown sys call %d\n", p->pid, p->name, అంకె);
    p->trapframe->a0 = -1;
  }
}
