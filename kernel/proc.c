#ଅନ୍ତର୍ଭାଵ "types.h"
#ଅନ୍ତର୍ଭାଵ "param.h"
#ଅନ୍ତର୍ଭାଵ "memlayout.h"
#ଅନ୍ତର୍ଭାଵ "riscv.h"
#ଅନ୍ତର୍ଭାଵ "spinlock.h"
#ଅନ୍ତର୍ଭାଵ "proc.h"
#ଅନ୍ତର୍ଭାଵ "defs.h"

ସଂରଚନା cpu cpus[NCPU];

ସଂରଚନା proc proc[NPROC];

ସଂରଚନା proc *initproc;

ପୂର୍ଣ୍ଣସଂଖ୍ୟା nextpid = 1;
ସଂରଚନା spinlock pid_lock;

ବାହ୍ୟ ଶୂନ୍ୟ forkret(ଶୂନ୍ୟ);
ସ୍ଥିର ଶୂନ୍ୟ freeproc(ସଂରଚନା proc *p);

ବାହ୍ୟ ଅକ୍ଷର trampoline[]; // trampoline.S

// helps ensure that wakeups of wait()ing
// parents are not lost. helps obey the
// memory model when using p->parent.
// must be acquired before any p->lock.
ସଂରଚନା spinlock wait_lock;

// Allocate a page for each process's kernel stack.
// Map it high in memory, followed by an invalid
// guard page.
ଶୂନ୍ୟ
proc_mapstacks(pagetable_t kpgtbl)
{
  ସଂରଚନା proc *p;

  ପାଇଁ (p = proc; p < &proc[NPROC]; p++) {
    ଅକ୍ଷର *pa = kalloc();
    ଯଦି (pa == 0)
      panic("kalloc");
    uint64 va = KSTACK((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(p - proc));
    kvmmap(kpgtbl, va, (uint64)pa, PGSIZE, PTE_R | PTE_W);
  }
}

// initialize the proc table.
ଶୂନ୍ୟ
procinit(ଶୂନ୍ୟ)
{
  ସଂରଚନା proc *p;

  initlock(&pid_lock, "nextpid");
  initlock(&wait_lock, "wait_lock");
  ପାଇଁ (p = proc; p < &proc[NPROC]; p++) {
    initlock(&p->lock, "proc");
    p->state = UNUSED;
    p->kstack = KSTACK((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(p - proc));
  }
}

// Must be called with interrupts disabled,
// to prevent race with process being moved
// to a different CPU.
ପୂର୍ଣ୍ଣସଂଖ୍ୟା
cpuid()
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା id = r_tp();
  ଫେରନ୍ତୁ id;
}

// Return this CPU's cpu struct.
// Interrupts must be disabled.
ସଂରଚନା cpu *
mycpu(ଶୂନ୍ୟ)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା id = cpuid();
  ସଂରଚନା cpu *c = &cpus[id];
  ଫେରନ୍ତୁ c;
}

// Return the current struct proc *, or zero if none.
ସଂରଚନା proc *
myproc(ଶୂନ୍ୟ)
{
  ଠେଲ_off();
  ସଂରଚନା cpu *c = mycpu();
  ସଂରଚନା proc *p = c->proc;
  କାଢ଼_off();
  ଫେରନ୍ତୁ p;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା
allocpid()
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା pid;

  acquire(&pid_lock);
  pid = nextpid;
  nextpid = nextpid + 1;
  release(&pid_lock);

  ଫେରନ୍ତୁ pid;
}

