#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "spinlock.h"
#include "proc.h"
#include "defs.h"

sa_nracanaa cpu cpus[NCPU];

sa_nracanaa proc proc[NPROC];

sa_nracanaa proc *initproc;

ចំនួនគត់ nextpid = 1;
sa_nracanaa spinlock pid_lock;

extern void forkret(void);
static void freeproc(sa_nracanaa proc *p);

extern var_na trampoline[]; // trampoline.S

// helps ensure that wakeups of wait()ing
// parents are not lost. helps obey the
// memory model when using p->parent.
// must be acquired before any p->lock.
sa_nracanaa spinlock wait_lock;

// Allocate a page for each process's kernel stack.
// Map it high in memory, followed by an invalid
// guard page.
void
proc_mapstacks(pagetable_t kpgtbl)
{
  sa_nracanaa proc *p;

  សម្រាប់ (p = proc; p < &proc[NPROC]; p++) {
    var_na *pa = kalloc();
    ប្រសិនបើ (pa == 0)
      panic("kalloc");
    uint64 va = KSTACK((ចំនួនគត់)(p - proc));
    kvmmap(kpgtbl, va, (uint64)pa, PGSIZE, PTE_R | PTE_W);
  }
}

// initialize the proc table.
void
procinit(void)
{
  sa_nracanaa proc *p;

  initlock(&pid_lock, "nextpid");
  initlock(&wait_lock, "wait_lock");
  សម្រាប់ (p = proc; p < &proc[NPROC]; p++) {
    initlock(&p->lock, "proc");
    p->state = UNUSED;
    p->kstack = KSTACK((ចំនួនគត់)(p - proc));
  }
}

// Must be called with interrupts disabled,
// to prevent race with process being moved
// to a different CPU.
ចំនួនគត់
cpuid()
{
  ចំនួនគត់ id = r_tp();
  ត្រឡប់ id;
}

// Return this CPU's cpu struct.
// Interrupts must be disabled.
sa_nracanaa cpu *
mycpu(void)
{
  ចំនួនគត់ id = cpuid();
  sa_nracanaa cpu *c = &cpus[id];
  ត្រឡប់ c;
}

// Return the current struct proc *, or zero if none.
sa_nracanaa proc *
myproc(void)
{
  push_off();
  sa_nracanaa cpu *c = mycpu();
  sa_nracanaa proc *p = c->proc;
  pop_off();
  ត្រឡប់ p;
}

ចំនួនគត់
allocpid()
{
  ចំនួនគត់ pid;

  acquire(&pid_lock);
  pid = nextpid;
  nextpid = nextpid + 1;
  release(&pid_lock);

  ត្រឡប់ pid;
}

// Look in the process table for an UNUSED proc.
// If found, initialize state required to run in the kernel,
// and return with p->lock held.
// If there are no free procs, or a memory allocation fails, return 0.
static sa_nracanaa proc *
allocproc(void)
{
  sa_nracanaa proc *p;

  សម្រាប់ (p = proc; p < &proc[NPROC]; p++) {
    acquire(&p->lock);
    ប្រសិនបើ (p->state == UNUSED) {
      goto found;
    } បើមិនដូច្នេះ {
      release(&p->lock);
    }
  }
  ត្រឡប់ 0;

found:
  p->pid = allocpid();
  p->state = USED;

  // Allocate a trapframe page.
  ប្រសិនបើ ((p->trapframe = (sa_nracanaa trapframe *)kalloc()) == 0) {
    freeproc(p);
    release(&p->lock);
    ត្រឡប់ 0;
  }

  // An empty user page table.
  p->pagetable = proc_pagetable(p);
  ប្រសិនបើ (p->pagetable == 0) {
    freeproc(p);
    release(&p->lock);
    ត្រឡប់ 0;
  }

  // Set up new context to start executing at forkret,
  // which returns to user space.
  memset(&p->context, 0, parimaa_na(p->context));
  p->context.ra = (uint64)forkret;
  p->context.sp = p->kstack + PGSIZE;

  ត្រឡប់ p;
}

