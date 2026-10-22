#شامل "types.h"
#شامل "param.h"
#شامل "memlayout.h"
#شامل "riscv.h"
#شامل "spinlock.h"
#شامل "proc.h"
#شامل "syscall.h"
#شامل "defs.h"

// Fetch the uint64 at addr from the current process.
صحیح
fetchaddr(uint64 addr, uint64 *ip)
{
  ساختار proc *p = myproc();
  اگر (addr >= p->sz ||
      addr + اندازه(uint64) > p->sz) // both tests needed, in case of overflow
    بازگشت -1;
  اگر (copyin(p->pagetable, p->sz, (نویسه *)ip, addr, اندازه(*ip)) != 0)
    بازگشت -1;
  بازگشت 0;
}

// Fetch the nul-terminated string at addr from the current process.
// Returns length of string, not including nul, or -1 for error.
صحیح
fetchstr(uint64 addr, نویسه *buf, صحیح max)
{
  ساختار proc *p = myproc();
  اگر (copyinstr(p->pagetable, p->sz, buf, addr, max) < 0)
    بازگشت -1;
  بازگشت strlen(buf);
}

ایستا uint64
argraw(صحیح n)
{
  ساختار proc *p = myproc();
  انتخاب (n) {
  حالت 0:
    بازگشت p->trapframe->a0;
  حالت 1:
    بازگشت p->trapframe->a1;
  حالت 2:
    بازگشت p->trapframe->a2;
  حالت 3:
    بازگشت p->trapframe->a3;
  حالت 4:
    بازگشت p->trapframe->a4;
  حالت 5:
    بازگشت p->trapframe->a5;
  }
  panic("argraw");
  بازگشت -1;
}

// Fetch the nth 32-bit system call argument.
تهی
argint(صحیح n, صحیح *ip)
{
  *ip = argraw(n);
}

// Retrieve an argument as a pointer.
// Doesn't check for legality, since
// copyin/copyout will do that.
تهی
argaddr(صحیح n, uint64 *ip)
{
  *ip = argraw(n);
}

// Fetch the nth word-sized system call argument as a null-terminated string.
// Copies into buf, at most max.
// Returns string length if OK (not including nul), -1 if error.
صحیح
argstr(صحیح n, نویسه *buf, صحیح max)
{
  uint64 addr;
  argaddr(n, &addr);
  بازگشت fetchstr(addr, buf, max);
}

// Prototypes for the functions that handle system calls.
بیرونی uint64 sys_fork(تهی);
بیرونی uint64 sys_exit(تهی);
بیرونی uint64 sys_wait(تهی);
بیرونی uint64 sys_pipe(تهی);
بیرونی uint64 sys_read(تهی);
بیرونی uint64 sys_kill(تهی);
بیرونی uint64 sys_exec(تهی);
بیرونی uint64 sys_fstat(تهی);
بیرونی uint64 sys_chdir(تهی);
بیرونی uint64 sys_dup(تهی);
بیرونی uint64 sys_getpid(تهی);
بیرونی uint64 sys_sbrk(تهی);
بیرونی uint64 sys_pause(تهی);
بیرونی uint64 sys_uptime(تهی);
بیرونی uint64 sys_open(تهی);
بیرونی uint64 sys_write(تهی);
بیرونی uint64 sys_mknod(تهی);
بیرونی uint64 sys_unlink(تهی);
بیرونی uint64 sys_link(تهی);
بیرونی uint64 sys_mkdir(تهی);
بیرونی uint64 sys_close(تهی);
بیرونی uint64 sys_sync(تهی);

// An array mapping syscall numbers from syscall.h
// to the function that handles the system call.
ایستا uint64 (*syscalls[])(تهی) = {
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

تهی
syscall(تهی)
{
  صحیح رقم;
  ساختار proc *p = myproc();

  رقم = p->trapframe->a7;
  اگر (رقم > 0 && رقم < NELEM(syscalls) && syscalls[رقم]) {
    // Use num to lookup the system call function for num, call it,
    // and store its return value in p->trapframe->a0
    p->trapframe->a0 = syscalls[رقم]();
  } وگرنه {
    printk("%d %s: unknown sys call %d\n", p->pid, p->name, رقم);
    p->trapframe->a0 = -1;
  }
}