// Look in the process table for an UNUSED proc.
// If found, initialize state required to run in the kernel,
// and return with p->lock held.
// If there are no free procs, or a memory allocation fails, return 0.
ସ୍ଥିର ସଂରଚନା proc *
allocproc(ଶୂନ୍ୟ)
{
  ସଂରଚନା proc *p;

  ପାଇଁ (p = proc; p < &proc[NPROC]; p++) {
    acquire(&p->lock);
    ଯଦି (p->state == UNUSED) {
      ଯାଅ found;
    } ଅନ୍ୟଥା {
      release(&p->lock);
    }
  }
  ଫେରନ୍ତୁ 0;

found:
  p->pid = allocpid();
  p->state = USED;

  // Allocate a trapframe page.
  ଯଦି ((p->trapframe = (ସଂରଚନା trapframe *)kalloc()) == 0) {
    freeproc(p);
    release(&p->lock);
    ଫେରନ୍ତୁ 0;
  }

  // An empty user page table.
  p->pagetable = proc_pagetable(p);
  ଯଦି (p->pagetable == 0) {
    freeproc(p);
    release(&p->lock);
    ଫେରନ୍ତୁ 0;
  }

  // Set up new context to start executing at forkret,
  // which returns to user space.
  memset(&p->context, 0, ଆକାର(p->context));
  p->context.ra = (uint64)forkret;
  p->context.sp = p->kstack + PGSIZE;

  ଫେରନ୍ତୁ p;
}

// free a proc structure and the data hanging from it,
// including user pages.
// p->lock must be held.
ସ୍ଥିର ଶୂନ୍ୟ
freeproc(ସଂରଚନା proc *p)
{
  ଯଦି (p->trapframe)
    kfree((ଶୂନ୍ୟ *)p->trapframe);
  p->trapframe = 0;
  ଯଦି (p->pagetable)
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
proc_pagetable(ସଂରଚନା proc *p)
{
  pagetable_t pagetable;

  // An empty page table.
  pagetable = uvmcreate();
  ଯଦି (pagetable == 0)
    ଫେରନ୍ତୁ 0;

  // map the trampoline code (for system call return)
  // at the highest user virtual address.
  // only the supervisor uses it, on the way
  // to/from user space, so not PTE_U.
  ଯଦି (mappages(pagetable, TRAMPOLINE, PGSIZE, (uint64)trampoline,
               PTE_R | PTE_X) < 0) {
    uvmfree(pagetable, 0);
    ଫେରନ୍ତୁ 0;
  }

  // map the trapframe page just below the trampoline page, for
  // trampoline.S.
  ଯଦି (mappages(pagetable, TRAPFRAME, PGSIZE, (uint64)(p->trapframe),
               PTE_R | PTE_W) < 0) {
    uvmunmap(pagetable, TRAMPOLINE, 1, 0);
    uvmfree(pagetable, 0);
    ଫେରନ୍ତୁ 0;
  }

  ଫେରନ୍ତୁ pagetable;
}

// Free a process's page table, and free the
// physical memory it refers to.
ଶୂନ୍ୟ
proc_freepagetable(pagetable_t pagetable, uint64 sz)
{
  uvmunmap(pagetable, TRAMPOLINE, 1, 0);
  uvmunmap(pagetable, TRAPFRAME, 1, 0);
  uvmfree(pagetable, sz);
}

// Set up first user process.
ଶୂନ୍ୟ
userinit(ଶୂନ୍ୟ)
{
  ସଂରଚନା proc *p;

  p = allocproc();
  initproc = p;

  p->cwd = namei("/");

  p->state = RUNNABLE;

  release(&p->lock);
}

// Grow or shrink user memory by n bytes.
// Return 0 on success, -1 on failure.
ପୂର୍ଣ୍ଣସଂଖ୍ୟା
growproc(ପୂର୍ଣ୍ଣସଂଖ୍ୟା n)
{
  uint64 sz;
  ସଂରଚନା proc *p = myproc();

  sz = p->sz;
  ଯଦି (n > 0) {
    ଯଦି (sz + n > TRAPFRAME) {
      ଫେରନ୍ତୁ -1;
    }
    ଯଦି ((sz = uvmalloc(p->pagetable, sz, sz + n, PTE_W)) == 0) {
      ଫେରନ୍ତୁ -1;
    }
  } ଅନ୍ୟଥା ଯଦି (n < 0) {
    sz = uvmdealloc(p->pagetable, sz, sz + n);
  }
  p->sz = sz;
  ଫେରନ୍ତୁ 0;
}

// Create a new process, copying the parent.
// Sets up child kernel stack to return as if from fork() system call.
ପୂର୍ଣ୍ଣସଂଖ୍ୟା
kfork(ଶୂନ୍ୟ)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା i, pid;
  ସଂରଚନା proc *np;
  ସଂରଚନା proc *p = myproc();

  // Allocate process.
  ଯଦି ((np = allocproc()) == 0) {
    ଫେରନ୍ତୁ -1;
  }

  // Copy user memory from parent to child.
  ଯଦି (uvmcopy(p->pagetable, np->pagetable, p->sz) < 0) {
    freeproc(np);
    release(&np->lock);
    ଫେରନ୍ତୁ -1;
  }
  np->sz = p->sz;

  // copy saved user registers.
  *(np->trapframe) = *(p->trapframe);

  // Cause fork to return 0 in the child.
  np->trapframe->a0 = 0;

  // increment reference counts on open file descriptors.
  ପାଇଁ (i = 0; i < NOFILE; i++)
    ଯଦି (p->ofile[i])
      np->ofile[i] = filedup(p->ofile[i]);
  np->cwd = idup(p->cwd);

  safestrcpy(np->name, p->name, ଆକାର(p->name));

  pid = np->pid;

  release(&np->lock);

  acquire(&wait_lock);
  np->parent = p;
  release(&wait_lock);

  acquire(&np->lock);
  np->state = RUNNABLE;
  release(&np->lock);

  ଫେରନ୍ତୁ pid;
}

