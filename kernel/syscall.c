#અન્તર્ભાવ "types.h"
#અન્તર્ભાવ "param.h"
#અન્તર્ભાવ "memlayout.h"
#અન્તર્ભાવ "riscv.h"
#અન્તર્ભાવ "spinlock.h"
#અન્તર્ભાવ "proc.h"
#અન્તર્ભાવ "syscall.h"
#અન્તર્ભાવ "defs.h"

// Fetch the uint64 at addr from the current process.
પૂર્ણાંક
fetchaddr(uint64 addr, uint64 *ip)
{
  સંરચના proc *p = myproc();
  જો (addr >= p->sz ||
      addr + કદ(uint64) > p->sz) // both tests needed, in case of overflow
    પાછા -1;
  જો (copyin(p->pagetable, p->sz, (અક્ષર *)ip, addr, કદ(*ip)) != 0)
    પાછા -1;
  પાછા 0;
}

// Fetch the nul-terminated string at addr from the current process.
// Returns length of string, not including nul, or -1 for error.
પૂર્ણાંક
fetchstr(uint64 addr, અક્ષર *buf, પૂર્ણાંક max)
{
  સંરચના proc *p = myproc();
  જો (copyinstr(p->pagetable, p->sz, buf, addr, max) < 0)
    પાછા -1;
  પાછા strlen(buf);
}

સ્થિર uint64
argraw(પૂર્ણાંક n)
{
  સંરચના proc *p = myproc();
  પસંદગી (n) {
  સ્થિતિ 0:
    પાછા p->trapframe->a0;
  સ્થિતિ 1:
    પાછા p->trapframe->a1;
  સ્થિતિ 2:
    પાછા p->trapframe->a2;
  સ્થિતિ 3:
    પાછા p->trapframe->a3;
  સ્થિતિ 4:
    પાછા p->trapframe->a4;
  સ્થિતિ 5:
    પાછા p->trapframe->a5;
  }
  panic("argraw");
  પાછા -1;
}

// Fetch the nth 32-bit system call argument.
શૂન્ય
argint(પૂર્ણાંક n, પૂર્ણાંક *ip)
{
  *ip = argraw(n);
}

// Retrieve an argument as a pointer.
// Doesn't check for legality, since
// copyin/copyout will do that.
શૂન્ય
argaddr(પૂર્ણાંક n, uint64 *ip)
{
  *ip = argraw(n);
}

// Fetch the nth word-sized system call argument as a null-terminated string.
// Copies into buf, at most max.
// Returns string length if OK (not including nul), -1 if error.
પૂર્ણાંક
argstr(પૂર્ણાંક n, અક્ષર *buf, પૂર્ણાંક max)
{
  uint64 addr;
  argaddr(n, &addr);
  પાછા fetchstr(addr, buf, max);
}

// Prototypes for the functions that handle system calls.
બાહ્ય uint64 sys_fork(શૂન્ય);
બાહ્ય uint64 sys_exit(શૂન્ય);
બાહ્ય uint64 sys_wait(શૂન્ય);
બાહ્ય uint64 sys_pipe(શૂન્ય);
બાહ્ય uint64 sys_read(શૂન્ય);
બાહ્ય uint64 sys_kill(શૂન્ય);
બાહ્ય uint64 sys_exec(શૂન્ય);
બાહ્ય uint64 sys_fstat(શૂન્ય);
બાહ્ય uint64 sys_chdir(શૂન્ય);
બાહ્ય uint64 sys_dup(શૂન્ય);
બાહ્ય uint64 sys_getpid(શૂન્ય);
બાહ્ય uint64 sys_sbrk(શૂન્ય);
બાહ્ય uint64 sys_pause(શૂન્ય);
બાહ્ય uint64 sys_uptime(શૂન્ય);
બાહ્ય uint64 sys_open(શૂન્ય);
બાહ્ય uint64 sys_write(શૂન્ય);
બાહ્ય uint64 sys_mknod(શૂન્ય);
બાહ્ય uint64 sys_unlink(શૂન્ય);
બાહ્ય uint64 sys_link(શૂન્ય);
બાહ્ય uint64 sys_mkdir(શૂન્ય);
બાહ્ય uint64 sys_close(શૂન્ય);
બાહ્ય uint64 sys_sync(શૂન્ય);

// An array mapping syscall numbers from syscall.h
// to the function that handles the system call.
સ્થિર uint64 (*syscalls[])(શૂન્ય) = {
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

શૂન્ય
syscall(શૂન્ય)
{
  પૂર્ણાંક અંક;
  સંરચના proc *p = myproc();

  અંક = p->trapframe->a7;
  જો (અંક > 0 && અંક < NELEM(syscalls) && syscalls[અંક]) {
    // Use num to lookup the system call function for num, call it,
    // and store its return value in p->trapframe->a0
    p->trapframe->a0 = syscalls[અંક]();
  } નહીંતર {
    printk("%d %s: unknown sys call %d\n", p->pid, p->name, અંક);
    p->trapframe->a0 = -1;
  }
}
