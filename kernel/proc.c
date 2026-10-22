#ਅਨ੍ਤਰ੍ਭਾਵ "types.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "param.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "memlayout.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "riscv.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "spinlock.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "proc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "defs.h"

ਬਣਤਰ cpu cpus[NCPU];

ਬਣਤਰ proc proc[NPROC];

ਬਣਤਰ proc *initproc;

ਪੂਰਨ nextpid = 1;
ਬਣਤਰ spinlock pid_ਤਾਲਾ;

ਬਾਹਰੀ ਖਾਲੀ forkret(ਖਾਲੀ);
ਸਥਿਰ ਖਾਲੀ freeproc(ਬਣਤਰ proc *p);

ਬਾਹਰੀ ਅੱਖਰ trampoline[]; // trampoline.S

// helps ensure that wakeups of wait()ing
// parents are not lost. helps obey the
// memory model when using p->parent.
// must be acquired before any p->lock.
ਬਣਤਰ spinlock wait_ਤਾਲਾ;

// Allocate a page for each process's kernel stack.
// Map it high in memory, followed by an invalid
// guard page.
ਖਾਲੀ
proc_mapstacks(pagetable_t kpgtbl)
{
  ਬਣਤਰ proc *p;

  ਲਈ (p = proc; p < &proc[NPROC]; p++) {
    ਅੱਖਰ *pa = kalloc();
    ਜੇ (pa == 0)
      panic("kalloc");
    uint64 va = KSTACK((ਪੂਰਨ)(p - proc));
    kvmmap(kpgtbl, va, (uint64)pa, PGSIZE, PTE_R | PTE_W);
  }
}

// initialize the proc table.
ਖਾਲੀ
procinit(ਖਾਲੀ)
{
  ਬਣਤਰ proc *p;

  initlock(&pid_ਤਾਲਾ, "nextpid");
  initlock(&wait_ਤਾਲਾ, "wait_lock");
  ਲਈ (p = proc; p < &proc[NPROC]; p++) {
    initlock(&p->ਤਾਲਾ, "proc");
    p->state = ਅਣਵਰਤਿਆ;
    p->kstack = KSTACK((ਪੂਰਨ)(p - proc));
  }
}

// Must be called with interrupts disabled,
// to prevent race with process being moved
// to a different CPU.
ਪੂਰਨ
cpuid()
{
  ਪੂਰਨ id = r_tp();
  ਵਾਪਸ id;
}

// Return this CPU's cpu struct.
// Interrupts must be disabled.
ਬਣਤਰ cpu *
mycpu(ਖਾਲੀ)
{
  ਪੂਰਨ id = cpuid();
  ਬਣਤਰ cpu *c = &cpus[id];
  ਵਾਪਸ c;
}

// Return the current struct proc *, or zero if none.
ਬਣਤਰ proc *
myproc(ਖਾਲੀ)
{
  ਧੱਕੋ_off();
  ਬਣਤਰ cpu *c = mycpu();
  ਬਣਤਰ proc *p = c->proc;
  ਕੱਢੋ_off();
  ਵਾਪਸ p;
}

ਪੂਰਨ
allocpid()
{
  ਪੂਰਨ pid;

  acquire(&pid_ਤਾਲਾ);
  pid = nextpid;
  nextpid = nextpid + 1;
  release(&pid_ਤਾਲਾ);

  ਵਾਪਸ pid;
}

// Look in the process table for an UNUSED proc.
// If found, initialize state required to run in the kernel,
// and return with p->lock held.
// If there are no free procs, or a memory allocation fails, return 0.
ਸਥਿਰ ਬਣਤਰ proc *
allocproc(ਖਾਲੀ)
{
  ਬਣਤਰ proc *p;

  ਲਈ (p = proc; p < &proc[NPROC]; p++) {
    acquire(&p->ਤਾਲਾ);
    ਜੇ (p->state == ਅਣਵਰਤਿਆ) {
      ਜਾਓ ਮਿਲਿਆ;
    } ਨਹੀਂਤਾਂ {
      release(&p->ਤਾਲਾ);
    }
  }
  ਵਾਪਸ 0;

ਮਿਲਿਆ:
  p->pid = allocpid();
  p->state = ਵਰਤਿਆ;

  // Allocate a trapframe page.
  ਜੇ ((p->trapframe = (ਬਣਤਰ trapframe *)kalloc()) == 0) {
    freeproc(p);
    release(&p->ਤਾਲਾ);
    ਵਾਪਸ 0;
  }

  // An empty user page table.
  p->pagetable = proc_pagetable(p);
  ਜੇ (p->pagetable == 0) {
    freeproc(p);
    release(&p->ਤਾਲਾ);
    ਵਾਪਸ 0;
  }

  // Set up new context to start executing at forkret,
  // which returns to user space.
  memset(&p->context, 0, ਆਕਾਰ(p->context));
  p->context.ra = (uint64)forkret;
  p->context.sp = p->kstack + PGSIZE;

  ਵਾਪਸ p;
}