// free a proc structure and the data hanging from it,
// including user pages.
// p->lock must be held.
static void
freeproc(sa_nracanaa proc *p)
{
  ប្រសិនបើ (p->trapframe)
    kfree((void *)p->trapframe);
  p->trapframe = 0;
  ប្រសិនបើ (p->pagetable)
    proc_freepagetable(p->pagetable, p->sz);
  p->pagetable = 0;
  p->sz = 0;
  p->pid = 0;
  p->name[0] = 0;
  p->chan = 0;
  p->killed = 0;
  p->xstate = 0;
  p->state = UNUSED;
}

// Create a user page table for a given process, with no user memory,
// but with trampoline and trapframe pages.
pagetable_t
proc_pagetable(sa_nracanaa proc *p)
{
  pagetable_t pagetable;

  // An empty page table.
  pagetable = uvmcreate();
  ប្រសិនបើ (pagetable == 0)
    ត្រឡប់ 0;

  // map the trampoline code (for system call return)
  // at the highest user virtual address.
  // only the supervisor uses it, on the way
  // to/from user space, so not PTE_U.
  ប្រសិនបើ (mappages(pagetable, TRAMPOLINE, PGSIZE, (uint64)trampoline,
               PTE_R | PTE_X) < 0) {
    uvmfree(pagetable, 0);
    ត្រឡប់ 0;
  }

  // map the trapframe page just below the trampoline page, for
  // trampoline.S.
  ប្រសិនបើ (mappages(pagetable, TRAPFRAME, PGSIZE, (uint64)(p->trapframe),
               PTE_R | PTE_W) < 0) {
    uvmunmap(pagetable, TRAMPOLINE, 1, 0);
    uvmfree(pagetable, 0);
    ត្រឡប់ 0;
  }

  ត្រឡប់ pagetable;
}

// Free a process's page table, and free the
// physical memory it refers to.
void
proc_freepagetable(pagetable_t pagetable, uint64 sz)
{
  uvmunmap(pagetable, TRAMPOLINE, 1, 0);
  uvmunmap(pagetable, TRAPFRAME, 1, 0);
  uvmfree(pagetable, sz);
}

// Set up first user process.
void
userinit(void)
{
  sa_nracanaa proc *p;

  p = allocproc();
  initproc = p;

  p->cwd = namei("/");

  p->state = RUNNABLE;

  release(&p->lock);
}

// Grow or shrink user memory by n bytes.
// Return 0 on success, -1 on failure.
ចំនួនគត់
growproc(ចំនួនគត់ n)
{
  uint64 sz;
  sa_nracanaa proc *p = myproc();

  sz = p->sz;
  ប្រសិនបើ (n > 0) {
    ប្រសិនបើ (sz + n > TRAPFRAME) {
      ត្រឡប់ -1;
    }
    ប្រសិនបើ ((sz = uvmalloc(p->pagetable, sz, sz + n, PTE_W)) == 0) {
      ត្រឡប់ -1;
    }
  } បើមិនដូច្នេះ ប្រសិនបើ (n < 0) {
    sz = uvmdealloc(p->pagetable, sz, sz + n);
  }
  p->sz = sz;
  ត្រឡប់ 0;
}

// Create a new process, copying the parent.
// Sets up child kernel stack to return as if from fork() system call.
ចំនួនគត់
kfork(void)
{
  ចំនួនគត់ i, pid;
  sa_nracanaa proc *np;
  sa_nracanaa proc *p = myproc();

  // Allocate process.
  ប្រសិនបើ ((np = allocproc()) == 0) {
    ត្រឡប់ -1;
  }

  // Copy user memory from parent to child.
  ប្រសិនបើ (uvmcopy(p->pagetable, np->pagetable, p->sz) < 0) {
    freeproc(np);
    release(&np->lock);
    ត្រឡប់ -1;
  }
  np->sz = p->sz;

  // copy saved user registers.
  *(np->trapframe) = *(p->trapframe);

  // Cause fork to return 0 in the child.
  np->trapframe->a0 = 0;

  // increment reference counts on open file descriptors.
  សម្រាប់ (i = 0; i < NOFILE; i++)
    ប្រសិនបើ (p->ofile[i])
      np->ofile[i] = filedup(p->ofile[i]);
  np->cwd = idup(p->cwd);

  safestrcpy(np->name, p->name, parimaa_na(p->name));

  pid = np->pid;

  release(&np->lock);

  acquire(&wait_lock);
  np->parent = p;
  release(&wait_lock);

  acquire(&np->lock);
  np->state = RUNNABLE;
  release(&np->lock);

  ត្រឡប់ pid;
}