// Pass p's abandoned children to init.
// Caller must hold wait_lock.
ଶୂନ୍ୟ
reparent(ସଂରଚନା proc *p)
{
  ସଂରଚନା proc *pp;

  ପାଇଁ (pp = proc; pp < &proc[NPROC]; pp++) {
    ଯଦି (pp->parent == p) {
      pp->parent = initproc;
      wakeup(initproc);
    }
  }
}

// Exit the current process.  Does not return.
// An exited process remains in the zombie state
// until its parent calls wait().
ଶୂନ୍ୟ
kexit(ପୂର୍ଣ୍ଣସଂଖ୍ୟା status)
{
  ସଂରଚନା proc *p = myproc();

  ଯଦି (p == initproc)
    panic("init exiting");

  // Close all open files.
  ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd = 0; fd < NOFILE; fd++) {
    ଯଦି (p->ofile[fd]) {
      ସଂରଚନା file *f = p->ofile[fd];
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
ପୂର୍ଣ୍ଣସଂଖ୍ୟା
kwait(uint64 addr)
{
  ସଂରଚନା proc *pp;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା havekids, pid;
  ସଂରଚନା proc *p = myproc();

  acquire(&wait_lock);

  ପାଇଁ (;;) {
    // Scan through table looking for exited children.
    havekids = 0;
    ପାଇଁ (pp = proc; pp < &proc[NPROC]; pp++) {
      ଯଦି (pp->parent == p) {
        // make sure the child isn't still in exit() or swtch().
        acquire(&pp->lock);

        havekids = 1;
        ଯଦି (pp->state == ZOMBIE) {
          // Found one.
          pid = pp->pid;
          ଯଦି (addr != 0 &&
              copyout(p->pagetable, p->sz, addr, (ଅକ୍ଷର *)&pp->xstate,
                      ଆକାର(pp->xstate)) < 0) {
            release(&pp->lock);
            release(&wait_lock);
            ଫେରନ୍ତୁ -1;
          }
          pp->parent = 0;
          freeproc(pp);
          release(&pp->lock);
          release(&wait_lock);
          ଫେରନ୍ତୁ pid;
        }
        release(&pp->lock);
      }
    }

    // No point waiting if we don't have any children.
    ଯଦି (!havekids || killed(p)) {
      release(&wait_lock);
      ଫେରନ୍ତୁ -1;
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
ଶୂନ୍ୟ
scheduler(ଶୂନ୍ୟ)
{
  ସଂରଚନା proc *p;
  ସଂରଚନା cpu *c = mycpu();

  c->proc = 0;
  ପାଇଁ (;;) {
    // The most recent process to run may have had interrupts
    // turned off; enable them to avoid a deadlock if all
    // processes are waiting. Then turn them back off
    // to avoid a possible race between an interrupt
    // and wfi.
    intr_on();
    intr_off();

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା found = 0;
    ପାଇଁ (p = proc; p < &proc[NPROC]; p++) {
      acquire(&p->lock);
      ଯଦି (p->state == RUNNABLE) {
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
    ଯଦି (found == 0) {
      // nothing to run; stop running on this core until an interrupt.
      asm ଅସ୍ଥିର("wfi");
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
ଶୂନ୍ୟ
sched(ଶୂନ୍ୟ)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା intena;
  ସଂରଚନା proc *p = myproc();

  ଯଦି (!holding(&p->lock))
    panic("sched p->lock");
  ଯଦି (mycpu()->noff != 1)
    panic("sched locks");
  ଯଦି (p->state == RUNNING)
    panic("sched RUNNING");
  ଯଦି (intr_get())
    panic("sched interruptible");

  intena = mycpu()->intena;
  swtch(&p->context, &mycpu()->context);
  mycpu()->intena = intena;
}

// Give up the CPU for one scheduling round.
ଶୂନ୍ୟ
yield(ଶୂନ୍ୟ)
{
  ସଂରଚନା proc *p = myproc();
  acquire(&p->lock);
  p->state = RUNNABLE;
  sched();
  release(&p->lock);
}

// A fork child's very first scheduling by scheduler()
// will swtch to forkret.
ଶୂନ୍ୟ
forkret(ଶୂନ୍ୟ)
{
  ବାହ୍ୟ ଅକ୍ଷର userret[];
  ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା first = 1;
  ସଂରଚନା proc *p = myproc();

  // Still holding p->lock from scheduler.
  release(&p->lock);

  ଯଦି (first) {
    first = 0;

    // File system initialization must be run in the context of a
    // regular process (e.g., because it calls sleep), and thus cannot
    // be run from main().
    fsinit(ROOTDEV);

    // We can invoke kexec() now that file system is initialized.
    // Put the return value (argc) of kexec into a0.
    p->trapframe->a0 = kexec("/init", (ଅକ୍ଷର *[]){"/init", 0});
    ଯଦି (p->trapframe->a0 == -1) {
      panic("exec");
    }
  }

  // return to user space, mimicing usertrap()'s return.
  prepare_return();
  uint64 satp = MAKE_SATP(p->pagetable);
  uint64 trampoline_userret = TRAMPOLINE + (userret - trampoline);
  ((ଶୂନ୍ୟ (*)(uint64))trampoline_userret)(satp);
}

// Register current process as waiting for wakeups on chan.
ଶୂନ୍ୟ
sleep_prepare(ଶୂନ୍ୟ *chan)
{
  ସଂରଚନା proc *p = myproc();

  acquire(&p->lock);
  ଯଦି (chan == 0)
    panic("sleep_prepare: zero chan");
  p->chan = chan;
  release(&p->lock);
}

// Put the thread to sleep.  Assumes sleep_prepare() was called before.
// If the channel registered by sleep_prepare() has been woken up in
// the meantime, do not go to sleep, and instead return immediately.
ଶୂନ୍ୟ
sleep(ଶୂନ୍ୟ)
{
  ସଂରଚନା proc *p = myproc();

  acquire(&p->lock);
  ଯଦି (p->chan != 0) {
    p->state = SLEEPING;
    sched();
  }
  release(&p->lock);
}

// Wake up all processes sleeping on channel chan.
ଶୂନ୍ୟ
wakeup(ଶୂନ୍ୟ *chan)
{
  ସଂରଚନା proc *p;

  ପାଇଁ (p = proc; p < &proc[NPROC]; p++) {
    acquire(&p->lock);
    ଯଦି (p->chan == chan) {
      // If the process is waiting for wakeups on this channel,
      // signal that the wakeup happened by clearing p->chan.
      p->chan = 0;

      // If this waiting process has gotten so far as to actually
      // go to sleep, also set it back to RUNNING.
      ଯଦି (p->state == SLEEPING) {
        p->state = RUNNABLE;
      }
    }
    release(&p->lock);
  }
}

// Kill the process with the given pid.
// The victim won't exit until it tries to return
// to user space (see usertrap() in trap.c).
ପୂର୍ଣ୍ଣସଂଖ୍ୟା
kkill(ପୂର୍ଣ୍ଣସଂଖ୍ୟା pid)
{
  ସଂରଚନା proc *p;

  ଯଦି (pid == 0)
    ଫେରନ୍ତୁ -1;

  ପାଇଁ (p = proc; p < &proc[NPROC]; p++) {
    acquire(&p->lock);
    ଯଦି (p->pid == pid) {
      p->killed = 1;
      ଯଦି (p->state == SLEEPING) {
        // Wake process from sleep().
        p->state = RUNNABLE;
      }
      release(&p->lock);
      ଫେରନ୍ତୁ 0;
    }
    release(&p->lock);
  }
  ଫେରନ୍ତୁ -1;
}

ଶୂନ୍ୟ
setkilled(ସଂରଚନା proc *p)
{
  acquire(&p->lock);
  p->killed = 1;
  release(&p->lock);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା
killed(ସଂରଚନା proc *p)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା k;

  acquire(&p->lock);
  k = p->killed;
  release(&p->lock);
  ଫେରନ୍ତୁ k;
}

// Copy to either a user address, or kernel address,
// depending on usr_dst.
// Returns 0 on success, -1 on error.
ପୂର୍ଣ୍ଣସଂଖ୍ୟା
either_copyout(ପୂର୍ଣ୍ଣସଂଖ୍ୟା user_dst, uint64 dst, ଶୂନ୍ୟ *src, uint64 len)
{
  ସଂରଚନା proc *p = myproc();
  ଯଦି (user_dst) {
    ଫେରନ୍ତୁ copyout(p->pagetable, p->sz, dst, src, len);
  } ଅନ୍ୟଥା {
    memmove((ଅକ୍ଷର *)dst, src, len);
    ଫେରନ୍ତୁ 0;
  }
}

// Copy from either a user address, or kernel address,
// depending on usr_src.
// Returns 0 on success, -1 on error.
ପୂର୍ଣ୍ଣସଂଖ୍ୟା
either_copyin(ଶୂନ୍ୟ *dst, ପୂର୍ଣ୍ଣସଂଖ୍ୟା user_src, uint64 src, uint64 len)
{
  ସଂରଚନା proc *p = myproc();
  ଯଦି (user_src) {
    ଫେରନ୍ତୁ copyin(p->pagetable, p->sz, dst, src, len);
  } ଅନ୍ୟଥା {
    memmove(dst, (ଅକ୍ଷର *)src, len);
    ଫେରନ୍ତୁ 0;
  }
}

// Print a process listing to console.  For debugging.
// Runs when user types ^P on console.
// No lock to avoid wedging a stuck machine further.
ଶୂନ୍ୟ
procdump(ଶୂନ୍ୟ)
{
  ସ୍ଥିର ଅକ୍ଷର *states[] = {
    // clang-format off
    [UNUSED]    = "unused",
    [USED]      = "used",
    [SLEEPING]  = "sleep ",
    [RUNNABLE]  = "runble",
    [RUNNING]   = "run   ",
    [ZOMBIE]    = "zombie"
    // clang-format on
  };
  ସଂରଚନା proc *p;
  ଅକ୍ଷର *state;

  printk("\n");
  ପାଇଁ (p = proc; p < &proc[NPROC]; p++) {
    ଯଦି (p->state == UNUSED)
      ଜାରି;
    ଯଦି (p->state >= 0 && p->state < NELEM(states) && states[p->state])
      state = states[p->state];
    ଅନ୍ୟଥା
      state = "???";
    printk("%d %s %s", p->pid, state, p->name);
    printk("\n");
  }
}
