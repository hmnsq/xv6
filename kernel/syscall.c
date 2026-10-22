#ضمن "types.h"
#ضمن "param.h"
#ضمن "memlayout.h"
#ضمن "riscv.h"
#ضمن "spinlock.h"
#ضمن "proc.h"
#ضمن "syscall.h"
#ضمن "defs.h"

// Fetch the uint64 at addr from the current process.
صحيح
fetchaddr(uint64 addr, uint64 *ip)
{
  بنية proc *p = myproc();
  إذا (addr >= p->sz ||
      addr + حجم(uint64) > p->sz) // both tests needed, in case of overflow
    إرجاع -1;
  إذا (copyin(p->pagetable, p->sz, (حرف *)ip, addr, حجم(*ip)) != 0)
    إرجاع -1;
  إرجاع 0;
}

// Fetch the nul-terminated string at addr from the current process.
// Returns length of string, not including nul, or -1 for error.
صحيح
fetchstr(uint64 addr, حرف *buf, صحيح max)
{
  بنية proc *p = myproc();
  إذا (copyinstr(p->pagetable, p->sz, buf, addr, max) < 0)
    إرجاع -1;
  إرجاع strlen(buf);
}

ساكن uint64
argraw(صحيح n)
{
  بنية proc *p = myproc();
  بدل (n) {
  حالة 0:
    إرجاع p->trapframe->a0;
  حالة 1:
    إرجاع p->trapframe->a1;
  حالة 2:
    إرجاع p->trapframe->a2;
  حالة 3:
    إرجاع p->trapframe->a3;
  حالة 4:
    إرجاع p->trapframe->a4;
  حالة 5:
    إرجاع p->trapframe->a5;
  }
  panic("argraw");
  إرجاع -1;
}

// Fetch the nth 32-bit system call argument.
فراغ
argint(صحيح n, صحيح *ip)
{
  *ip = argraw(n);
}

// Retrieve an argument as a pointer.
// Doesn't check for legality, since
// copyin/copyout will do that.
فراغ
argaddr(صحيح n, uint64 *ip)
{
  *ip = argraw(n);
}

// Fetch the nth word-sized system call argument as a null-terminated string.
// Copies into buf, at most max.
// Returns string length if OK (not including nul), -1 if error.
صحيح
argstr(صحيح n, حرف *buf, صحيح max)
{
  uint64 addr;
  argaddr(n, &addr);
  إرجاع fetchstr(addr, buf, max);
}

// Prototypes for the functions that handle system calls.
خارجي uint64 sys_fork(فراغ);
خارجي uint64 sys_exit(فراغ);
خارجي uint64 sys_wait(فراغ);
خارجي uint64 sys_pipe(فراغ);
خارجي uint64 sys_read(فراغ);
خارجي uint64 sys_kill(فراغ);
خارجي uint64 sys_exec(فراغ);
خارجي uint64 sys_fstat(فراغ);
خارجي uint64 sys_chdir(فراغ);
خارجي uint64 sys_dup(فراغ);
خارجي uint64 sys_getpid(فراغ);
خارجي uint64 sys_sbrk(فراغ);
خارجي uint64 sys_pause(فراغ);
خارجي uint64 sys_uptime(فراغ);
خارجي uint64 sys_open(فراغ);
خارجي uint64 sys_write(فراغ);
خارجي uint64 sys_mknod(فراغ);
خارجي uint64 sys_unlink(فراغ);
خارجي uint64 sys_link(فراغ);
خارجي uint64 sys_mkdir(فراغ);
خارجي uint64 sys_close(فراغ);
خارجي uint64 sys_sync(فراغ);

// An array mapping syscall numbers from syscall.h
// to the function that handles the system call.
ساكن uint64 (*syscalls[])(فراغ) = {
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

فراغ
syscall(فراغ)
{
  صحيح رقم;
  بنية proc *p = myproc();

  رقم = p->trapframe->a7;
  إذا (رقم > 0 && رقم < NELEM(syscalls) && syscalls[رقم]) {
    // Use num to lookup the system call function for num, call it,
    // and store its return value in p->trapframe->a0
    p->trapframe->a0 = syscalls[رقم]();
  } وإلا {
    printk("%d %s: unknown sys call %d\n", p->pid, p->name, رقم);
    p->trapframe->a0 = -1;
  }
}
