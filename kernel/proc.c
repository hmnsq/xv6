#شامل "types.h"
#شامل "param.h"
#شامل "memlayout.h"
#شامل "riscv.h"
#شامل "spinlock.h"
#شامل "proc.h"
#شامل "defs.h"

ساختار cpu cpus[NCPU];

ساختار proc proc[NPROC];

ساختار proc *initproc;

صحیح nextpid = 1;
ساختار spinlock pid_lock;

بیرونی تهی forkret(تهی);
ایستا تهی freeproc(ساختار proc *p);

بیرونی نویسه trampoline[]; // trampoline.S

// helps ensure that wakeups of wait()ing
// parents are not lost. helps obey the
// memory model when using p->parent.
// must be acquired before any p->lock.
ساختار spinlock wait_lock;

// Allocate a page for each process's kernel stack.
// Map it high in memory, followed by an invalid
// guard page.
تهی
proc_mapstacks(pagetable_t kpgtbl)
{
  ساختار proc *p;

  برای (p = proc; p < &proc[NPROC]; p++) {
    نویسه *pa = kalloc();
    اگر (pa == 0)
      panic("kalloc");
    uint64 va = KSTACK((صحیح)(p - proc));
    kvmmap(kpgtbl, va, (uint64)pa, PGSIZE, PTE_R | PTE_W);
  }
}

// initialize the proc table.
تهی
procinit(تهی)
{
  ساختار proc *p;

  initlock(&pid_lock, "nextpid");
  initlock(&wait_lock, "wait_lock");
  برای (p = proc; p < &proc[NPROC]; p++) {
    initlock(&p->lock, "proc");
    p->state = UNUSED;
    p->kstack = KSTACK((صحیح)(p - proc));
  }
}

// Must be called with interrupts disabled,
// to prevent race with process being moved
// to a different CPU.
صحیح
cpuid()
{
  صحیح id = r_tp();
  بازگشت id;
}

// Return this CPU's cpu struct.
// Interrupts must be disabled.
ساختار cpu *
mycpu(تهی)
{
  صحیح id = cpuid();
  ساختار cpu *c = &cpus[id];
  بازگشت c;
}

// Return the current struct proc *, or zero if none.
ساختار proc *
myproc(تهی)
{
  افزودن_off();
  ساختار cpu *c = mycpu();
  ساختار proc *p = c->proc;
  برداشت_off();
  بازگشت p;
}

صحیح
allocpid()
{
  صحیح pid;

  acquire(&pid_lock);
  pid = nextpid;
  nextpid = nextpid + 1;
  release(&pid_lock);

  بازگشت pid;
}

// Look in the process table for an UNUSED proc.
// If found, initialize state required to run in the kernel,
// and return with p->lock held.
// If there are no free procs, or a memory allocation fails, return 0.
ایستا ساختار proc *
allocproc(تهی)
{
  ساختار proc *p;

  برای (p = proc; p < &proc[NPROC]; p++) {
    acquire(&p->lock);
    اگر (p->state == UNUSED) {
      برو found;
    } وگرنه {
      release(&p->lock);
    }
  }
  بازگشت 0;

found:
  p->pid = allocpid();
  p->state = USED;

  // Allocate a trapframe page.
  اگر ((p->trapframe = (ساختار trapframe *)kalloc()) == 0) {
    freeproc(p);
    release(&p->lock);
    بازگشت 0;
  }

  // An empty user page table.
  p->pagetable = proc_pagetable(p);
  اگر (p->pagetable == 0) {
    freeproc(p);
    release(&p->lock);
    بازگشت 0;
  }

  // Set up new context to start executing at forkret,
  // which returns to user space.
  memset(&p->context, 0, اندازه(p->context));
  p->context.ra = (uint64)forkret;
  p->context.sp = p->kstack + PGSIZE;

  بازگشت p;
}

