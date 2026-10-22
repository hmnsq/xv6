#అన్తర్భావ "types.h"
#అన్తర్భావ "param.h"
#అన్తర్భావ "memlayout.h"
#అన్తర్భావ "riscv.h"
#అన్తర్భావ "spinlock.h"
#అన్తర్భావ "proc.h"
#అన్తర్భావ "defs.h"

నిర్మాణం cpu cpus[NCPU];

నిర్మాణం proc proc[NPROC];

నిర్మాణం proc *initproc;

పూర్ణాంకం nextpid = 1;
నిర్మాణం spinlock pid_తాళం;

బాహ్య శూన్య forkret(శూన్య);
స్థిర శూన్య freeproc(నిర్మాణం proc *p);

బాహ్య అక్షరం trampoline[]; // trampoline.S

// helps ensure that wakeups of wait()ing
// parents are not lost. helps obey the
// memory model when using p->parent.
// must be acquired before any p->lock.
నిర్మాణం spinlock wait_తాళం;

// Allocate a page for each process's kernel stack.
// Map it high in memory, followed by an invalid
// guard page.
శూన్య
proc_mapstacks(pagetable_t kpgtbl)
{
  నిర్మాణం proc *p;

  కొరకు (p = proc; p < &proc[NPROC]; p++) {
    అక్షరం *pa = kalloc();
    ఒకవేళ (pa == 0)
      panic("kalloc");
    uint64 va = KSTACK((పూర్ణాంకం)(p - proc));
    kvmmap(kpgtbl, va, (uint64)pa, PGSIZE, PTE_R | PTE_W);
  }
}

// initialize the proc table.
శూన్య
procinit(శూన్య)
{
  నిర్మాణం proc *p;

  initlock(&pid_తాళం, "nextpid");
  initlock(&wait_తాళం, "wait_lock");
  కొరకు (p = proc; p < &proc[NPROC]; p++) {
    initlock(&p->తాళం, "proc");
    p->state = వాడని;
    p->kstack = KSTACK((పూర్ణాంకం)(p - proc));
  }
}

// Must be called with interrupts disabled,
// to prevent race with process being moved
// to a different CPU.
పూర్ణాంకం
cpuid()
{
  పూర్ణాంకం id = r_tp();
  తిరిగి id;
}

// Return this CPU's cpu struct.
// Interrupts must be disabled.
నిర్మాణం cpu *
mycpu(శూన్య)
{
  పూర్ణాంకం id = cpuid();
  నిర్మాణం cpu *c = &cpus[id];
  తిరిగి c;
}

// Return the current struct proc *, or zero if none.
నిర్మాణం proc *
myproc(శూన్య)
{
  నెట్టు_off();
  నిర్మాణం cpu *c = mycpu();
  నిర్మాణం proc *p = c->proc;
  తీయి_off();
  తిరిగి p;
}

పూర్ణాంకం
allocpid()
{
  పూర్ణాంకం pid;

  acquire(&pid_తాళం);
  pid = nextpid;
  nextpid = nextpid + 1;
  release(&pid_తాళం);

  తిరిగి pid;
}

// Look in the process table for an UNUSED proc.
// If found, initialize state required to run in the kernel,
// and return with p->lock held.
// If there are no free procs, or a memory allocation fails, return 0.
స్థిర నిర్మాణం proc *
allocproc(శూన్య)
{
  నిర్మాణం proc *p;

  కొరకు (p = proc; p < &proc[NPROC]; p++) {
    acquire(&p->తాళం);
    ఒకవేళ (p->state == వాడని) {
      వెళ్ళు దొరికింది;
    } లేకపోతే {
      release(&p->తాళం);
    }
  }
  తిరిగి 0;

దొరికింది:
  p->pid = allocpid();
  p->state = వాడిన;

  // Allocate a trapframe page.
  ఒకవేళ ((p->trapframe = (నిర్మాణం trapframe *)kalloc()) == 0) {
    freeproc(p);
    release(&p->తాళం);
    తిరిగి 0;
  }

  // An empty user page table.
  p->pagetable = proc_pagetable(p);
  ఒకవేళ (p->pagetable == 0) {
    freeproc(p);
    release(&p->తాళం);
    తిరిగి 0;
  }

  // Set up new context to start executing at forkret,
  // which returns to user space.
  memset(&p->context, 0, పరిమాణం(p->context));
  p->context.ra = (uint64)forkret;
  p->context.sp = p->kstack + PGSIZE;

  తిరిగి p;
}

