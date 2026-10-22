#ಅನ್ತರ್ಭಾವ "types.h"
#ಅನ್ತರ್ಭಾವ "param.h"
#ಅನ್ತರ್ಭಾವ "memlayout.h"
#ಅನ್ತರ್ಭಾವ "riscv.h"
#ಅನ್ತರ್ಭಾವ "spinlock.h"
#ಅನ್ತರ್ಭಾವ "proc.h"
#ಅನ್ತರ್ಭಾವ "defs.h"

ಸಂರಚನಾ cpu cpus[NCPU];

ಸಂರಚನಾ proc proc[NPROC];

ಸಂರಚನಾ proc *initproc;

ಪೂರ್ಣಾಙ್ಕ nextpid = 1;
ಸಂರಚನಾ spinlock pid_lock;

ಬಾಹ್ಯ ಶೂನ್ಯ forkret(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ freeproc(ಸಂರಚನಾ proc *p);

ಬಾಹ್ಯ ವರ್ಣ trampoline[]; // trampoline.S

// helps ensure that wakeups of wait()ing
// parents are not lost. helps obey the
// memory model when using p->parent.
// must be acquired before any p->lock.
ಸಂರಚನಾ spinlock wait_lock;

// Allocate a page for each process's kernel stack.
// Map it high in memory, followed by an invalid
// guard page.
ಶೂನ್ಯ
proc_mapstacks(pagetable_t kpgtbl)
{
  ಸಂರಚನಾ proc *p;

  ಪ್ರತಿ (p = proc; p < &proc[NPROC]; p++) {
    ವರ್ಣ *pa = kalloc();
    ಯದಿ (pa == 0)
      panic("kalloc");
    uint64 va = KSTACK((ಪೂರ್ಣಾಙ್ಕ)(p - proc));
    kvmmap(kpgtbl, va, (uint64)pa, PGSIZE, PTE_R | PTE_W);
  }
}

// initialize the proc table.
ಶೂನ್ಯ
procinit(ಶೂನ್ಯ)
{
  ಸಂರಚನಾ proc *p;

  initlock(&pid_lock, "nextpid");
  initlock(&wait_lock, "wait_lock");
  ಪ್ರತಿ (p = proc; p < &proc[NPROC]; p++) {
    initlock(&p->lock, "proc");
    p->state = UNUSED;
    p->kstack = KSTACK((ಪೂರ್ಣಾಙ್ಕ)(p - proc));
  }
}

// Must be called with interrupts disabled,
// to prevent race with process being moved
// to a different CPU.
ಪೂರ್ಣಾಙ್ಕ
cpuid()
{
  ಪೂರ್ಣಾಙ್ಕ id = r_tp();
  ಪ್ರತ್ಯಾಗಮ id;
}

// Return this CPU's cpu struct.
// Interrupts must be disabled.
ಸಂರಚನಾ cpu *
mycpu(ಶೂನ್ಯ)
{
  ಪೂರ್ಣಾಙ್ಕ id = cpuid();
  ಸಂರಚನಾ cpu *c = &cpus[id];
  ಪ್ರತ್ಯಾಗಮ c;
}

// Return the current struct proc *, or zero if none.
ಸಂರಚನಾ proc *
myproc(ಶೂನ್ಯ)
{
  ತಳ್ಳು_off();
  ಸಂರಚನಾ cpu *c = mycpu();
  ಸಂರಚನಾ proc *p = c->proc;
  ತೆಗೆ_off();
  ಪ್ರತ್ಯಾಗಮ p;
}

ಪೂರ್ಣಾಙ್ಕ
allocpid()
{
  ಪೂರ್ಣಾಙ್ಕ pid;

  acquire(&pid_lock);
  pid = nextpid;
  nextpid = nextpid + 1;
  release(&pid_lock);

  ಪ್ರತ್ಯಾಗಮ pid;
}

// Look in the process table for an UNUSED proc.
// If found, initialize state required to run in the kernel,
// and return with p->lock held.
// If there are no free procs, or a memory allocation fails, return 0.
ಸ್ಥಿರ ಸಂರಚನಾ proc *
allocproc(ಶೂನ್ಯ)
{
  ಸಂರಚನಾ proc *p;

  ಪ್ರತಿ (p = proc; p < &proc[NPROC]; p++) {
    acquire(&p->lock);
    ಯದಿ (p->state == UNUSED) {
      ಗಚ್ಛ found;
    } ಅನ್ಯಥಾ {
      release(&p->lock);
    }
  }
  ಪ್ರತ್ಯಾಗಮ 0;

found:
  p->pid = allocpid();
  p->state = USED;

  // Allocate a trapframe page.
  ಯದಿ ((p->trapframe = (ಸಂರಚನಾ trapframe *)kalloc()) == 0) {
    freeproc(p);
    release(&p->lock);
    ಪ್ರತ್ಯಾಗಮ 0;
  }

  // An empty user page table.
  p->pagetable = proc_pagetable(p);
  ಯದಿ (p->pagetable == 0) {
    freeproc(p);
    release(&p->lock);
    ಪ್ರತ್ಯಾಗಮ 0;
  }

  // Set up new context to start executing at forkret,
  // which returns to user space.
  memset(&p->context, 0, ಪರಿಮಾಣ(p->context));
  p->context.ra = (uint64)forkret;
  p->context.sp = p->kstack + PGSIZE;

  ಪ್ರತ್ಯಾಗಮ p;
}

// free a proc structure and the data hanging from it,
// including user pages.
// p->lock must be held.
ಸ್ಥಿರ ಶೂನ್ಯ
freeproc(ಸಂರಚನಾ proc *p)
{
  ಯದಿ (p->trapframe)
    kfree((ಶೂನ್ಯ *)p->trapframe);
  p->trapframe = 0;
  ಯದಿ (p->pagetable)
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
proc_pagetable(ಸಂರಚನಾ proc *p)
{
  pagetable_t pagetable;

  // An empty page table.
  pagetable = uvmcreate();
  ಯದಿ (pagetable == 0)
    ಪ್ರತ್ಯಾಗಮ 0;

  // map the trampoline code (for system call return)
  // at the highest user virtual address.
  // only the supervisor uses it, on the way
  // to/from user space, so not PTE_U.
  ಯದಿ (mappages(pagetable, TRAMPOLINE, PGSIZE, (uint64)trampoline,
               PTE_R | PTE_X) < 0) {
    uvmfree(pagetable, 0);
    ಪ್ರತ್ಯಾಗಮ 0;
  }

  // map the trapframe page just below the trampoline page, for
  // trampoline.S.
  ಯದಿ (mappages(pagetable, TRAPFRAME, PGSIZE, (uint64)(p->trapframe),
               PTE_R | PTE_W) < 0) {
    uvmunmap(pagetable, TRAMPOLINE, 1, 0);
    uvmfree(pagetable, 0);
    ಪ್ರತ್ಯಾಗಮ 0;
  }

  ಪ್ರತ್ಯಾಗಮ pagetable;
}

// Free a process's page table, and free the
// physical memory it refers to.
ಶೂನ್ಯ
proc_freepagetable(pagetable_t pagetable, uint64 sz)
{
  uvmunmap(pagetable, TRAMPOLINE, 1, 0);
  uvmunmap(pagetable, TRAPFRAME, 1, 0);
  uvmfree(pagetable, sz);
}

// Set up first user process.
ಶೂನ್ಯ
userinit(ಶೂನ್ಯ)
{
  ಸಂರಚನಾ proc *p;

  p = allocproc();
  initproc = p;

  p->cwd = namei("/");

  p->state = RUNNABLE;

  release(&p->lock);
}

// Grow or shrink user memory by n bytes.
// Return 0 on success, -1 on failure.
ಪೂರ್ಣಾಙ್ಕ
growproc(ಪೂರ್ಣಾಙ್ಕ n)
{
  uint64 sz;
  ಸಂರಚನಾ proc *p = myproc();

  sz = p->sz;
  ಯದಿ (n > 0) {
    ಯದಿ (sz + n > TRAPFRAME) {
      ಪ್ರತ್ಯಾಗಮ -1;
    }
    ಯದಿ ((sz = uvmalloc(p->pagetable, sz, sz + n, PTE_W)) == 0) {
      ಪ್ರತ್ಯಾಗಮ -1;
    }
  } ಅನ್ಯಥಾ ಯದಿ (n < 0) {
    sz = uvmdealloc(p->pagetable, sz, sz + n);
  }
  p->sz = sz;
  ಪ್ರತ್ಯಾಗಮ 0;
}

// Create a new process, copying the parent.
// Sets up child kernel stack to return as if from fork() system call.
ಪೂರ್ಣಾಙ್ಕ
kfork(ಶೂನ್ಯ)
{
  ಪೂರ್ಣಾಙ್ಕ i, pid;
  ಸಂರಚನಾ proc *np;
  ಸಂರಚನಾ proc *p = myproc();

  // Allocate process.
  ಯದಿ ((np = allocproc()) == 0) {
    ಪ್ರತ್ಯಾಗಮ -1;
  }

  // Copy user memory from parent to child.
  ಯದಿ (uvmcopy(p->pagetable, np->pagetable, p->sz) < 0) {
    freeproc(np);
    release(&np->lock);
    ಪ್ರತ್ಯಾಗಮ -1;
  }
  np->sz = p->sz;

  // copy saved user registers.
  *(np->trapframe) = *(p->trapframe);

  // Cause fork to return 0 in the child.
  np->trapframe->a0 = 0;

  // increment reference counts on open file descriptors.
  ಪ್ರತಿ (i = 0; i < NOFILE; i++)
    ಯದಿ (p->ofile[i])
      np->ofile[i] = filedup(p->ofile[i]);
  np->cwd = idup(p->cwd);

  safestrcpy(np->name, p->name, ಪರಿಮಾಣ(p->name));

  pid = np->pid;

  release(&np->lock);

  acquire(&wait_lock);
  np->parent = p;
  release(&wait_lock);

  acquire(&np->lock);
  np->state = RUNNABLE;
  release(&np->lock);

  ಪ್ರತ್ಯಾಗಮ pid;
}

// Pass p's abandoned children to init.
// Caller must hold wait_lock.
ಶೂನ್ಯ
reparent(ಸಂರಚನಾ proc *p)
{
  ಸಂರಚನಾ proc *pp;

  ಪ್ರತಿ (pp = proc; pp < &proc[NPROC]; pp++) {
    ಯದಿ (pp->parent == p) {
      pp->parent = initproc;
      wakeup(initproc);
    }
  }
}

// Exit the current process.  Does not return.
// An exited process remains in the zombie state
// until its parent calls wait().
ಶೂನ್ಯ
kexit(ಪೂರ್ಣಾಙ್ಕ status)
{
  ಸಂರಚನಾ proc *p = myproc();

  ಯದಿ (p == initproc)
    panic("init exiting");

  // Close all open files.
  ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ fd = 0; fd < NOFILE; fd++) {
    ಯದಿ (p->ofile[fd]) {
      ಸಂರಚನಾ file *f = p->ofile[fd];
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
ಪೂರ್ಣಾಙ್ಕ
kwait(uint64 addr)
{
  ಸಂರಚನಾ proc *pp;
  ಪೂರ್ಣಾಙ್ಕ havekids, pid;
  ಸಂರಚನಾ proc *p = myproc();

  acquire(&wait_lock);

  ಪ್ರತಿ (;;) {
    // Scan through table looking for exited children.
    havekids = 0;
    ಪ್ರತಿ (pp = proc; pp < &proc[NPROC]; pp++) {
      ಯದಿ (pp->parent == p) {
        // make sure the child isn't still in exit() or swtch().
        acquire(&pp->lock);

        havekids = 1;
        ಯದಿ (pp->state == ZOMBIE) {
          // Found one.
          pid = pp->pid;
          ಯದಿ (addr != 0 &&
              copyout(p->pagetable, p->sz, addr, (ವರ್ಣ *)&pp->xstate,
                      ಪರಿಮಾಣ(pp->xstate)) < 0) {
            release(&pp->lock);
            release(&wait_lock);
            ಪ್ರತ್ಯಾಗಮ -1;
          }
          pp->parent = 0;
          freeproc(pp);
          release(&pp->lock);
          release(&wait_lock);
          ಪ್ರತ್ಯಾಗಮ pid;
        }
        release(&pp->lock);
      }
    }

    // No point waiting if we don't have any children.
    ಯದಿ (!havekids || killed(p)) {
      release(&wait_lock);
      ಪ್ರತ್ಯಾಗಮ -1;
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
ಶೂನ್ಯ
scheduler(ಶೂನ್ಯ)
{
  ಸಂರಚನಾ proc *p;
  ಸಂರಚನಾ cpu *c = mycpu();

  c->proc = 0;
  ಪ್ರತಿ (;;) {
    // The most recent process to run may have had interrupts
    // turned off; enable them to avoid a deadlock if all
    // processes are waiting. Then turn them back off
    // to avoid a possible race between an interrupt
    // and wfi.
    intr_on();
    intr_off();

    ಪೂರ್ಣಾಙ್ಕ found = 0;
    ಪ್ರತಿ (p = proc; p < &proc[NPROC]; p++) {
      acquire(&p->lock);
      ಯದಿ (p->state == RUNNABLE) {
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
    ಯದಿ (found == 0) {
      // nothing to run; stop running on this core until an interrupt.
      asm ಚಞ್ಚಲ("wfi");
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
ಶೂನ್ಯ
sched(ಶೂನ್ಯ)
{
  ಪೂರ್ಣಾಙ್ಕ intena;
  ಸಂರಚನಾ proc *p = myproc();

  ಯದಿ (!holding(&p->lock))
    panic("sched p->lock");
  ಯದಿ (mycpu()->noff != 1)
    panic("sched locks");
  ಯದಿ (p->state == RUNNING)
    panic("sched RUNNING");
  ಯದಿ (intr_get())
    panic("sched interruptible");

  intena = mycpu()->intena;
  swtch(&p->context, &mycpu()->context);
  mycpu()->intena = intena;
}

// Give up the CPU for one scheduling round.
ಶೂನ್ಯ
yield(ಶೂನ್ಯ)
{
  ಸಂರಚನಾ proc *p = myproc();
  acquire(&p->lock);
  p->state = RUNNABLE;
  sched();
  release(&p->lock);
}

// A fork child's very first scheduling by scheduler()
// will swtch to forkret.
ಶೂನ್ಯ
forkret(ಶೂನ್ಯ)
{
  ಬಾಹ್ಯ ವರ್ಣ userret[];
  ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ first = 1;
  ಸಂರಚನಾ proc *p = myproc();

  // Still holding p->lock from scheduler.
  release(&p->lock);

  ಯದಿ (first) {
    first = 0;

    // File system initialization must be run in the context of a
    // regular process (e.g., because it calls sleep), and thus cannot
    // be run from main().
    fsinit(ROOTDEV);

    // We can invoke kexec() now that file system is initialized.
    // Put the return value (argc) of kexec into a0.
    p->trapframe->a0 = kexec("/init", (ವರ್ಣ *[]){"/init", 0});
    ಯದಿ (p->trapframe->a0 == -1) {
      panic("exec");
    }
  }

  // return to user space, mimicing usertrap()'s return.
  prepare_return();
  uint64 satp = MAKE_SATP(p->pagetable);
  uint64 trampoline_userret = TRAMPOLINE + (userret - trampoline);
  ((ಶೂನ್ಯ (*)(uint64))trampoline_userret)(satp);
}

// Register current process as waiting for wakeups on chan.
ಶೂನ್ಯ
sleep_prepare(ಶೂನ್ಯ *chan)
{
  ಸಂರಚನಾ proc *p = myproc();

  acquire(&p->lock);
  ಯದಿ (chan == 0)
    panic("sleep_prepare: zero chan");
  p->chan = chan;
  release(&p->lock);
}

// Put the thread to sleep.  Assumes sleep_prepare() was called before.
// If the channel registered by sleep_prepare() has been woken up in
// the meantime, do not go to sleep, and instead return immediately.
ಶೂನ್ಯ
sleep(ಶೂನ್ಯ)
{
  ಸಂರಚನಾ proc *p = myproc();

  acquire(&p->lock);
  ಯದಿ (p->chan != 0) {
    p->state = SLEEPING;
    sched();
  }
  release(&p->lock);
}

// Wake up all processes sleeping on channel chan.
ಶೂನ್ಯ
wakeup(ಶೂನ್ಯ *chan)
{
  ಸಂರಚನಾ proc *p;

  ಪ್ರತಿ (p = proc; p < &proc[NPROC]; p++) {
    acquire(&p->lock);
    ಯದಿ (p->chan == chan) {
      // If the process is waiting for wakeups on this channel,
      // signal that the wakeup happened by clearing p->chan.
      p->chan = 0;

      // If this waiting process has gotten so far as to actually
      // go to sleep, also set it back to RUNNING.
      ಯದಿ (p->state == SLEEPING) {
        p->state = RUNNABLE;
      }
    }
    release(&p->lock);
  }
}

// Kill the process with the given pid.
// The victim won't exit until it tries to return
// to user space (see usertrap() in trap.c).
ಪೂರ್ಣಾಙ್ಕ
kkill(ಪೂರ್ಣಾಙ್ಕ pid)
{
  ಸಂರಚನಾ proc *p;

  ಯದಿ (pid == 0)
    ಪ್ರತ್ಯಾಗಮ -1;

  ಪ್ರತಿ (p = proc; p < &proc[NPROC]; p++) {
    acquire(&p->lock);
    ಯದಿ (p->pid == pid) {
      p->killed = 1;
      ಯದಿ (p->state == SLEEPING) {
        // Wake process from sleep().
        p->state = RUNNABLE;
      }
      release(&p->lock);
      ಪ್ರತ್ಯಾಗಮ 0;
    }
    release(&p->lock);
  }
  ಪ್ರತ್ಯಾಗಮ -1;
}

ಶೂನ್ಯ
setkilled(ಸಂರಚನಾ proc *p)
{
  acquire(&p->lock);
  p->killed = 1;
  release(&p->lock);
}

ಪೂರ್ಣಾಙ್ಕ
killed(ಸಂರಚನಾ proc *p)
{
  ಪೂರ್ಣಾಙ್ಕ k;

  acquire(&p->lock);
  k = p->killed;
  release(&p->lock);
  ಪ್ರತ್ಯಾಗಮ k;
}

// Copy to either a user address, or kernel address,
// depending on usr_dst.
// Returns 0 on success, -1 on error.
ಪೂರ್ಣಾಙ್ಕ
either_copyout(ಪೂರ್ಣಾಙ್ಕ user_dst, uint64 dst, ಶೂನ್ಯ *src, uint64 len)
{
  ಸಂರಚನಾ proc *p = myproc();
  ಯದಿ (user_dst) {
    ಪ್ರತ್ಯಾಗಮ copyout(p->pagetable, p->sz, dst, src, len);
  } ಅನ್ಯಥಾ {
    memmove((ವರ್ಣ *)dst, src, len);
    ಪ್ರತ್ಯಾಗಮ 0;
  }
}

// Copy from either a user address, or kernel address,
// depending on usr_src.
// Returns 0 on success, -1 on error.
ಪೂರ್ಣಾಙ್ಕ
either_copyin(ಶೂನ್ಯ *dst, ಪೂರ್ಣಾಙ್ಕ user_src, uint64 src, uint64 len)
{
  ಸಂರಚನಾ proc *p = myproc();
  ಯದಿ (user_src) {
    ಪ್ರತ್ಯಾಗಮ copyin(p->pagetable, p->sz, dst, src, len);
  } ಅನ್ಯಥಾ {
    memmove(dst, (ವರ್ಣ *)src, len);
    ಪ್ರತ್ಯಾಗಮ 0;
  }
}

// Print a process listing to console.  For debugging.
// Runs when user types ^P on console.
// No lock to avoid wedging a stuck machine further.
ಶೂನ್ಯ
procdump(ಶೂನ್ಯ)
{
  ಸ್ಥಿರ ವರ್ಣ *states[] = {
    // clang-format off
    [UNUSED]    = "unused",
    [USED]      = "used",
    [SLEEPING]  = "sleep ",
    [RUNNABLE]  = "runble",
    [RUNNING]   = "run   ",
    [ZOMBIE]    = "zombie"
    // clang-format on
  };
  ಸಂರಚನಾ proc *p;
  ವರ್ಣ *state;

  printk("\n");
  ಪ್ರತಿ (p = proc; p < &proc[NPROC]; p++) {
    ಯದಿ (p->state == UNUSED)
      ಅನುವರ್ತ;
    ಯದಿ (p->state >= 0 && p->state < NELEM(states) && states[p->state])
      state = states[p->state];
    ಅನ್ಯಥಾ
      state = "???";
    printk("%d %s %s", p->pid, state, p->name);
    printk("\n");
  }
}