// Pass p's abandoned children to init.
// Caller must hold wait_lock.
void
reparent(sa_nracanaa proc *p)
{
  sa_nracanaa proc *pp;

  សម្រាប់ (pp = proc; pp < &proc[NPROC]; pp++) {
    ប្រសិនបើ (pp->parent == p) {
      pp->parent = initproc;
      wakeup(initproc);
    }
  }
}

// Exit the current process.  Does not return.
// An exited process remains in the zombie state
// until its parent calls wait().
void
kexit(ចំនួនគត់ status)
{
  sa_nracanaa proc *p = myproc();

  ប្រសិនបើ (p == initproc)
    panic("init exiting");

  // Close all open files.
  សម្រាប់ (ចំនួនគត់ fd = 0; fd < NOFILE; fd++) {
    ប្រសិនបើ (p->ofile[fd]) {
      sa_nracanaa file *f = p->ofile[fd];
      fileclose(f);
      p->ofile[fd] = 0;
    }
  }

  begin_op();
  iput(p->cwd);
  end_op();
  p->cwd = 0;

  acquire(&wait_lock);

  // Give any children to init.
  reparent(p);

  // Parent might be sleeping in wait().
  wakeup(p->parent);

  acquire(&p->lock);

  p->xstate = status;
  p->state = ZOMBIE;

  release(&wait_lock);

  // Jump into the scheduler, never to return.
  sched();
  panic("zombie exit");
}

// Wait for a child process to exit and return its pid.
// Return -1 if this process has no children.
ចំនួនគត់
kwait(uint64 addr)
{
  sa_nracanaa proc *pp;
  ចំនួនគត់ havekids, pid;
  sa_nracanaa proc *p = myproc();

  acquire(&wait_lock);

  សម្រាប់ (;;) {
    // Scan through table looking for exited children.
    havekids = 0;
    សម្រាប់ (pp = proc; pp < &proc[NPROC]; pp++) {
      ប្រសិនបើ (pp->parent == p) {
        // make sure the child isn't still in exit() or swtch().
        acquire(&pp->lock);

        havekids = 1;
        ប្រសិនបើ (pp->state == ZOMBIE) {
          // Found one.
          pid = pp->pid;
          ប្រសិនបើ (addr != 0 &&
              copyout(p->pagetable, p->sz, addr, (var_na *)&pp->xstate,
                      parimaa_na(pp->xstate)) < 0) {
            release(&pp->lock);
            release(&wait_lock);
            ត្រឡប់ -1;
          }
          pp->parent = 0;
          freeproc(pp);
          release(&pp->lock);
          release(&wait_lock);
          ត្រឡប់ pid;
        }
        release(&pp->lock);
      }
    }

    // No point waiting if we don't have any children.
    ប្រសិនបើ (!havekids || killed(p)) {
      release(&wait_lock);
      ត្រឡប់ -1;
    }

    // Wait for a child to exit.
    sleep_prepare(p); //DOC: wait-sleep
    release(&wait_lock);
    sleep();
    acquire(&wait_lock);
  }
}

