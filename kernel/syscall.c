#അന്തര്ഭാവ "types.h"
#അന്തര്ഭാവ "param.h"
#അന്തര്ഭാവ "memlayout.h"
#അന്തര്ഭാവ "riscv.h"
#അന്തര്ഭാവ "spinlock.h"
#അന്തര്ഭാവ "proc.h"
#അന്തര്ഭാവ "syscall.h"
#അന്തര്ഭാവ "defs.h"

// Fetch the uint64 at addr from the current process.
പൂർണ്ണസംഖ്യ
fetchaddr(uint64 addr, uint64 *ip)
{
  ഘടന proc *p = myproc();
  എങ്കിൽ (addr >= p->sz ||
      addr + വലുപ്പം(uint64) > p->sz) // both tests needed, in case of overflow
    തിരികെ -1;
  എങ്കിൽ (copyin(p->pagetable, p->sz, (അക്ഷരം *)ip, addr, വലുപ്പം(*ip)) != 0)
    തിരികെ -1;
  തിരികെ 0;
}

// Fetch the nul-terminated string at addr from the current process.
// Returns length of string, not including nul, or -1 for error.
പൂർണ്ണസംഖ്യ
fetchstr(uint64 addr, അക്ഷരം *buf, പൂർണ്ണസംഖ്യ max)
{
  ഘടന proc *p = myproc();
  എങ്കിൽ (copyinstr(p->pagetable, p->sz, buf, addr, max) < 0)
    തിരികെ -1;
  തിരികെ strlen(buf);
}

സ്ഥിതം uint64
argraw(പൂർണ്ണസംഖ്യ n)
{
  ഘടന proc *p = myproc();
  തിരഞ്ഞെടുപ്പ് (n) {
  അവസ്ഥ 0:
    തിരികെ p->trapframe->a0;
  അവസ്ഥ 1:
    തിരികെ p->trapframe->a1;
  അവസ്ഥ 2:
    തിരികെ p->trapframe->a2;
  അവസ്ഥ 3:
    തിരികെ p->trapframe->a3;
  അവസ്ഥ 4:
    തിരികെ p->trapframe->a4;
  അവസ്ഥ 5:
    തിരികെ p->trapframe->a5;
  }
  panic("argraw");
  തിരികെ -1;
}

// Fetch the nth 32-bit system call argument.
ശൂന്യം
argint(പൂർണ്ണസംഖ്യ n, പൂർണ്ണസംഖ്യ *ip)
{
  *ip = argraw(n);
}

// Retrieve an argument as a pointer.
// Doesn't check for legality, since
// copyin/copyout will do that.
ശൂന്യം
argaddr(പൂർണ്ണസംഖ്യ n, uint64 *ip)
{
  *ip = argraw(n);
}

// Fetch the nth word-sized system call argument as a null-terminated string.
// Copies into buf, at most max.
// Returns string length if OK (not including nul), -1 if error.
പൂർണ്ണസംഖ്യ
argstr(പൂർണ്ണസംഖ്യ n, അക്ഷരം *buf, പൂർണ്ണസംഖ്യ max)
{
  uint64 addr;
  argaddr(n, &addr);
  തിരികെ fetchstr(addr, buf, max);
}

// Prototypes for the functions that handle system calls.
ബാഹ്യ uint64 sys_fork(ശൂന്യം);
ബാഹ്യ uint64 sys_exit(ശൂന്യം);
ബാഹ്യ uint64 sys_wait(ശൂന്യം);
ബാഹ്യ uint64 sys_pipe(ശൂന്യം);
ബാഹ്യ uint64 sys_read(ശൂന്യം);
ബാഹ്യ uint64 sys_kill(ശൂന്യം);
ബാഹ്യ uint64 sys_exec(ശൂന്യം);
ബാഹ്യ uint64 sys_fstat(ശൂന്യം);
ബാഹ്യ uint64 sys_chdir(ശൂന്യം);
ബാഹ്യ uint64 sys_dup(ശൂന്യം);
ബാഹ്യ uint64 sys_getpid(ശൂന്യം);
ബാഹ്യ uint64 sys_sbrk(ശൂന്യം);
ബാഹ്യ uint64 sys_pause(ശൂന്യം);
ബാഹ്യ uint64 sys_uptime(ശൂന്യം);
ബാഹ്യ uint64 sys_open(ശൂന്യം);
ബാഹ്യ uint64 sys_write(ശൂന്യം);
ബാഹ്യ uint64 sys_mknod(ശൂന്യം);
ബാഹ്യ uint64 sys_unlink(ശൂന്യം);
ബാഹ്യ uint64 sys_link(ശൂന്യം);
ബാഹ്യ uint64 sys_mkdir(ശൂന്യം);
ബാഹ്യ uint64 sys_close(ശൂന്യം);
ബാഹ്യ uint64 sys_sync(ശൂന്യം);

// An array mapping syscall numbers from syscall.h
// to the function that handles the system call.
സ്ഥിതം uint64 (*syscalls[])(ശൂന്യം) = {
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

ശൂന്യം
syscall(ശൂന്യം)
{
  പൂർണ്ണസംഖ്യ അക്കം;
  ഘടന proc *p = myproc();

  അക്കം = p->trapframe->a7;
  എങ്കിൽ (അക്കം > 0 && അക്കം < NELEM(syscalls) && syscalls[അക്കം]) {
    // Use num to lookup the system call function for num, call it,
    // and store its return value in p->trapframe->a0
    p->trapframe->a0 = syscalls[അക്കം]();
  } അല്ലെങ്കിൽ {
    printk("%d %s: unknown sys call %d\n", p->pid, p->name, അക്കം);
    p->trapframe->a0 = -1;
  }
}
