#অন্তর্ভাব "types.h"
#অন্তর্ভাব "param.h"
#অন্তর্ভাব "memlayout.h"
#অন্তর্ভাব "riscv.h"
#অন্তর্ভাব "spinlock.h"
#অন্তর্ভাব "proc.h"
#অন্তর্ভাব "syscall.h"
#অন্তর্ভাব "defs.h"

// Fetch the uint64 at addr from the current process.
পূর্ণসংখ্যা
fetchaddr(uint64 addr, uint64 *ip)
{
  কাঠামো proc *p = myproc();
  যদি (addr >= p->sz ||
      addr + আকার(uint64) > p->sz) // both tests needed, in case of overflow
    ফেরত -1;
  যদি (copyin(p->pagetable, p->sz, (অক্ষর *)ip, addr, আকার(*ip)) != 0)
    ফেরত -1;
  ফেরত 0;
}

// Fetch the nul-terminated string at addr from the current process.
// Returns length of string, not including nul, or -1 for error.
পূর্ণসংখ্যা
fetchstr(uint64 addr, অক্ষর *buf, পূর্ণসংখ্যা max)
{
  কাঠামো proc *p = myproc();
  যদি (copyinstr(p->pagetable, p->sz, buf, addr, max) < 0)
    ফেরত -1;
  ফেরত strlen(buf);
}

স্থির uint64
argraw(পূর্ণসংখ্যা n)
{
  কাঠামো proc *p = myproc();
  নির্বাচন (n) {
  ক্ষেত্র 0:
    ফেরত p->trapframe->a0;
  ক্ষেত্র 1:
    ফেরত p->trapframe->a1;
  ক্ষেত্র 2:
    ফেরত p->trapframe->a2;
  ক্ষেত্র 3:
    ফেরত p->trapframe->a3;
  ক্ষেত্র 4:
    ফেরত p->trapframe->a4;
  ক্ষেত্র 5:
    ফেরত p->trapframe->a5;
  }
  panic("argraw");
  ফেরত -1;
}

// Fetch the nth 32-bit system call argument.
শূন্য
argint(পূর্ণসংখ্যা n, পূর্ণসংখ্যা *ip)
{
  *ip = argraw(n);
}

// Retrieve an argument as a pointer.
// Doesn't check for legality, since
// copyin/copyout will do that.
শূন্য
argaddr(পূর্ণসংখ্যা n, uint64 *ip)
{
  *ip = argraw(n);
}

// Fetch the nth word-sized system call argument as a null-terminated string.
// Copies into buf, at most max.
// Returns string length if OK (not including nul), -1 if error.
পূর্ণসংখ্যা
argstr(পূর্ণসংখ্যা n, অক্ষর *buf, পূর্ণসংখ্যা max)
{
  uint64 addr;
  argaddr(n, &addr);
  ফেরত fetchstr(addr, buf, max);
}

// Prototypes for the functions that handle system calls.
বাহ্যিক uint64 sys_fork(শূন্য);
বাহ্যিক uint64 sys_exit(শূন্য);
বাহ্যিক uint64 sys_wait(শূন্য);
বাহ্যিক uint64 sys_pipe(শূন্য);
বাহ্যিক uint64 sys_read(শূন্য);
বাহ্যিক uint64 sys_kill(শূন্য);
বাহ্যিক uint64 sys_exec(শূন্য);
বাহ্যিক uint64 sys_fstat(শূন্য);
বাহ্যিক uint64 sys_chdir(শূন্য);
বাহ্যিক uint64 sys_dup(শূন্য);
বাহ্যিক uint64 sys_getpid(শূন্য);
বাহ্যিক uint64 sys_sbrk(শূন্য);
বাহ্যিক uint64 sys_pause(শূন্য);
বাহ্যিক uint64 sys_uptime(শূন্য);
বাহ্যিক uint64 sys_open(শূন্য);
বাহ্যিক uint64 sys_write(শূন্য);
বাহ্যিক uint64 sys_mknod(শূন্য);
বাহ্যিক uint64 sys_unlink(শূন্য);
বাহ্যিক uint64 sys_link(শূন্য);
বাহ্যিক uint64 sys_mkdir(শূন্য);
বাহ্যিক uint64 sys_close(শূন্য);
বাহ্যিক uint64 sys_sync(শূন্য);

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
  পূর্ণসংখ্যা অঙ্ক;
  কাঠামো proc *p = myproc();

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
