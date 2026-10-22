#அந்தர்பாவ "types.h"
#அந்தர்பாவ "param.h"
#அந்தர்பாவ "memlayout.h"
#அந்தர்பாவ "riscv.h"
#அந்தர்பாவ "spinlock.h"
#அந்தர்பாவ "proc.h"
#அந்தர்பாவ "syscall.h"
#அந்தர்பாவ "defs.h"

// Fetch the uint64 at addr from the current process.
முழு
fetchaddr(uint64 addr, uint64 *ip)
{
  கட்டமைப்பு proc *p = myproc();
  எனில் (addr >= p->sz ||
      addr + அளவு(uint64) > p->sz) // both tests needed, in case of overflow
    திரும்பு -1;
  எனில் (copyin(p->pagetable, p->sz, (எழுத்து *)ip, addr, அளவு(*ip)) != 0)
    திரும்பு -1;
  திரும்பு 0;
}

// Fetch the nul-terminated string at addr from the current process.
// Returns length of string, not including nul, or -1 for error.
முழு
fetchstr(uint64 addr, எழுத்து *buf, முழு max)
{
  கட்டமைப்பு proc *p = myproc();
  எனில் (copyinstr(p->pagetable, p->sz, buf, addr, max) < 0)
    திரும்பு -1;
  திரும்பு strlen(buf);
}

நிலையான uint64
argraw(முழு n)
{
  கட்டமைப்பு proc *p = myproc();
  தேர்வு (n) {
  நிலை 0:
    திரும்பு p->trapframe->a0;
  நிலை 1:
    திரும்பு p->trapframe->a1;
  நிலை 2:
    திரும்பு p->trapframe->a2;
  நிலை 3:
    திரும்பு p->trapframe->a3;
  நிலை 4:
    திரும்பு p->trapframe->a4;
  நிலை 5:
    திரும்பு p->trapframe->a5;
  }
  panic("argraw");
  திரும்பு -1;
}

// Fetch the nth 32-bit system call argument.
வெறுமை
argint(முழு n, முழு *ip)
{
  *ip = argraw(n);
}

// Retrieve an argument as a pointer.
// Doesn't check for legality, since
// copyin/copyout will do that.
வெறுமை
argaddr(முழு n, uint64 *ip)
{
  *ip = argraw(n);
}

// Fetch the nth word-sized system call argument as a null-terminated string.
// Copies into buf, at most max.
// Returns string length if OK (not including nul), -1 if error.
முழு
argstr(முழு n, எழுத்து *buf, முழு max)
{
  uint64 addr;
  argaddr(n, &addr);
  திரும்பு fetchstr(addr, buf, max);
}

// Prototypes for the functions that handle system calls.
வெளி uint64 sys_fork(வெறுமை);
வெளி uint64 sys_exit(வெறுமை);
வெளி uint64 sys_wait(வெறுமை);
வெளி uint64 sys_pipe(வெறுமை);
வெளி uint64 sys_read(வெறுமை);
வெளி uint64 sys_kill(வெறுமை);
வெளி uint64 sys_exec(வெறுமை);
வெளி uint64 sys_fstat(வெறுமை);
வெளி uint64 sys_chdir(வெறுமை);
வெளி uint64 sys_dup(வெறுமை);
வெளி uint64 sys_getpid(வெறுமை);
வெளி uint64 sys_sbrk(வெறுமை);
வெளி uint64 sys_pause(வெறுமை);
வெளி uint64 sys_uptime(வெறுமை);
வெளி uint64 sys_open(வெறுமை);
வெளி uint64 sys_write(வெறுமை);
வெளி uint64 sys_mknod(வெறுமை);
வெளி uint64 sys_unlink(வெறுமை);
வெளி uint64 sys_link(வெறுமை);
வெளி uint64 sys_mkdir(வெறுமை);
வெளி uint64 sys_close(வெறுமை);
வெளி uint64 sys_sync(வெறுமை);

// An array mapping syscall numbers from syscall.h
// to the function that handles the system call.
நிலையான uint64 (*syscalls[])(வெறுமை) = {
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

வெறுமை
syscall(வெறுமை)
{
  முழு இலக்கம்;
  கட்டமைப்பு proc *p = myproc();

  இலக்கம் = p->trapframe->a7;
  எனில் (இலக்கம் > 0 && இலக்கம் < NELEM(syscalls) && syscalls[இலக்கம்]) {
    // Use num to lookup the system call function for num, call it,
    // and store its return value in p->trapframe->a0
    p->trapframe->a0 = syscalls[இலக்கம்]();
  } இல்லையெனில் {
    printk("%d %s: unknown sys call %d\n", p->pid, p->name, இலக்கம்);
    p->trapframe->a0 = -1;
  }
}