// free a proc structure and the data hanging from it,
// including user pages.
// p->lock must be held.
స్థిర శూన్య
freeproc(నిర్మాణం proc *p)
{
  ఒకవేళ (p->trapframe)
    kfree((శూన్య *)p->trapframe);
  p->trapframe = 0;
  ఒకవేళ (p->pagetable)
    proc_freepagetable(p->pagetable, p->sz);
  p->pagetable = 0;
  p->sz = 0;
  p->pid = 0;
  p->name[0] = 0;
  p->chan = 0;
  p->killed = 0;
  p->xstate = 0;
  p->state = వాడని;
}

// Create a user page table for a given process, with no user memory,
// but with trampoline and trapframe pages.
pagetable_t
proc_pagetable(నిర్మాణం proc *p)
{
  pagetable_t pagetable;

  // An empty page table.
  pagetable = uvmcreate();
  ఒకవేళ (pagetable == 0)
    తిరిగి 0;

  // map the trampoline code (for system call return)
  // at the highest user virtual address.
  // only the supervisor uses it, on the way
  // to/from user space, so not PTE_U.
  ఒకవేళ (mappages(pagetable, TRAMPOLINE, PGSIZE, (uint64)trampoline,
               PTE_R | PTE_X) < 0) {
    uvmfree(pagetable, 0);
    తిరిగి 0;
  }

  // map the trapframe page just below the trampoline page, for
  // trampoline.S.
  ఒకవేళ (mappages(pagetable, TRAPFRAME, PGSIZE, (uint64)(p->trapframe),
               PTE_R | PTE_W) < 0) {
    uvmunmap(pagetable, TRAMPOLINE, 1, 0);
    uvmfree(pagetable, 0);
    తిరిగి 0;
  }

  తిరిగి pagetable;
}

// Free a process's page table, and free the
// physical memory it refers to.
శూన్య
proc_freepagetable(pagetable_t pagetable, uint64 sz)
{
  uvmunmap(pagetable, TRAMPOLINE, 1, 0);
  uvmunmap(pagetable, TRAPFRAME, 1, 0);
  uvmfree(pagetable, sz);
}

// Set up first user process.
శూన్య
userinit(శూన్య)
{
  నిర్మాణం proc *p;

  p = allocproc();
  initproc = p;

  p->cwd = namei("/");

  p->state = RUNNABLE;

  release(&p->తాళం);
}

// Grow or shrink user memory by n bytes.
// Return 0 on success, -1 on failure.
పూర్ణాంకం
growproc(పూర్ణాంకం n)
{
  uint64 sz;
  నిర్మాణం proc *p = myproc();

  sz = p->sz;
  ఒకవేళ (n > 0) {
    ఒకవేళ (sz + n > TRAPFRAME) {
      తిరిగి -1;
    }
    ఒకవేళ ((sz = uvmalloc(p->pagetable, sz, sz + n, PTE_W)) == 0) {
      తిరిగి -1;
    }
  } లేకపోతే ఒకవేళ (n < 0) {
    sz = uvmdealloc(p->pagetable, sz, sz + n);
  }
  p->sz = sz;
  తిరిగి 0;
}