// free a proc structure and the data hanging from it,
// including user pages.
// p->lock must be held.
ایستا تهی
freeproc(ساختار proc *p)
{
  اگر (p->trapframe)
    kfree((تهی *)p->trapframe);
  p->trapframe = 0;
  اگر (p->pagetable)
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
proc_pagetable(ساختار proc *p)
{
  pagetable_t pagetable;

  // An empty page table.
  pagetable = uvmcreate();
  اگر (pagetable == 0)
    بازگشت 0;

  // map the trampoline code (for system call return)
  // at the highest user virtual address.
  // only the supervisor uses it, on the way
  // to/from user space, so not PTE_U.
  اگر (mappages(pagetable, TRAMPOLINE, PGSIZE, (uint64)trampoline,
               PTE_R | PTE_X) < 0) {
    uvmfree(pagetable, 0);
    بازگشت 0;
  }

  // map the trapframe page just below the trampoline page, for
  // trampoline.S.
  اگر (mappages(pagetable, TRAPFRAME, PGSIZE, (uint64)(p->trapframe),
               PTE_R | PTE_W) < 0) {
    uvmunmap(pagetable, TRAMPOLINE, 1, 0);
    uvmfree(pagetable, 0);
    بازگشت 0;
  }

  بازگشت pagetable;
}

// Free a process's page table, and free the
// physical memory it refers to.
تهی
proc_freepagetable(pagetable_t pagetable, uint64 sz)
{
  uvmunmap(pagetable, TRAMPOLINE, 1, 0);
  uvmunmap(pagetable, TRAPFRAME, 1, 0);
  uvmfree(pagetable, sz);
}

// Set up first user process.
تهی
userinit(تهی)
{
  ساختار proc *p;

  p = allocproc();
  initproc = p;

  p->cwd = namei("/");

  p->state = RUNNABLE;

  release(&p->lock);
}

// Grow or shrink user memory by n bytes.
// Return 0 on success, -1 on failure.
صحیح
growproc(صحیح n)
{
  uint64 sz;
  ساختار proc *p = myproc();

  sz = p->sz;
  اگر (n > 0) {
    اگر (sz + n > TRAPFRAME) {
      بازگشت -1;
    }
    اگر ((sz = uvmalloc(p->pagetable, sz, sz + n, PTE_W)) == 0) {
      بازگشت -1;
    }
  } وگرنه اگر (n < 0) {
    sz = uvmdealloc(p->pagetable, sz, sz + n);
  }
  p->sz = sz;
  بازگشت 0;
}

// Create a new process, copying the parent.
// Sets up child kernel stack to return as if from fork() system call.
صحیح
kfork(تهی)
{
  صحیح i, pid;
  ساختار proc *np;
  ساختار proc *p = myproc();

  // Allocate process.
  اگر ((np = allocproc()) == 0) {
    بازگشت -1;
  }

  // Copy user memory from parent to child.
  اگر (uvmcopy(p->pagetable, np->pagetable, p->sz) < 0) {
    freeproc(np);
    release(&np->lock);
    بازگشت -1;
  }
  np->sz = p->sz;

  // copy saved user registers.
  *(np->trapframe) = *(p->trapframe);

  // Cause fork to return 0 in the child.
  np->trapframe->a0 = 0;

  // increment reference counts on open file descriptors.
  برای (i = 0; i < NOFILE; i++)
    اگر (p->ofile[i])
      np->ofile[i] = filedup(p->ofile[i]);
  np->cwd = idup(p->cwd);

  safestrcpy(np->name, p->name, اندازه(p->name));

  pid = np->pid;

  release(&np->lock);

  acquire(&wait_lock);
  np->parent = p;
  release(&wait_lock);

  acquire(&np->lock);
  np->state = RUNNABLE;
  release(&np->lock);

  بازگشت pid;
}

// Pass p's abandoned children to init.
// Caller must hold wait_lock.
تهی
reparent(ساختار proc *p)
{
  ساختار proc *pp;

  برای (pp = proc; pp < &proc[NPROC]; pp++) {
    اگر (pp->parent == p) {
      pp->parent = initproc;
      wakeup(initproc);
    }
  }
}

// Exit the current process.  Does not return.
// An exited process remains in the zombie state
// until its parent calls wait().
تهی
kexit(صحیح status)
{
  ساختار proc *p = myproc();

  اگر (p == initproc)
    panic("init exiting");

  // Close all open files.
  برای (صحیح fd = 0; fd < NOFILE; fd++) {
    اگر (p->ofile[fd]) {
      ساختار file *f = p->ofile[fd];
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
صحیح
kwait(uint64 addr)
{
  ساختار proc *pp;
  صحیح havekids, pid;
  ساختار proc *p = myproc();

  acquire(&wait_lock);

  برای (;;) {
    // Scan through table looking for exited children.
    havekids = 0;
    برای (pp = proc; pp < &proc[NPROC]; pp++) {
      اگر (pp->parent == p) {
        // make sure the child isn't still in exit() or swtch().
        acquire(&pp->lock);

        havekids = 1;
        اگر (pp->state == ZOMBIE) {
          // Found one.
          pid = pp->pid;
          اگر (addr != 0 &&
              copyout(p->pagetable, p->sz, addr, (نویسه *)&pp->xstate,
                      اندازه(pp->xstate)) < 0) {
            release(&pp->lock);
            release(&wait_lock);
            بازگشت -1;
          }
          pp->parent = 0;
          freeproc(pp);
          release(&pp->lock);
          release(&wait_lock);
          بازگشت pid;
        }
        release(&pp->lock);
      }
    }

    // No point waiting if we don't have any children.
    اگر (!havekids || killed(p)) {
      release(&wait_lock);
      بازگشت -1;
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
تهی
scheduler(تهی)
{
  ساختار proc *p;
  ساختار cpu *c = mycpu();

  c->proc = 0;
  برای (;;) {
    // The most recent process to run may have had interrupts
    // turned off; enable them to avoid a deadlock if all
    // processes are waiting. Then turn them back off
    // to avoid a possible race between an interrupt
    // and wfi.
    intr_on();
    intr_off();

    صحیح found = 0;
    برای (p = proc; p < &proc[NPROC]; p++) {
      acquire(&p->lock);
      اگر (p->state == RUNNABLE) {
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
    اگر (found == 0) {
      // nothing to run; stop running on this core until an interrupt.
      asm فرار("wfi");
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
تهی
sched(تهی)
{
  صحیح intena;
  ساختار proc *p = myproc();

  اگر (!holding(&p->lock))
    panic("sched p->lock");
  اگر (mycpu()->noff != 1)
    panic("sched locks");
  اگر (p->state == RUNNING)
    panic("sched RUNNING");
  اگر (intr_get())
    panic("sched interruptible");

  intena = mycpu()->intena;
  swtch(&p->context, &mycpu()->context);
  mycpu()->intena = intena;
}

// Give up the CPU for one scheduling round.
تهی
yield(تهی)
{
  ساختار proc *p = myproc();
  acquire(&p->lock);
  p->state = RUNNABLE;
  sched();
  release(&p->lock);
}

// A fork child's very first scheduling by scheduler()
// will swtch to forkret.
تهی
forkret(تهی)
{
  بیرونی نویسه userret[];
  ایستا صحیح first = 1;
  ساختار proc *p = myproc();

  // Still holding p->lock from scheduler.
  release(&p->lock);

  اگر (first) {
    first = 0;

    // File system initialization must be run in the context of a
    // regular process (e.g., because it calls sleep), and thus cannot
    // be run from main().
    fsinit(ROOTDEV);

    // We can invoke kexec() now that file system is initialized.
    // Put the return value (argc) of kexec into a0.
    p->trapframe->a0 = kexec("/init", (نویسه *[]){"/init", 0});
    اگر (p->trapframe->a0 == -1) {
      panic("exec");
    }
  }

  // return to user space, mimicing usertrap()'s return.
  prepare_return();
  uint64 satp = MAKE_SATP(p->pagetable);
  uint64 trampoline_userret = TRAMPOLINE + (userret - trampoline);
  ((تهی (*)(uint64))trampoline_userret)(satp);
}

// Register current process as waiting for wakeups on chan.
تهی
sleep_prepare(تهی *chan)
{
  ساختار proc *p = myproc();

  acquire(&p->lock);
  اگر (chan == 0)
    panic("sleep_prepare: zero chan");
  p->chan = chan;
  release(&p->lock);
}

// Put the thread to sleep.  Assumes sleep_prepare() was called before.
// If the channel registered by sleep_prepare() has been woken up in
// the meantime, do not go to sleep, and instead return immediately.
تهی
sleep(تهی)
{
  ساختار proc *p = myproc();

  acquire(&p->lock);
  اگر (p->chan != 0) {
    p->state = SLEEPING;
    sched();
  }
  release(&p->lock);
}

// Wake up all processes sleeping on channel chan.
تهی
wakeup(تهی *chan)
{
  ساختار proc *p;

  برای (p = proc; p < &proc[NPROC]; p++) {
    acquire(&p->lock);
    اگر (p->chan == chan) {
      // If the process is waiting for wakeups on this channel,
      // signal that the wakeup happened by clearing p->chan.
      p->chan = 0;

      // If this waiting process has gotten so far as to actually
      // go to sleep, also set it back to RUNNING.
      اگر (p->state == SLEEPING) {
        p->state = RUNNABLE;
      }
    }
    release(&p->lock);
  }
}

// Kill the process with the given pid.
// The victim won't exit until it tries to return
// to user space (see usertrap() in trap.c).
صحیح
kkill(صحیح pid)
{
  ساختار proc *p;

  اگر (pid == 0)
    بازگشت -1;

  برای (p = proc; p < &proc[NPROC]; p++) {
    acquire(&p->lock);
    اگر (p->pid == pid) {
      p->killed = 1;
      اگر (p->state == SLEEPING) {
        // Wake process from sleep().
        p->state = RUNNABLE;
      }
      release(&p->lock);
      بازگشت 0;
    }
    release(&p->lock);
  }
  بازگشت -1;
}

تهی
setkilled(ساختار proc *p)
{
  acquire(&p->lock);
  p->killed = 1;
  release(&p->lock);
}

صحیح
killed(ساختار proc *p)
{
  صحیح k;

  acquire(&p->lock);
  k = p->killed;
  release(&p->lock);
  بازگشت k;
}

// Copy to either a user address, or kernel address,
// depending on usr_dst.
// Returns 0 on success, -1 on error.
صحیح
either_copyout(صحیح user_dst, uint64 dst, تهی *src, uint64 len)
{
  ساختار proc *p = myproc();
  اگر (user_dst) {
    بازگشت copyout(p->pagetable, p->sz, dst, src, len);
  } وگرنه {
    memmove((نویسه *)dst, src, len);
    بازگشت 0;
  }
}

// Copy from either a user address, or kernel address,
// depending on usr_src.
// Returns 0 on success, -1 on error.
صحیح
either_copyin(تهی *dst, صحیح user_src, uint64 src, uint64 len)
{
  ساختار proc *p = myproc();
  اگر (user_src) {
    بازگشت copyin(p->pagetable, p->sz, dst, src, len);
  } وگرنه {
    memmove(dst, (نویسه *)src, len);
    بازگشت 0;
  }
}

// Print a process listing to console.  For debugging.
// Runs when user types ^P on console.
// No lock to avoid wedging a stuck machine further.
تهی
procdump(تهی)
{
  ایستا نویسه *states[] = {
    // clang-format off
    [UNUSED]    = "unused",
    [USED]      = "used",
    [SLEEPING]  = "sleep ",
    [RUNNABLE]  = "runble",
    [RUNNING]   = "run   ",
    [ZOMBIE]    = "zombie"
    // clang-format on
  };
  ساختار proc *p;
  نویسه *state;

  printk("\n");
  برای (p = proc; p < &proc[NPROC]; p++) {
    اگر (p->state == UNUSED)
      ادامه;
    اگر (p->state >= 0 && p->state < NELEM(states) && states[p->state])
      state = states[p->state];
    وگرنه
      state = "???";
    printk("%d %s %s", p->pid, state, p->name);
    printk("\n");
  }
}