// Per-CPU process scheduler.
// Each CPU calls scheduler() after setting itself up.
// Scheduler never returns.  It loops, doing:
//  - choose a process to run.
//  - swtch to start running that process.
//  - eventually that process transfers control
//    via swtch back to the scheduler.
void
scheduler(void)
{
  sa_nracanaa proc *p;
  sa_nracanaa cpu *c = mycpu();

  c->proc = 0;
  សម្រាប់ (;;) {
    // The most recent process to run may have had interrupts
    // turned off; enable them to avoid a deadlock if all
    // processes are waiting. Then turn them back off
    // to avoid a possible race between an interrupt
    // and wfi.
    intr_on();
    intr_off();

    ចំនួនគត់ found = 0;
    សម្រាប់ (p = proc; p < &proc[NPROC]; p++) {
      acquire(&p->lock);
      ប្រសិនបើ (p->state == RUNNABLE) {
        // Switch to chosen process.  It is the process's job
        // to release its lock and then reacquire it
        // before jumping back to us.
        p->state = RUNNING;
        c->proc = p;
        swtch(&c->context, &p->context);

        // Don't re-enable interrupts on release.
        mycpu()->intena = 0;

        // Process is done running for now.
        // It should have changed its p->state before coming back.
        c->proc = 0;
        found = 1;
      }
      release(&p->lock);
    }
    ប្រសិនបើ (found == 0) {
      // nothing to run; stop running on this core until an interrupt.
      asm ca_ncala("wfi");
    }
  }
}

// Switch to scheduler.  Must hold only p->lock
// and have changed proc->state. Saves and restores
// intena because intena is a property of this
// kernel thread, not this CPU. It should
// be proc->intena and proc->noff, but that would
// break in the few places where a lock is held but
// there's no process.
void
sched(void)
{
  ចំនួនគត់ intena;
  sa_nracanaa proc *p = myproc();

  ប្រសិនបើ (!holding(&p->lock))
    panic("sched p->lock");
  ប្រសិនបើ (mycpu()->noff != 1)
    panic("sched locks");
  ប្រសិនបើ (p->state == RUNNING)
    panic("sched RUNNING");
  ប្រសិនបើ (intr_get())
    panic("sched interruptible");

  intena = mycpu()->intena;
  swtch(&p->context, &mycpu()->context);
  mycpu()->intena = intena;
}

// Give up the CPU for one scheduling round.
void
yield(void)
{
  sa_nracanaa proc *p = myproc();
  acquire(&p->lock);
  p->state = RUNNABLE;
  sched();
  release(&p->lock);
}

// A fork child's very first scheduling by scheduler()
// will swtch to forkret.
void
forkret(void)
{
  extern var_na userret[];
  static ចំនួនគត់ first = 1;
  sa_nracanaa proc *p = myproc();

  // Still holding p->lock from scheduler.
  release(&p->lock);

  ប្រសិនបើ (first) {
    first = 0;

    // File system initialization must be run in the context of a
    // regular process (e.g., because it calls sleep), and thus cannot
    // be run from main().
    fsinit(ROOTDEV);

    // We can invoke kexec() now that file system is initialized.
    // Put the return value (argc) of kexec into a0.
    p->trapframe->a0 = kexec("/init", (var_na *[]){"/init", 0});
    ប្រសិនបើ (p->trapframe->a0 == -1) {
      panic("exec");
    }
  }

  // return to user space, mimicing usertrap()'s return.
  prepare_return();
  uint64 satp = MAKE_SATP(p->pagetable);
  uint64 trampoline_userret = TRAMPOLINE + (userret - trampoline);
  ((void (*)(uint64))trampoline_userret)(satp);
}

// Register current process as waiting for wakeups on chan.
void
sleep_prepare(void *chan)
{
  sa_nracanaa proc *p = myproc();

  acquire(&p->lock);
  ប្រសិនបើ (chan == 0)
    panic("sleep_prepare: zero chan");
  p->chan = chan;
  release(&p->lock);
}

// Put the thread to sleep.  Assumes sleep_prepare() was called before.
// If the channel registered by sleep_prepare() has been woken up in
// the meantime, do not go to sleep, and instead return immediately.
void
sleep(void)
{
  sa_nracanaa proc *p = myproc();

  acquire(&p->lock);
  ប្រសិនបើ (p->chan != 0) {
    p->state = SLEEPING;
    sched();
  }
  release(&p->lock);
}

