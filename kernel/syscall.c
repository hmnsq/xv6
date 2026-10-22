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
  ساخت proc *p = myproc();
  اگر (addr >= p->sz ||
      addr + حجم(uint64) > p->sz) // both tests needed, in case of overflow
    واپس -1;
  اگر (copyin(p->pagetable, p->sz, (حرف *)ip, addr, حجم(*ip)) != 0)
    واپس -1;
  واپس 0;
}

// Fetch the nul-terminated string at addr from the current process.
// Returns length of string, not including nul, or -1 for error.
صحیح
fetchstr(uint64 addr, حرف *buf, صحیح max)
{
  ساخت proc *p = myproc();
  اگر (copyinstr(p->pagetable, p->sz, buf, addr, max) < 0)
    واپس -1;
  واپس لڑی_لمبائی(buf);
}

ساکن uint64
argraw(صحیح n)
{
  ساخت proc *p = myproc();
  انتخاب (n) {
  حالت 0:
    واپس p->trapframe->a0;
  حالت 1:
    واپس p->trapframe->a1;
  حالت 2:
    واپس p->trapframe->a2;
  حالت 3:
    واپس p->trapframe->a3;
  حالت 4:
    واپس p->trapframe->a4;
  حالت 5:
    واپس p->trapframe->a5;
  }
  panic("argraw");
  واپس -1;
}

// Fetch the nth 32-bit system call argument.
خالی
argint(صحیح n, صحیح *ip)
{
  *ip = argraw(n);
}

// Retrieve an argument as a pointer.
// Doesn't check for legality, since
// copyin/copyout will do that.
خالی
argaddr(صحیح n, uint64 *ip)
{
  *ip = argraw(n);
}

// Fetch the nth word-sized system call argument as a null-terminated string.
// Copies into buf, at most max.
// Returns string length if OK (not including nul), -1 if error.
صحیح
argstr(صحیح n, حرف *buf, صحیح max)
{
  uint64 addr;
  argaddr(n, &addr);
  واپس fetchstr(addr, buf, max);
}

// Prototypes for the functions that handle system calls.
خارجی uint64 sys_fork(خالی);
خارجی uint64 sys_exit(خالی);
خارجی uint64 sys_wait(خالی);
خارجی uint64 sys_pipe(خالی);
خارجی uint64 sys_read(خالی);
خارجی uint64 sys_kill(خالی);
خارجی uint64 sys_exec(خالی);
خارجی uint64 sys_fstat(خالی);
خارجی uint64 sys_chdir(خالی);
خارجی uint64 sys_dup(خالی);
خارجی uint64 sys_getpid(خالی);
خارجی uint64 sys_sbrk(خالی);
خارجی uint64 sys_pause(خالی);
خارجی uint64 sys_uptime(خالی);
خارجی uint64 sys_open(خالی);
خارجی uint64 sys_write(خالی);
خارجی uint64 sys_mknod(خالی);
خارجی uint64 sys_unlink(خالی);
خارجی uint64 sys_link(خالی);
خارجی uint64 sys_mkdir(خالی);
خارجی uint64 sys_close(خالی);
خارجی uint64 sys_sync(خالی);

// An array mapping syscall numbers from syscall.h
// to the function that handles the system call.
ساکن uint64 (*syscalls[])(خالی) = {
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

خالی
syscall(خالی)
{
  صحیح عدد;
  ساخت proc *p = myproc();

  عدد = p->trapframe->a7;
  اگر (عدد > 0 && عدد < NELEM(syscalls) && syscalls[عدد]) {
    // Use num to lookup the system call function for num, call it,
    // and store its return value in p->trapframe->a0
    p->trapframe->a0 = syscalls[عدد]();
  } ورنہ {
    printk("%d %s: unknown sys call %d\n", p->pid, p->name, عدد);
    p->trapframe->a0 = -1;
  }
}