// free a proc structure and the data hanging from it,
// including user pages.
// p->lock must be held.
ਸਥਿਰ ਖਾਲੀ
freeproc(ਬਣਤਰ proc *p)
{
  ਜੇ (p->trapframe)
    kfree((ਖਾਲੀ *)p->trapframe);
  p->trapframe = 0;
  ਜੇ (p->pagetable)
    proc_freepagetable(p->pagetable, p->sz);
  p->pagetable = 0;
  p->sz = 0;
  p->pid = 0;
  p->name[0] = 0;
  p->chan = 0;
  p->killed = 0;
  p->xstate = 0;
  p->state = ਅਣਵਰਤਿਆ;
}

// Create a user page table for a given process, with no user memory,
// but with trampoline and trapframe pages.
pagetable_t
proc_pagetable(ਬਣਤਰ proc *p)
{
  pagetable_t pagetable;

  // An empty page table.
  pagetable = uvmcreate();
  ਜੇ (pagetable == 0)
    ਵਾਪਸ 0;

  // map the trampoline code (for system call return)
  // at the highest user virtual address.
  // only the supervisor uses it, on the way
  // to/from user space, so not PTE_U.
  ਜੇ (mappages(pagetable, TRAMPOLINE, PGSIZE, (uint64)trampoline,
               PTE_R | PTE_X) < 0) {
    uvmfree(pagetable, 0);
    ਵਾਪਸ 0;
  }

  // map the trapframe page just below the trampoline page, for
  // trampoline.S.
  ਜੇ (mappages(pagetable, TRAPFRAME, PGSIZE, (uint64)(p->trapframe),
               PTE_R | PTE_W) < 0) {
    uvmunmap(pagetable, TRAMPOLINE, 1, 0);
    uvmfree(pagetable, 0);
    ਵਾਪਸ 0;
  }

  ਵਾਪਸ pagetable;
}

// Free a process's page table, and free the
// physical memory it refers to.
ਖਾਲੀ
proc_freepagetable(pagetable_t pagetable, uint64 sz)
{
  uvmunmap(pagetable, TRAMPOLINE, 1, 0);
  uvmunmap(pagetable, TRAPFRAME, 1, 0);
  uvmfree(pagetable, sz);
}

// Set up first user process.
ਖਾਲੀ
userinit(ਖਾਲੀ)
{
  ਬਣਤਰ proc *p;

  p = allocproc();
  initproc = p;

  p->cwd = namei("/");

  p->state = RUNNABLE;

  release(&p->ਤਾਲਾ);
}

// Grow or shrink user memory by n bytes.
// Return 0 on success, -1 on failure.
ਪੂਰਨ
growproc(ਪੂਰਨ n)
{
  uint64 sz;
  ਬਣਤਰ proc *p = myproc();

  sz = p->sz;
  ਜੇ (n > 0) {
    ਜੇ (sz + n > TRAPFRAME) {
      ਵਾਪਸ -1;
    }
    ਜੇ ((sz = uvmalloc(p->pagetable, sz, sz + n, PTE_W)) == 0) {
      ਵਾਪਸ -1;
    }
  } ਨਹੀਂਤਾਂ ਜੇ (n < 0) {
    sz = uvmdealloc(p->pagetable, sz, sz + n);
  }
  p->sz = sz;
  ਵਾਪਸ 0;
}