// Wake up all processes sleeping on channel chan.
void
wakeup(void *chan)
{
  sa_nracanaa proc *p;

  សម្រាប់ (p = proc; p < &proc[NPROC]; p++) {
    acquire(&p->lock);
    ប្រសិនបើ (p->chan == chan) {
      // If the process is waiting for wakeups on this channel,
      // signal that the wakeup happened by clearing p->chan.
      p->chan = 0;

      // If this waiting process has gotten so far as to actually
      // go to sleep, also set it back to RUNNING.
      ប្រសិនបើ (p->state == SLEEPING) {
        p->state = RUNNABLE;
      }
    }
    release(&p->lock);
  }
}

// Kill the process with the given pid.
// The victim won't exit until it tries to return
// to user space (see usertrap() in trap.c).
ចំនួនគត់
kkill(ចំនួនគត់ pid)
{
  sa_nracanaa proc *p;

  ប្រសិនបើ (pid == 0)
    ត្រឡប់ -1;

  សម្រាប់ (p = proc; p < &proc[NPROC]; p++) {
    acquire(&p->lock);
    ប្រសិនបើ (p->pid == pid) {
      p->killed = 1;
      ប្រសិនបើ (p->state == SLEEPING) {
        // Wake process from sleep().
        p->state = RUNNABLE;
      }
      release(&p->lock);
      ត្រឡប់ 0;
    }
    release(&p->lock);
  }
  ត្រឡប់ -1;
}

void
setkilled(sa_nracanaa proc *p)
{
  acquire(&p->lock);
  p->killed = 1;
  release(&p->lock);
}

ចំនួនគត់
killed(sa_nracanaa proc *p)
{
  ចំនួនគត់ k;

  acquire(&p->lock);
  k = p->killed;
  release(&p->lock);
  ត្រឡប់ k;
}

// Copy to either a user address, or kernel address,
// depending on usr_dst.
// Returns 0 on success, -1 on error.
ចំនួនគត់
either_copyout(ចំនួនគត់ user_dst, uint64 dst, void *src, uint64 len)
{
  sa_nracanaa proc *p = myproc();
  ប្រសិនបើ (user_dst) {
    ត្រឡប់ copyout(p->pagetable, p->sz, dst, src, len);
  } បើមិនដូច្នេះ {
    memmove((var_na *)dst, src, len);
    ត្រឡប់ 0;
  }
}

// Copy from either a user address, or kernel address,
// depending on usr_src.
// Returns 0 on success, -1 on error.
ចំនួនគត់
either_copyin(void *dst, ចំនួនគត់ user_src, uint64 src, uint64 len)
{
  sa_nracanaa proc *p = myproc();
  ប្រសិនបើ (user_src) {
    ត្រឡប់ copyin(p->pagetable, p->sz, dst, src, len);
  } បើមិនដូច្នេះ {
    memmove(dst, (var_na *)src, len);
    ត្រឡប់ 0;
  }
}

// Print a process listing to console.  For debugging.
// Runs when user types ^P on console.
// No lock to avoid wedging a stuck machine further.
void
procdump(void)
{
  static var_na *states[] = {
    // clang-format off
    [UNUSED]    = "unused",
    [USED]      = "used",
    [SLEEPING]  = "sleep ",
    [RUNNABLE]  = "runble",
    [RUNNING]   = "run   ",
    [ZOMBIE]    = "zombie"
    // clang-format on
  };
  sa_nracanaa proc *p;
  var_na *state;

  printk("\n");
  សម្រាប់ (p = proc; p < &proc[NPROC]; p++) {
    ប្រសិនបើ (p->state == UNUSED)
      continue;
    ប្រសិនបើ (p->state >= 0 && p->state < NELEM(states) && states[p->state])
      state = states[p->state];
    បើមិនដូច្នេះ
      state = "???";
    printk("%d %s %s", p->pid, state, p->name);
    printk("\n");
  }
}