// Create a new process, copying the parent.
// Sets up child kernel stack to return as if from fork() system call.
పూర్ణాంకం
kfork(శూన్య)
{
  పూర్ణాంకం i, pid;
  నిర్మాణం proc *np;
  నిర్మాణం proc *p = myproc();

  // Allocate process.
  ఒకవేళ ((np = allocproc()) == 0) {
    తిరిగి -1;
  }

  // Copy user memory from parent to child.
  ఒకవేళ (uvmcopy(p->pagetable, np->pagetable, p->sz) < 0) {
    freeproc(np);
    release(&np->తాళం);
    తిరిగి -1;
  }
  np->sz = p->sz;

  // copy saved user registers.
  *(np->trapframe) = *(p->trapframe);

  // Cause fork to return 0 in the child.
  np->trapframe->a0 = 0;

  // increment reference counts on open file descriptors.
  కొరకు (i = 0; i < NOFILE; i++)
    ఒకవేళ (p->ofile[i])
      np->ofile[i] = filedup(p->ofile[i]);
  np->cwd = idup(p->cwd);

  safestrcpy(np->name, p->name, పరిమాణం(p->name));

  pid = np->pid;

  release(&np->తాళం);

  acquire(&wait_తాళం);
  np->మాతృ = p;
  release(&wait_తాళం);

  acquire(&np->తాళం);
  np->state = RUNNABLE;
  release(&np->తాళం);

  తిరిగి pid;
}

// Pass p's abandoned children to init.
// Caller must hold wait_lock.
శూన్య
reparent(నిర్మాణం proc *p)
{
  నిర్మాణం proc *pp;

  కొరకు (pp = proc; pp < &proc[NPROC]; pp++) {
    ఒకవేళ (pp->మాతృ == p) {
      pp->మాతృ = initproc;
      wakeup(initproc);
    }
  }
}

// Exit the current process.  Does not return.
// An exited process remains in the zombie state
// until its parent calls wait().
శూన్య
kexit(పూర్ణాంకం స్థితి_2)
{
  నిర్మాణం proc *p = myproc();

  ఒకవేళ (p == initproc)
    panic("init exiting");

  // Close all open files.
  కొరకు (పూర్ణాంకం fd = 0; fd < NOFILE; fd++) {
    ఒకవేళ (p->ofile[fd]) {
      నిర్మాణం file *f = p->ofile[fd];
      fileclose(f);
      p->ofile[fd] = 0;
    }
  }

  begin_op();
  iput(p->cwd);
  చివర_op();
  p->cwd = 0;

  acquire(&wait_తాళం);

  // Give any children to init.
  reparent(p);

  // Parent might be sleeping in wait().
  wakeup(p->మాతృ);

  acquire(&p->తాళం);

  p->xstate = స్థితి_2;
  p->state = ZOMBIE;

  release(&wait_తాళం);

  // Jump into the scheduler, never to return.
  sched();
  panic("zombie exit");
}

// Wait for a child process to exit and return its pid.
// Return -1 if this process has no children.
పూర్ణాంకం
kwait(uint64 addr)
{
  నిర్మాణం proc *pp;
  పూర్ణాంకం havekids, pid;
  నిర్మాణం proc *p = myproc();

  acquire(&wait_తాళం);

  కొరకు (;;) {
    // Scan through table looking for exited children.
    havekids = 0;
    కొరకు (pp = proc; pp < &proc[NPROC]; pp++) {
      ఒకవేళ (pp->మాతృ == p) {
        // make sure the child isn't still in exit() or swtch().
        acquire(&pp->తాళం);

        havekids = 1;
        ఒకవేళ (pp->state == ZOMBIE) {
          // Found one.
          pid = pp->pid;
          ఒకవేళ (addr != 0 &&
              copyout(p->pagetable, p->sz, addr, (అక్షరం *)&pp->xstate,
                      పరిమాణం(pp->xstate)) < 0) {
            release(&pp->తాళం);
            release(&wait_తాళం);
            తిరిగి -1;
          }
          pp->మాతృ = 0;
          freeproc(pp);
          release(&pp->తాళం);
          release(&wait_తాళం);
          తిరిగి pid;
        }
        release(&pp->తాళం);
      }
    }

    // No point waiting if we don't have any children.
    ఒకవేళ (!havekids || killed(p)) {
      release(&wait_తాళం);
      తిరిగి -1;
    }

    // Wait for a child to exit.
    sleep_prepare(p); //DOC: wait-sleep
    release(&wait_తాళం);
    sleep();
    acquire(&wait_తాళం);
  }
}