// Create a new process, copying the parent.
// Sets up child kernel stack to return as if from fork() system call.
ਪੂਰਨ
kfork(ਖਾਲੀ)
{
  ਪੂਰਨ i, pid;
  ਬਣਤਰ proc *np;
  ਬਣਤਰ proc *p = myproc();

  // Allocate process.
  ਜੇ ((np = allocproc()) == 0) {
    ਵਾਪਸ -1;
  }

  // Copy user memory from parent to child.
  ਜੇ (uvmcopy(p->pagetable, np->pagetable, p->sz) < 0) {
    freeproc(np);
    release(&np->ਤਾਲਾ);
    ਵਾਪਸ -1;
  }
  np->sz = p->sz;

  // copy saved user registers.
  *(np->trapframe) = *(p->trapframe);

  // Cause fork to return 0 in the child.
  np->trapframe->a0 = 0;

  // increment reference counts on open file descriptors.
  ਲਈ (i = 0; i < NOFILE; i++)
    ਜੇ (p->ofile[i])
      np->ofile[i] = filedup(p->ofile[i]);
  np->cwd = idup(p->cwd);

  safestrcpy(np->name, p->name, ਆਕਾਰ(p->name));

  pid = np->pid;

  release(&np->ਤਾਲਾ);

  acquire(&wait_ਤਾਲਾ);
  np->ਮਾਪਾ = p;
  release(&wait_ਤਾਲਾ);

  acquire(&np->ਤਾਲਾ);
  np->state = RUNNABLE;
  release(&np->ਤਾਲਾ);

  ਵਾਪਸ pid;
}

// Pass p's abandoned children to init.
// Caller must hold wait_lock.
ਖਾਲੀ
reparent(ਬਣਤਰ proc *p)
{
  ਬਣਤਰ proc *pp;

  ਲਈ (pp = proc; pp < &proc[NPROC]; pp++) {
    ਜੇ (pp->ਮਾਪਾ == p) {
      pp->ਮਾਪਾ = initproc;
      wakeup(initproc);
    }
  }
}

// Exit the current process.  Does not return.
// An exited process remains in the zombie state
// until its parent calls wait().
ਖਾਲੀ
kexit(ਪੂਰਨ ਸਥਿਤੀ)
{
  ਬਣਤਰ proc *p = myproc();

  ਜੇ (p == initproc)
    panic("init exiting");

  // Close all open files.
  ਲਈ (ਪੂਰਨ fd = 0; fd < NOFILE; fd++) {
    ਜੇ (p->ofile[fd]) {
      ਬਣਤਰ file *f = p->ofile[fd];
      fileclose(f);
      p->ofile[fd] = 0;
    }
  }

  begin_op();
  iput(p->cwd);
  ਸਿਰਾ_op();
  p->cwd = 0;

  acquire(&wait_ਤਾਲਾ);

  // Give any children to init.
  reparent(p);

  // Parent might be sleeping in wait().
  wakeup(p->ਮਾਪਾ);

  acquire(&p->ਤਾਲਾ);

  p->xstate = ਸਥਿਤੀ;
  p->state = ZOMBIE;

  release(&wait_ਤਾਲਾ);

  // Jump into the scheduler, never to return.
  sched();
  panic("zombie exit");
}

// Wait for a child process to exit and return its pid.
// Return -1 if this process has no children.
ਪੂਰਨ
kwait(uint64 addr)
{
  ਬਣਤਰ proc *pp;
  ਪੂਰਨ havekids, pid;
  ਬਣਤਰ proc *p = myproc();

  acquire(&wait_ਤਾਲਾ);

  ਲਈ (;;) {
    // Scan through table looking for exited children.
    havekids = 0;
    ਲਈ (pp = proc; pp < &proc[NPROC]; pp++) {
      ਜੇ (pp->ਮਾਪਾ == p) {
        // make sure the child isn't still in exit() or swtch().
        acquire(&pp->ਤਾਲਾ);

        havekids = 1;
        ਜੇ (pp->state == ZOMBIE) {
          // Found one.
          pid = pp->pid;
          ਜੇ (addr != 0 &&
              copyout(p->pagetable, p->sz, addr, (ਅੱਖਰ *)&pp->xstate,
                      ਆਕਾਰ(pp->xstate)) < 0) {
            release(&pp->ਤਾਲਾ);
            release(&wait_ਤਾਲਾ);
            ਵਾਪਸ -1;
          }
          pp->ਮਾਪਾ = 0;
          freeproc(pp);
          release(&pp->ਤਾਲਾ);
          release(&wait_ਤਾਲਾ);
          ਵਾਪਸ pid;
        }
        release(&pp->ਤਾਲਾ);
      }
    }

    // No point waiting if we don't have any children.
    ਜੇ (!havekids || killed(p)) {
      release(&wait_ਤਾਲਾ);
      ਵਾਪਸ -1;
    }

    // Wait for a child to exit.
    sleep_prepare(p); //DOC: wait-sleep
    release(&wait_ਤਾਲਾ);
    sleep();
    acquire(&wait_ਤਾਲਾ);
  }
}

