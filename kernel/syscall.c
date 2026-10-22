#समावेश "types.h"
#समावेश "param.h"
#समावेश "memlayout.h"
#समावेश "riscv.h"
#समावेश "spinlock.h"
#समावेश "proc.h"
#समावेश "syscall.h"
#समावेश "defs.h"

// Fetch the uint64 at addr from the current process.
पूर्णांक
fetchaddr(uint64 addr, uint64 *ip)
{
  काष्ठा proc *p = myproc();
  अगर (addr >= p->sz ||
      addr + माप(uint64) > p->sz) // both tests needed, in case of overflow
    वापस -1;
  अगर (copyin(p->pagetable, p->sz, (अक्षर *)ip, addr, माप(*ip)) != 0)
    वापस -1;
  वापस 0;
}

// Fetch the nul-terminated string at addr from the current process.
// Returns length of string, not including nul, or -1 for error.
पूर्णांक
fetchstr(uint64 addr, अक्षर *buf, पूर्णांक max)
{
  काष्ठा proc *p = myproc();
  अगर (copyinstr(p->pagetable, p->sz, buf, addr, max) < 0)
    वापस -1;
  वापस म_माप(buf);
}

अटल uint64
argraw(पूर्णांक n)
{
  काष्ठा proc *p = myproc();
  चयन (n) {
  हाल 0:
    वापस p->trapframe->a0;
  हाल 1:
    वापस p->trapframe->a1;
  हाल 2:
    वापस p->trapframe->a2;
  हाल 3:
    वापस p->trapframe->a3;
  हाल 4:
    वापस p->trapframe->a4;
  हाल 5:
    वापस p->trapframe->a5;
  }
  panic("argraw");
  वापस -1;
}

// Fetch the nth 32-bit system call argument.
व्योम
argint(पूर्णांक n, पूर्णांक *ip)
{
  *ip = argraw(n);
}

// Retrieve an argument as a pointer.
// Doesn't check for legality, since
// copyin/copyout will do that.
व्योम
argaddr(पूर्णांक n, uint64 *ip)
{
  *ip = argraw(n);
}

// Fetch the nth word-sized system call argument as a null-terminated string.
// Copies into buf, at most max.
// Returns string length if OK (not including nul), -1 if error.
पूर्णांक
argstr(पूर्णांक n, अक्षर *buf, पूर्णांक max)
{
  uint64 addr;
  argaddr(n, &addr);
  वापस fetchstr(addr, buf, max);
}

// Prototypes for the functions that handle system calls.
बाह्य uint64 sys_fork(व्योम);
बाह्य uint64 sys_exit(व्योम);
बाह्य uint64 sys_wait(व्योम);
बाह्य uint64 sys_pipe(व्योम);
बाह्य uint64 sys_read(व्योम);
बाह्य uint64 sys_kill(व्योम);
बाह्य uint64 sys_exec(व्योम);
बाह्य uint64 sys_fstat(व्योम);
बाह्य uint64 sys_chdir(व्योम);
बाह्य uint64 sys_dup(व्योम);
बाह्य uint64 sys_getpid(व्योम);
बाह्य uint64 sys_sbrk(व्योम);
बाह्य uint64 sys_pause(व्योम);
बाह्य uint64 sys_uptime(व्योम);
बाह्य uint64 sys_open(व्योम);
बाह्य uint64 sys_write(व्योम);
बाह्य uint64 sys_mknod(व्योम);
बाह्य uint64 sys_unlink(व्योम);
बाह्य uint64 sys_link(व्योम);
बाह्य uint64 sys_mkdir(व्योम);
बाह्य uint64 sys_close(व्योम);
बाह्य uint64 sys_sync(व्योम);

// An array mapping syscall numbers from syscall.h
// to the function that handles the system call.
अटल uint64 (*syscalls[])(व्योम) = {
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

व्योम
syscall(व्योम)
{
  पूर्णांक अंक;
  काष्ठा proc *p = myproc();

  अंक = p->trapframe->a7;
  अगर (अंक > 0 && अंक < NELEM(syscalls) && syscalls[अंक]) {
    // Use num to lookup the system call function for num, call it,
    // and store its return value in p->trapframe->a0
    p->trapframe->a0 = syscalls[अंक]();
  } अन्यथा {
    printk("%d %s: unknown sys call %d\n", p->pid, p->name, अंक);
    p->trapframe->a0 = -1;
  }
}