// Per-CPU process scheduler.
// Each CPU calls scheduler() after setting itself up.
// Scheduler never returns.  It loops, doing:
//  - choose a process to run.
//  - swtch to start running that process.
//  - eventually that process transfers control
//    via swtch back to the scheduler.
శూన్య
scheduler(శూన్య)
{
  నిర్మాణం proc *p;
  నిర్మాణం cpu *c = mycpu();

  c->proc = 0;
  కొరకు (;;) {
    // The most recent process to run may have had interrupts
    // turned off; enable them to avoid a deadlock if all
    // processes are waiting. Then turn them back off
    // to avoid a possible race between an interrupt
    // and wfi.
    intr_on();
    intr_off();

    పూర్ణాంకం దొరికింది = 0;
    కొరకు (p = proc; p < &proc[NPROC]; p++) {
      acquire(&p->తాళం);
      ఒకవేళ (p->state == RUNNABLE) {
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
        దొరికింది = 1;
      }
      release(&p->తాళం);
    }
    ఒకవేళ (దొరికింది == 0) {
      // nothing to run; stop running on this core until an interrupt.
      asm అస్థిర("wfi");
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
శూన్య
sched(శూన్య)
{
  పూర్ణాంకం intena;
  నిర్మాణం proc *p = myproc();

  ఒకవేళ (!holding(&p->తాళం))
    panic("sched p->lock");
  ఒకవేళ (mycpu()->noff != 1)
    panic("sched locks");
  ఒకవేళ (p->state == RUNNING)
    panic("sched RUNNING");
  ఒకవేళ (intr_get())
    panic("sched interruptible");

  intena = mycpu()->intena;
  swtch(&p->context, &mycpu()->context);
  mycpu()->intena = intena;
}

// Give up the CPU for one scheduling round.
శూన్య
yield(శూన్య)
{
  నిర్మాణం proc *p = myproc();
  acquire(&p->తాళం);
  p->state = RUNNABLE;
  sched();
  release(&p->తాళం);
}

// A fork child's very first scheduling by scheduler()
// will swtch to forkret.
శూన్య
forkret(శూన్య)
{
  బాహ్య అక్షరం userret[];
  స్థిర పూర్ణాంకం మొదటి = 1;
  నిర్మాణం proc *p = myproc();

  // Still holding p->lock from scheduler.
  release(&p->తాళం);

  ఒకవేళ (మొదటి) {
    మొదటి = 0;

    // File system initialization must be run in the context of a
    // regular process (e.g., because it calls sleep), and thus cannot
    // be run from main().
    fsinit(ROOTDEV);

    // We can invoke kexec() now that file system is initialized.
    // Put the return value (argc) of kexec into a0.
    p->trapframe->a0 = kexec("/init", (అక్షరం *[]){"/init", 0});
    ఒకవేళ (p->trapframe->a0 == -1) {
      panic("exec");
    }
  }

  // return to user space, mimicing usertrap()'s return.
  prepare_return();
  uint64 satp = MAKE_SATP(p->pagetable);
  uint64 trampoline_userret = TRAMPOLINE + (userret - trampoline);
  ((శూన్య (*)(uint64))trampoline_userret)(satp);
}

// Register current process as waiting for wakeups on chan.
శూన్య
sleep_prepare(శూన్య *chan)
{
  నిర్మాణం proc *p = myproc();

  acquire(&p->తాళం);
  ఒకవేళ (chan == 0)
    panic("sleep_prepare: zero chan");
  p->chan = chan;
  release(&p->తాళం);
}

// Put the thread to sleep.  Assumes sleep_prepare() was called before.
// If the channel registered by sleep_prepare() has been woken up in
// the meantime, do not go to sleep, and instead return immediately.
శూన్య
sleep(శూన్య)
{
  నిర్మాణం proc *p = myproc();

  acquire(&p->తాళం);
  ఒకవేళ (p->chan != 0) {
    p->state = SLEEPING;
    sched();
  }
  release(&p->తాళం);
}

// Wake up all processes sleeping on channel chan.
శూన్య
wakeup(శూన్య *chan)
{
  నిర్మాణం proc *p;

  కొరకు (p = proc; p < &proc[NPROC]; p++) {
    acquire(&p->తాళం);
    ఒకవేళ (p->chan == chan) {
      // If the process is waiting for wakeups on this channel,
      // signal that the wakeup happened by clearing p->chan.
      p->chan = 0;

      // If this waiting process has gotten so far as to actually
      // go to sleep, also set it back to RUNNING.
      ఒకవేళ (p->state == SLEEPING) {
        p->state = RUNNABLE;
      }
    }
    release(&p->తాళం);
  }
}

// Kill the process with the given pid.
// The victim won't exit until it tries to return
// to user space (see usertrap() in trap.c).
పూర్ణాంకం
kkill(పూర్ణాంకం pid)
{
  నిర్మాణం proc *p;

  ఒకవేళ (pid == 0)
    తిరిగి -1;

  కొరకు (p = proc; p < &proc[NPROC]; p++) {
    acquire(&p->తాళం);
    ఒకవేళ (p->pid == pid) {
      p->killed = 1;
      ఒకవేళ (p->state == SLEEPING) {
        // Wake process from sleep().
        p->state = RUNNABLE;
      }
      release(&p->తాళం);
      తిరిగి 0;
    }
    release(&p->తాళం);
  }
  తిరిగి -1;
}

శూన్య
setkilled(నిర్మాణం proc *p)
{
  acquire(&p->తాళం);
  p->killed = 1;
  release(&p->తాళం);
}

పూర్ణాంకం
killed(నిర్మాణం proc *p)
{
  పూర్ణాంకం k;

  acquire(&p->తాళం);
  k = p->killed;
  release(&p->తాళం);
  తిరిగి k;
}

// Copy to either a user address, or kernel address,
// depending on usr_dst.
// Returns 0 on success, -1 on error.
పూర్ణాంకం
either_copyout(పూర్ణాంకం user_dst, uint64 dst, శూన్య *src, uint64 len)
{
  నిర్మాణం proc *p = myproc();
  ఒకవేళ (user_dst) {
    తిరిగి copyout(p->pagetable, p->sz, dst, src, len);
  } లేకపోతే {
    memmove((అక్షరం *)dst, src, len);
    తిరిగి 0;
  }
}

// Copy from either a user address, or kernel address,
// depending on usr_src.
// Returns 0 on success, -1 on error.
పూర్ణాంకం
either_copyin(శూన్య *dst, పూర్ణాంకం user_src, uint64 src, uint64 len)
{
  నిర్మాణం proc *p = myproc();
  ఒకవేళ (user_src) {
    తిరిగి copyin(p->pagetable, p->sz, dst, src, len);
  } లేకపోతే {
    memmove(dst, (అక్షరం *)src, len);
    తిరిగి 0;
  }
}

// Print a process listing to console.  For debugging.
// Runs when user types ^P on console.
// No lock to avoid wedging a stuck machine further.
శూన్య
procdump(శూన్య)
{
  స్థిర అక్షరం *స్థితులు[] = {
    // clang-format off
    [వాడని]    = "unused",
    [వాడిన]      = "used",
    [SLEEPING]  = "sleep ",
    [RUNNABLE]  = "runble",
    [RUNNING]   = "run   ",
    [ZOMBIE]    = "zombie"
    // clang-format on
  };
  నిర్మాణం proc *p;
  అక్షరం *state;

  printk("\n");
  కొరకు (p = proc; p < &proc[NPROC]; p++) {
    ఒకవేళ (p->state == వాడని)
      కొనసాగు;
    ఒకవేళ (p->state >= 0 && p->state < NELEM(స్థితులు) && స్థితులు[p->state])
      state = స్థితులు[p->state];
    లేకపోతే
      state = "???";
    printk("%d %s %s", p->pid, state, p->name);
    printk("\n");
  }
}