// Per-CPU process scheduler.
// Each CPU calls scheduler() after setting itself up.
// Scheduler never returns.  It loops, doing:
//  - choose a process to run.
//  - swtch to start running that process.
//  - eventually that process transfers control
//    via swtch back to the scheduler.
ਖਾਲੀ
scheduler(ਖਾਲੀ)
{
  ਬਣਤਰ proc *p;
  ਬਣਤਰ cpu *c = mycpu();

  c->proc = 0;
  ਲਈ (;;) {
    // The most recent process to run may have had interrupts
    // turned off; enable them to avoid a deadlock if all
    // processes are waiting. Then turn them back off
    // to avoid a possible race between an interrupt
    // and wfi.
    intr_on();
    intr_off();

    ਪੂਰਨ ਮਿਲਿਆ = 0;
    ਲਈ (p = proc; p < &proc[NPROC]; p++) {
      acquire(&p->ਤਾਲਾ);
      ਜੇ (p->state == RUNNABLE) {
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
        ਮਿਲਿਆ = 1;
      }
      release(&p->ਤਾਲਾ);
    }
    ਜੇ (ਮਿਲਿਆ == 0) {
      // nothing to run; stop running on this core until an interrupt.
      asm ਅਸਥਿਰ("wfi");
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
ਖਾਲੀ
sched(ਖਾਲੀ)
{
  ਪੂਰਨ intena;
  ਬਣਤਰ proc *p = myproc();

  ਜੇ (!holding(&p->ਤਾਲਾ))
    panic("sched p->lock");
  ਜੇ (mycpu()->noff != 1)
    panic("sched locks");
  ਜੇ (p->state == RUNNING)
    panic("sched RUNNING");
  ਜੇ (intr_get())
    panic("sched interruptible");

  intena = mycpu()->intena;
  swtch(&p->context, &mycpu()->context);
  mycpu()->intena = intena;
}

// Give up the CPU for one scheduling round.
ਖਾਲੀ
yield(ਖਾਲੀ)
{
  ਬਣਤਰ proc *p = myproc();
  acquire(&p->ਤਾਲਾ);
  p->state = RUNNABLE;
  sched();
  release(&p->ਤਾਲਾ);
}

// A fork child's very first scheduling by scheduler()
// will swtch to forkret.
ਖਾਲੀ
forkret(ਖਾਲੀ)
{
  ਬਾਹਰੀ ਅੱਖਰ userret[];
  ਸਥਿਰ ਪੂਰਨ ਪਹਿਲਾ = 1;
  ਬਣਤਰ proc *p = myproc();

  // Still holding p->lock from scheduler.
  release(&p->ਤਾਲਾ);

  ਜੇ (ਪਹਿਲਾ) {
    ਪਹਿਲਾ = 0;

    // File system initialization must be run in the context of a
    // regular process (e.g., because it calls sleep), and thus cannot
    // be run from main().
    fsinit(ROOTDEV);

    // We can invoke kexec() now that file system is initialized.
    // Put the return value (argc) of kexec into a0.
    p->trapframe->a0 = kexec("/init", (ਅੱਖਰ *[]){"/init", 0});
    ਜੇ (p->trapframe->a0 == -1) {
      panic("exec");
    }
  }

  // return to user space, mimicing usertrap()'s return.
  prepare_return();
  uint64 satp = MAKE_SATP(p->pagetable);
  uint64 trampoline_userret = TRAMPOLINE + (userret - trampoline);
  ((ਖਾਲੀ (*)(uint64))trampoline_userret)(satp);
}

// Register current process as waiting for wakeups on chan.
ਖਾਲੀ
sleep_prepare(ਖਾਲੀ *chan)
{
  ਬਣਤਰ proc *p = myproc();

  acquire(&p->ਤਾਲਾ);
  ਜੇ (chan == 0)
    panic("sleep_prepare: zero chan");
  p->chan = chan;
  release(&p->ਤਾਲਾ);
}

// Put the thread to sleep.  Assumes sleep_prepare() was called before.
// If the channel registered by sleep_prepare() has been woken up in
// the meantime, do not go to sleep, and instead return immediately.
ਖਾਲੀ
sleep(ਖਾਲੀ)
{
  ਬਣਤਰ proc *p = myproc();

  acquire(&p->ਤਾਲਾ);
  ਜੇ (p->chan != 0) {
    p->state = SLEEPING;
    sched();
  }
  release(&p->ਤਾਲਾ);
}

// Wake up all processes sleeping on channel chan.
ਖਾਲੀ
wakeup(ਖਾਲੀ *chan)
{
  ਬਣਤਰ proc *p;

  ਲਈ (p = proc; p < &proc[NPROC]; p++) {
    acquire(&p->ਤਾਲਾ);
    ਜੇ (p->chan == chan) {
      // If the process is waiting for wakeups on this channel,
      // signal that the wakeup happened by clearing p->chan.
      p->chan = 0;

      // If this waiting process has gotten so far as to actually
      // go to sleep, also set it back to RUNNING.
      ਜੇ (p->state == SLEEPING) {
        p->state = RUNNABLE;
      }
    }
    release(&p->ਤਾਲਾ);
  }
}

// Kill the process with the given pid.
// The victim won't exit until it tries to return
// to user space (see usertrap() in trap.c).
ਪੂਰਨ
kkill(ਪੂਰਨ pid)
{
  ਬਣਤਰ proc *p;

  ਜੇ (pid == 0)
    ਵਾਪਸ -1;

  ਲਈ (p = proc; p < &proc[NPROC]; p++) {
    acquire(&p->ਤਾਲਾ);
    ਜੇ (p->pid == pid) {
      p->killed = 1;
      ਜੇ (p->state == SLEEPING) {
        // Wake process from sleep().
        p->state = RUNNABLE;
      }
      release(&p->ਤਾਲਾ);
      ਵਾਪਸ 0;
    }
    release(&p->ਤਾਲਾ);
  }
  ਵਾਪਸ -1;
}

ਖਾਲੀ
setkilled(ਬਣਤਰ proc *p)
{
  acquire(&p->ਤਾਲਾ);
  p->killed = 1;
  release(&p->ਤਾਲਾ);
}

ਪੂਰਨ
killed(ਬਣਤਰ proc *p)
{
  ਪੂਰਨ k;

  acquire(&p->ਤਾਲਾ);
  k = p->killed;
  release(&p->ਤਾਲਾ);
  ਵਾਪਸ k;
}

// Copy to either a user address, or kernel address,
// depending on usr_dst.
// Returns 0 on success, -1 on error.
ਪੂਰਨ
either_copyout(ਪੂਰਨ user_dst, uint64 dst, ਖਾਲੀ *src, uint64 len)
{
  ਬਣਤਰ proc *p = myproc();
  ਜੇ (user_dst) {
    ਵਾਪਸ copyout(p->pagetable, p->sz, dst, src, len);
  } ਨਹੀਂਤਾਂ {
    memmove((ਅੱਖਰ *)dst, src, len);
    ਵਾਪਸ 0;
  }
}

// Copy from either a user address, or kernel address,
// depending on usr_src.
// Returns 0 on success, -1 on error.
ਪੂਰਨ
either_copyin(ਖਾਲੀ *dst, ਪੂਰਨ user_src, uint64 src, uint64 len)
{
  ਬਣਤਰ proc *p = myproc();
  ਜੇ (user_src) {
    ਵਾਪਸ copyin(p->pagetable, p->sz, dst, src, len);
  } ਨਹੀਂਤਾਂ {
    memmove(dst, (ਅੱਖਰ *)src, len);
    ਵਾਪਸ 0;
  }
}

// Print a process listing to console.  For debugging.
// Runs when user types ^P on console.
// No lock to avoid wedging a stuck machine further.
ਖਾਲੀ
procdump(ਖਾਲੀ)
{
  ਸਥਿਰ ਅੱਖਰ *ਹਾਲਤਾਂ[] = {
    // clang-format off
    [ਅਣਵਰਤਿਆ]    = "unused",
    [ਵਰਤਿਆ]      = "used",
    [SLEEPING]  = "sleep ",
    [RUNNABLE]  = "runble",
    [RUNNING]   = "run   ",
    [ZOMBIE]    = "zombie"
    // clang-format on
  };
  ਬਣਤਰ proc *p;
  ਅੱਖਰ *state;

  printk("\n");
  ਲਈ (p = proc; p < &proc[NPROC]; p++) {
    ਜੇ (p->state == ਅਣਵਰਤਿਆ)
      ਜਾਰੀ;
    ਜੇ (p->state >= 0 && p->state < NELEM(ਹਾਲਤਾਂ) && ਹਾਲਤਾਂ[p->state])
      state = ਹਾਲਤਾਂ[p->state];
    ਨਹੀਂਤਾਂ
      state = "???";
    printk("%d %s %s", p->pid, state, p->name);
    printk("\n");
  }
}
