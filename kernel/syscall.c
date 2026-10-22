#ਅਨ੍ਤਰ੍ਭਾਵ "types.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "param.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "memlayout.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "riscv.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "spinlock.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "proc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "syscall.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "defs.h"

// Fetch the uint64 at addr from the current process.
ਪੂਰਨ
fetchaddr(uint64 addr, uint64 *ip)
{
  ਬਣਤਰ proc *p = myproc();
  ਜੇ (addr >= p->sz ||
      addr + ਆਕਾਰ(uint64) > p->sz) // both tests needed, in case of overflow
    ਵਾਪਸ -1;
  ਜੇ (copyin(p->pagetable, p->sz, (ਅੱਖਰ *)ip, addr, ਆਕਾਰ(*ip)) != 0)
    ਵਾਪਸ -1;
  ਵਾਪਸ 0;
}

// Fetch the nul-terminated string at addr from the current process.
// Returns length of string, not including nul, or -1 for error.
ਪੂਰਨ
fetchstr(uint64 addr, ਅੱਖਰ *buf, ਪੂਰਨ max)
{
  ਬਣਤਰ proc *p = myproc();
  ਜੇ (copyinstr(p->pagetable, p->sz, buf, addr, max) < 0)
    ਵਾਪਸ -1;
  ਵਾਪਸ ਲੜੀ_ਲੰਬਾਈ(buf);
}

ਸਥਿਰ uint64
argraw(ਪੂਰਨ n)
{
  ਬਣਤਰ proc *p = myproc();
  ਚੋਣ (n) {
  ਹਾਲਤ 0:
    ਵਾਪਸ p->trapframe->a0;
  ਹਾਲਤ 1:
    ਵਾਪਸ p->trapframe->a1;
  ਹਾਲਤ 2:
    ਵਾਪਸ p->trapframe->a2;
  ਹਾਲਤ 3:
    ਵਾਪਸ p->trapframe->a3;
  ਹਾਲਤ 4:
    ਵਾਪਸ p->trapframe->a4;
  ਹਾਲਤ 5:
    ਵਾਪਸ p->trapframe->a5;
  }
  panic("argraw");
  ਵਾਪਸ -1;
}

// Fetch the nth 32-bit system call argument.
ਖਾਲੀ
argint(ਪੂਰਨ n, ਪੂਰਨ *ip)
{
  *ip = argraw(n);
}

// Retrieve an argument as a pointer.
// Doesn't check for legality, since
// copyin/copyout will do that.
ਖਾਲੀ
argaddr(ਪੂਰਨ n, uint64 *ip)
{
  *ip = argraw(n);
}

// Fetch the nth word-sized system call argument as a null-terminated string.
// Copies into buf, at most max.
// Returns string length if OK (not including nul), -1 if error.
ਪੂਰਨ
argstr(ਪੂਰਨ n, ਅੱਖਰ *buf, ਪੂਰਨ max)
{
  uint64 addr;
  argaddr(n, &addr);
  ਵਾਪਸ fetchstr(addr, buf, max);
}

// Prototypes for the functions that handle system calls.
ਬਾਹਰੀ uint64 sys_fork(ਖਾਲੀ);
ਬਾਹਰੀ uint64 sys_exit(ਖਾਲੀ);
ਬਾਹਰੀ uint64 sys_wait(ਖਾਲੀ);
ਬਾਹਰੀ uint64 sys_pipe(ਖਾਲੀ);
ਬਾਹਰੀ uint64 sys_read(ਖਾਲੀ);
ਬਾਹਰੀ uint64 sys_kill(ਖਾਲੀ);
ਬਾਹਰੀ uint64 sys_exec(ਖਾਲੀ);
ਬਾਹਰੀ uint64 sys_fstat(ਖਾਲੀ);
ਬਾਹਰੀ uint64 sys_chdir(ਖਾਲੀ);
ਬਾਹਰੀ uint64 sys_dup(ਖਾਲੀ);
ਬਾਹਰੀ uint64 sys_getpid(ਖਾਲੀ);
ਬਾਹਰੀ uint64 sys_sbrk(ਖਾਲੀ);
ਬਾਹਰੀ uint64 sys_pause(ਖਾਲੀ);
ਬਾਹਰੀ uint64 sys_uptime(ਖਾਲੀ);
ਬਾਹਰੀ uint64 sys_open(ਖਾਲੀ);
ਬਾਹਰੀ uint64 sys_write(ਖਾਲੀ);
ਬਾਹਰੀ uint64 sys_mknod(ਖਾਲੀ);
ਬਾਹਰੀ uint64 sys_unlink(ਖਾਲੀ);
ਬਾਹਰੀ uint64 sys_link(ਖਾਲੀ);
ਬਾਹਰੀ uint64 sys_mkdir(ਖਾਲੀ);
ਬਾਹਰੀ uint64 sys_close(ਖਾਲੀ);
ਬਾਹਰੀ uint64 sys_sync(ਖਾਲੀ);

// An array mapping syscall numbers from syscall.h
// to the function that handles the system call.
ਸਥਿਰ uint64 (*syscalls[])(ਖਾਲੀ) = {
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

ਖਾਲੀ
syscall(ਖਾਲੀ)
{
  ਪੂਰਨ ਅੰਕ;
  ਬਣਤਰ proc *p = myproc();

  ਅੰਕ = p->trapframe->a7;
  ਜੇ (ਅੰਕ > 0 && ਅੰਕ < NELEM(syscalls) && syscalls[ਅੰਕ]) {
    // Use num to lookup the system call function for num, call it,
    // and store its return value in p->trapframe->a0
    p->trapframe->a0 = syscalls[ਅੰਕ]();
  } ਨਹੀਂਤਾਂ {
    printk("%d %s: unknown sys call %d\n", p->pid, p->name, ਅੰਕ);
    p->trapframe->a0 = -1;
  }
}
