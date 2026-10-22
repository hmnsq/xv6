#अन्तर्भाव "types.h"
#अन्तर्भाव "param.h"
#अन्तर्भाव "memlayout.h"
#अन्तर्भाव "riscv.h"
#अन्तर्भाव "spinlock.h"
#अन्तर्भाव "proc.h"
#अन्तर्भाव "syscall.h"
#अन्तर्भाव "defs.h"

// Fetch the uint64 at addr from the current process.
पूर्णाङ्क
fetchaddr(uint64 addr, uint64 *ip)
{
  संरचना proc *p = myproc();
  जइ (addr >= p->sz ||
      addr + परिमाण(uint64) > p->sz) // both tests needed, in case of overflow
    प्रत्यागम -1;
  जइ (copyin(p->pagetable, p->sz, (वर्ण *)ip, addr, परिमाण(*ip)) != 0)
    प्रत्यागम -1;
  प्रत्यागम 0;
}

// Fetch the nul-terminated string at addr from the current process.
// Returns length of string, not including nul, or -1 for error.
पूर्णाङ्क
fetchstr(uint64 addr, वर्ण *buf, पूर्णाङ्क max)
{
  संरचना proc *p = myproc();
  जइ (copyinstr(p->pagetable, p->sz, buf, addr, max) < 0)
    प्रत्यागम -1;
  प्रत्यागम strlen(buf);
}

स्थिर uint64
argraw(पूर्णाङ्क n)
{
  संरचना proc *p = myproc();
  विकल्प (n) {
  पक्ष 0:
    प्रत्यागम p->trapframe->a0;
  पक्ष 1:
    प्रत्यागम p->trapframe->a1;
  पक्ष 2:
    प्रत्यागम p->trapframe->a2;
  पक्ष 3:
    प्रत्यागम p->trapframe->a3;
  पक्ष 4:
    प्रत्यागम p->trapframe->a4;
  पक्ष 5:
    प्रत्यागम p->trapframe->a5;
  }
  panic("argraw");
  प्रत्यागम -1;
}

// Fetch the nth 32-bit system call argument.
शून्य
argint(पूर्णाङ्क n, पूर्णाङ्क *ip)
{
  *ip = argraw(n);
}

// Retrieve an argument as a pointer.
// Doesn't check for legality, since
// copyin/copyout will do that.
शून्य
argaddr(पूर्णाङ्क n, uint64 *ip)
{
  *ip = argraw(n);
}

// Fetch the nth word-sized system call argument as a null-terminated string.
// Copies into buf, at most max.
// Returns string length if OK (not including nul), -1 if error.
पूर्णाङ्क
argstr(पूर्णाङ्क n, वर्ण *buf, पूर्णाङ्क max)
{
  uint64 addr;
  argaddr(n, &addr);
  प्रत्यागम fetchstr(addr, buf, max);
}

// Prototypes for the functions that handle system calls.
बाह्य uint64 sys_fork(शून्य);
बाह्य uint64 sys_exit(शून्य);
बाह्य uint64 sys_wait(शून्य);
बाह्य uint64 sys_pipe(शून्य);
बाह्य uint64 sys_read(शून्य);
बाह्य uint64 sys_kill(शून्य);
बाह्य uint64 sys_exec(शून्य);
बाह्य uint64 sys_fstat(शून्य);
बाह्य uint64 sys_chdir(शून्य);
बाह्य uint64 sys_dup(शून्य);
बाह्य uint64 sys_getpid(शून्य);
बाह्य uint64 sys_sbrk(शून्य);
बाह्य uint64 sys_pause(शून्य);
बाह्य uint64 sys_uptime(शून्य);
बाह्य uint64 sys_open(शून्य);
बाह्य uint64 sys_write(शून्य);
बाह्य uint64 sys_mknod(शून्य);
बाह्य uint64 sys_unlink(शून्य);
बाह्य uint64 sys_link(शून्य);
बाह्य uint64 sys_mkdir(शून्य);
बाह्य uint64 sys_close(शून्य);
बाह्य uint64 sys_sync(शून्य);

// An array mapping syscall numbers from syscall.h
// to the function that handles the system call.
स्थिर uint64 (*syscalls[])(शून्य) = {
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

शून्य
syscall(शून्य)
{
  पूर्णाङ्क अङ्कः;
  संरचना proc *p = myproc();

  अङ्कः = p->trapframe->a7;
  जइ (अङ्कः > 0 && अङ्कः < NELEM(syscalls) && syscalls[अङ्कः]) {
    // Use num to lookup the system call function for num, call it,
    // and store its return value in p->trapframe->a0
    p->trapframe->a0 = syscalls[अङ्कः]();
  } अण्णहा {
    printk("%d %s: unknown sys call %d\n", p->pid, p->name, अङ्कः);
    p->trapframe->a0 = -1;
  }
}
