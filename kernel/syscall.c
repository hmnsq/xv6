#ಅನ್ತರ್ಭಾವ "types.h"
#ಅನ್ತರ್ಭಾವ "param.h"
#ಅನ್ತರ್ಭಾವ "memlayout.h"
#ಅನ್ತರ್ಭಾವ "riscv.h"
#ಅನ್ತರ್ಭಾವ "spinlock.h"
#ಅನ್ತರ್ಭಾವ "proc.h"
#ಅನ್ತರ್ಭಾವ "syscall.h"
#ಅನ್ತರ್ಭಾವ "defs.h"

// Fetch the uint64 at addr from the current process.
ಪೂರ್ಣಾಂಕ
fetchaddr(uint64 addr, uint64 *ip)
{
  ರಚನೆ proc *p = myproc();
  ಆದರೆ (addr >= p->sz ||
      addr + ಗಾತ್ರ(uint64) > p->sz) // both tests needed, in case of overflow
    ಹಿಂತಿರುಗಿ -1;
  ಆದರೆ (copyin(p->pagetable, p->sz, (ಅಕ್ಷರ *)ip, addr, ಗಾತ್ರ(*ip)) != 0)
    ಹಿಂತಿರುಗಿ -1;
  ಹಿಂತಿರುಗಿ 0;
}

// Fetch the nul-terminated string at addr from the current process.
// Returns length of string, not including nul, or -1 for error.
ಪೂರ್ಣಾಂಕ
fetchstr(uint64 addr, ಅಕ್ಷರ *buf, ಪೂರ್ಣಾಂಕ max)
{
  ರಚನೆ proc *p = myproc();
  ಆದರೆ (copyinstr(p->pagetable, p->sz, buf, addr, max) < 0)
    ಹಿಂತಿರುಗಿ -1;
  ಹಿಂತಿರುಗಿ ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(buf);
}

ಸ್ಥಿರ uint64
argraw(ಪೂರ್ಣಾಂಕ n)
{
  ರಚನೆ proc *p = myproc();
  ಆಯ್ಕೆ (n) {
  ಸ್ಥಿತಿ 0:
    ಹಿಂತಿರುಗಿ p->trapframe->a0;
  ಸ್ಥಿತಿ 1:
    ಹಿಂತಿರುಗಿ p->trapframe->a1;
  ಸ್ಥಿತಿ 2:
    ಹಿಂತಿರುಗಿ p->trapframe->a2;
  ಸ್ಥಿತಿ 3:
    ಹಿಂತಿರುಗಿ p->trapframe->a3;
  ಸ್ಥಿತಿ 4:
    ಹಿಂತಿರುಗಿ p->trapframe->a4;
  ಸ್ಥಿತಿ 5:
    ಹಿಂತಿರುಗಿ p->trapframe->a5;
  }
  panic("argraw");
  ಹಿಂತಿರುಗಿ -1;
}

// Fetch the nth 32-bit system call argument.
ಶೂನ್ಯ
argint(ಪೂರ್ಣಾಂಕ n, ಪೂರ್ಣಾಂಕ *ip)
{
  *ip = argraw(n);
}

// Retrieve an argument as a pointer.
// Doesn't check for legality, since
// copyin/copyout will do that.
ಶೂನ್ಯ
argaddr(ಪೂರ್ಣಾಂಕ n, uint64 *ip)
{
  *ip = argraw(n);
}

// Fetch the nth word-sized system call argument as a null-terminated string.
// Copies into buf, at most max.
// Returns string length if OK (not including nul), -1 if error.
ಪೂರ್ಣಾಂಕ
argstr(ಪೂರ್ಣಾಂಕ n, ಅಕ್ಷರ *buf, ಪೂರ್ಣಾಂಕ max)
{
  uint64 addr;
  argaddr(n, &addr);
  ಹಿಂತಿರುಗಿ fetchstr(addr, buf, max);
}

// Prototypes for the functions that handle system calls.
ಬಾಹ್ಯ uint64 sys_fork(ಶೂನ್ಯ);
ಬಾಹ್ಯ uint64 sys_exit(ಶೂನ್ಯ);
ಬಾಹ್ಯ uint64 sys_wait(ಶೂನ್ಯ);
ಬಾಹ್ಯ uint64 sys_pipe(ಶೂನ್ಯ);
ಬಾಹ್ಯ uint64 sys_read(ಶೂನ್ಯ);
ಬಾಹ್ಯ uint64 sys_kill(ಶೂನ್ಯ);
ಬಾಹ್ಯ uint64 sys_exec(ಶೂನ್ಯ);
ಬಾಹ್ಯ uint64 sys_fstat(ಶೂನ್ಯ);
ಬಾಹ್ಯ uint64 sys_chdir(ಶೂನ್ಯ);
ಬಾಹ್ಯ uint64 sys_dup(ಶೂನ್ಯ);
ಬಾಹ್ಯ uint64 sys_getpid(ಶೂನ್ಯ);
ಬಾಹ್ಯ uint64 sys_sbrk(ಶೂನ್ಯ);
ಬಾಹ್ಯ uint64 sys_pause(ಶೂನ್ಯ);
ಬಾಹ್ಯ uint64 sys_uptime(ಶೂನ್ಯ);
ಬಾಹ್ಯ uint64 sys_open(ಶೂನ್ಯ);
ಬಾಹ್ಯ uint64 sys_write(ಶೂನ್ಯ);
ಬಾಹ್ಯ uint64 sys_mknod(ಶೂನ್ಯ);
ಬಾಹ್ಯ uint64 sys_unlink(ಶೂನ್ಯ);
ಬಾಹ್ಯ uint64 sys_link(ಶೂನ್ಯ);
ಬಾಹ್ಯ uint64 sys_mkdir(ಶೂನ್ಯ);
ಬಾಹ್ಯ uint64 sys_close(ಶೂನ್ಯ);
ಬಾಹ್ಯ uint64 sys_sync(ಶೂನ್ಯ);

// An array mapping syscall numbers from syscall.h
// to the function that handles the system call.
ಸ್ಥಿರ uint64 (*syscalls[])(ಶೂನ್ಯ) = {
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

ಶೂನ್ಯ
syscall(ಶೂನ್ಯ)
{
  ಪೂರ್ಣಾಂಕ ಅಂಕೆ;
  ರಚನೆ proc *p = myproc();

  ಅಂಕೆ = p->trapframe->a7;
  ಆದರೆ (ಅಂಕೆ > 0 && ಅಂಕೆ < NELEM(syscalls) && syscalls[ಅಂಕೆ]) {
    // Use num to lookup the system call function for num, call it,
    // and store its return value in p->trapframe->a0
    p->trapframe->a0 = syscalls[ಅಂಕೆ]();
  } ಇಲ್ಲದಿದ್ದರೆ {
    printk("%d %s: unknown sys call %d\n", p->pid, p->name, ಅಂಕೆ);
    p->trapframe->a0 = -1;
  }
}
