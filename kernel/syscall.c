#অন্তর্ভাৱ "types.h"
#অন্তর্ভাৱ "param.h"
#অন্তর্ভাৱ "memlayout.h"
#অন্তর্ভাৱ "riscv.h"
#অন্তর্ভাৱ "spinlock.h"
#অন্তর্ভাৱ "proc.h"
#অন্তর্ভাৱ "syscall.h"
#অন্তর্ভাৱ "defs.h"

// Fetch the uint64 at addr from the current process.
পূর্ণাঙ্ক
fetchaddr(uint64 addr, uint64 *ip)
{
  সংরচনা proc *p = myproc();
  যদি (addr >= p->sz ||
      addr + পরিমাণ(uint64) > p->sz) // both tests needed, in case of overflow
    ঘূৰাই -1;
  যদি (copyin(p->pagetable, p->sz, (ৱর্ণ *)ip, addr, পরিমাণ(*ip)) != 0)
    ঘূৰাই -1;
  ঘূৰাই 0;
}

// Fetch the nul-terminated string at addr from the current process.
// Returns length of string, not including nul, or -1 for error.
পূর্ণাঙ্ক
fetchstr(uint64 addr, ৱর্ণ *buf, পূর্ণাঙ্ক max)
{
  সংরচনা proc *p = myproc();
  যদি (copyinstr(p->pagetable, p->sz, buf, addr, max) < 0)
    ঘূৰাই -1;
  ঘূৰাই strlen(buf);
}

স্থির uint64
argraw(পূর্ণাঙ্ক n)
{
  সংরচনা proc *p = myproc();
  ৱিকল্প (n) {
  পক্ষ 0:
    ঘূৰাই p->trapframe->a0;
  পক্ষ 1:
    ঘূৰাই p->trapframe->a1;
  পক্ষ 2:
    ঘূৰাই p->trapframe->a2;
  পক্ষ 3:
    ঘূৰাই p->trapframe->a3;
  পক্ষ 4:
    ঘূৰাই p->trapframe->a4;
  পক্ষ 5:
    ঘূৰাই p->trapframe->a5;
  }
  panic("argraw");
  ঘূৰাই -1;
}

// Fetch the nth 32-bit system call argument.
শূন্য
argint(পূর্ণাঙ্ক n, পূর্ণাঙ্ক *ip)
{
  *ip = argraw(n);
}

// Retrieve an argument as a pointer.
// Doesn't check for legality, since
// copyin/copyout will do that.
শূন্য
argaddr(পূর্ণাঙ্ক n, uint64 *ip)
{
  *ip = argraw(n);
}

// Fetch the nth word-sized system call argument as a null-terminated string.
// Copies into buf, at most max.
// Returns string length if OK (not including nul), -1 if error.
পূর্ণাঙ্ক
argstr(পূর্ণাঙ্ক n, ৱর্ণ *buf, পূর্ণাঙ্ক max)
{
  uint64 addr;
  argaddr(n, &addr);
  ঘূৰাই fetchstr(addr, buf, max);
}

// Prototypes for the functions that handle system calls.
বাহ্য uint64 sys_fork(শূন্য);
বাহ্য uint64 sys_exit(শূন্য);
বাহ্য uint64 sys_wait(শূন্য);
বাহ্য uint64 sys_pipe(শূন্য);
বাহ্য uint64 sys_read(শূন্য);
বাহ্য uint64 sys_kill(শূন্য);
বাহ্য uint64 sys_exec(শূন্য);
বাহ্য uint64 sys_fstat(শূন্য);
বাহ্য uint64 sys_chdir(শূন্য);
বাহ্য uint64 sys_dup(শূন্য);
বাহ্য uint64 sys_getpid(শূন্য);
বাহ্য uint64 sys_sbrk(শূন্য);
বাহ্য uint64 sys_pause(শূন্য);
বাহ্য uint64 sys_uptime(শূন্য);
বাহ্য uint64 sys_open(শূন্য);
বাহ্য uint64 sys_write(শূন্য);
বাহ্য uint64 sys_mknod(শূন্য);
বাহ্য uint64 sys_unlink(শূন্য);
বাহ্য uint64 sys_link(শূন্য);
বাহ্য uint64 sys_mkdir(শূন্য);
বাহ্য uint64 sys_close(শূন্য);
বাহ্য uint64 sys_sync(শূন্য);

// An array mapping syscall numbers from syscall.h
// to the function that handles the system call.
স্থির uint64 (*syscalls[])(শূন্য) = {
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

শূন্য
syscall(শূন্য)
{
  পূর্ণাঙ্ক অঙ্ক;
  সংরচনা proc *p = myproc();

  অঙ্ক = p->trapframe->a7;
  যদি (অঙ্ক > 0 && অঙ্ক < NELEM(syscalls) && syscalls[অঙ্ক]) {
    // Use num to lookup the system call function for num, call it,
    // and store its return value in p->trapframe->a0
    p->trapframe->a0 = syscalls[অঙ্ক]();
  } অন্যথা {
    printk("%d %s: unknown sys call %d\n", p->pid, p->name, অঙ্ক);
    p->trapframe->a0 = -1;
  }
}
