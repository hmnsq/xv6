#समावेश "types.h"
#समावेश "param.h"
#समावेश "memlayout.h"
#समावेश "riscv.h"
#समावेश "spinlock.h"
#समावेश "proc.h"
#समावेश "defs.h"

काष्ठा cpu cpus[NCPU];

काष्ठा proc proc[NPROC];

काष्ठा proc *initproc;

पूर्णांक nextpid = 1;
काष्ठा spinlock pid_lock;

बाह्य व्योम forkret(व्योम);
अटल व्योम freeproc(काष्ठा proc *p);

बाह्य अक्षर trampoline[]; // trampoline.S

// helps ensure that wakeups of wait()ing
// parents are not lost. helps obey the
// memory model when using p->parent.
// must be acquired before any p->lock.
काष्ठा spinlock wait_lock;

// Allocate a page for each process's kernel stack.
// Map it high in memory, followed by an invalid
// guard page.
व्योम
proc_mapstacks(pagetable_t kpgtbl)
{
  काष्ठा proc *p;

  क्रम (p = proc; p < &proc[NPROC]; p++) {
    अक्षर *pa = kalloc();
    अगर (pa == 0)
      panic("kalloc");
    uint64 va = KSTACK((पूर्णांक)(p - proc));
    kvmmap(kpgtbl, va, (uint64)pa, PGSIZE, PTE_R | PTE_W);
  }
}

// initialize the proc table.
व्योम
procinit(व्योम)
{
  काष्ठा proc *p;

  initlock(&pid_lock, "nextpid");
  initlock(&wait_lock, "wait_lock");
  क्रम (p = proc; p < &proc[NPROC]; p++) {
    initlock(&p->lock, "proc");
    p->state = UNUSED;
    p->kstack = KSTACK((पूर्णांक)(p - proc));
  }
}

// Must be called with interrupts disabled,
// to prevent race with process being moved
// to a different CPU.
पूर्णांक
cpuid()
{
  पूर्णांक id = r_tp();
  वापस id;
}

// Return this CPU's cpu struct.
// Interrupts must be disabled.
काष्ठा cpu *
mycpu(व्योम)
{
  पूर्णांक id = cpuid();
  काष्ठा cpu *c = &cpus[id];
  वापस c;
}

// Return the current struct proc *, or zero if none.
काष्ठा proc *
myproc(व्योम)
{
  धकेलो_off();
  काष्ठा cpu *c = mycpu();
  काष्ठा proc *p = c->proc;
  निकालो_off();
  वापस p;
}

पूर्णांक
allocpid()
{
  पूर्णांक pid;

  acquire(&pid_lock);
  pid = nextpid;
  nextpid = nextpid + 1;
  release(&pid_lock);

  वापस pid;
}

// Look in the process table for an UNUSED proc.
// If found, initialize state required to run in the kernel,
// and return with p->lock held.
// If there are no free procs, or a memory allocation fails, return 0.
अटल काष्ठा proc *
allocproc(व्योम)
{
  काष्ठा proc *p;

  क्रम (p = proc; p < &proc[NPROC]; p++) {
    acquire(&p->lock);
    अगर (p->state == UNUSED) {
      जाओ found;
    } अन्यथा {
      release(&p->lock);
    }
  }
  वापस 0;

found:
  p->pid = allocpid();
  p->state = USED;

  // Allocate a trapframe page.
  अगर ((p->trapframe = (काष्ठा trapframe *)kalloc()) == 0) {
    freeproc(p);
    release(&p->lock);
    वापस 0;
  }

  // An empty user page table.
  p->pagetable = proc_pagetable(p);
  अगर (p->pagetable == 0) {
    freeproc(p);
    release(&p->lock);
    वापस 0;
  }

  // Set up new context to start executing at forkret,
  // which returns to user space.
  स_रखो(&p->context, 0, माप(p->context));
  p->context.ra = (uint64)forkret;
  p->context.sp = p->kstack + PGSIZE;

  वापस p;
}

// free a proc structure and the data hanging from it,
// including user pages.
// p->lock must be held.
अटल व्योम
freeproc(काष्ठा proc *p)
{
  अगर (p->trapframe)
    kfree((व्योम *)p->trapframe);
  p->trapframe = 0;
  अगर (p->pagetable)
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
proc_pagetable(काष्ठा proc *p)
{
  pagetable_t pagetable;

  // An empty page table.
  pagetable = uvmcreate();
  अगर (pagetable == 0)
    वापस 0;

  // map the trampoline code (for system call return)
  // at the highest user virtual address.
  // only the supervisor uses it, on the way
  // to/from user space, so not PTE_U.
  अगर (mappages(pagetable, TRAMPOLINE, PGSIZE, (uint64)trampoline,
               PTE_R | PTE_X) < 0) {
    uvmfree(pagetable, 0);
    वापस 0;
  }

  // map the trapframe page just below the trampoline page, for
  // trampoline.S.
  अगर (mappages(pagetable, TRAPFRAME, PGSIZE, (uint64)(p->trapframe),
               PTE_R | PTE_W) < 0) {
    uvmunmap(pagetable, TRAMPOLINE, 1, 0);
    uvmfree(pagetable, 0);
    वापस 0;
  }

  वापस pagetable;
}

// Free a process's page table, and free the
// physical memory it refers to.
व्योम
proc_freepagetable(pagetable_t pagetable, uint64 sz)
{
  uvmunmap(pagetable, TRAMPOLINE, 1, 0);
  uvmunmap(pagetable, TRAPFRAME, 1, 0);
  uvmfree(pagetable, sz);
}

// Set up first user process.
व्योम
userinit(व्योम)
{
  काष्ठा proc *p;

  p = allocproc();
  initproc = p;

  p->cwd = namei("/");

  p->state = RUNNABLE;

  release(&p->lock);
}

// Grow or shrink user memory by n bytes.
// Return 0 on success, -1 on failure.
पूर्णांक
growproc(पूर्णांक n)
{
  uint64 sz;
  काष्ठा proc *p = myproc();

  sz = p->sz;
  अगर (n > 0) {
    अगर (sz + n > TRAPFRAME) {
      वापस -1;
    }
    अगर ((sz = uvmalloc(p->pagetable, sz, sz + n, PTE_W)) == 0) {
      वापस -1;
    }
  } अन्यथा अगर (n < 0) {
    sz = uvmdealloc(p->pagetable, sz, sz + n);
  }
  p->sz = sz;
  वापस 0;
}

// Create a new process, copying the parent.
// Sets up child kernel stack to return as if from fork() system call.
पूर्णांक
kfork(व्योम)
{
  पूर्णांक i, pid;
  काष्ठा proc *np;
  काष्ठा proc *p = myproc();

  // Allocate process.
  अगर ((np = allocproc()) == 0) {
    वापस -1;
  }

  // Copy user memory from parent to child.
  अगर (uvmcopy(p->pagetable, np->pagetable, p->sz) < 0) {
    freeproc(np);
    release(&np->lock);
    वापस -1;
  }
  np->sz = p->sz;

  // copy saved user registers.
  *(np->trapframe) = *(p->trapframe);

  // Cause fork to return 0 in the child.
  np->trapframe->a0 = 0;

  // increment reference counts on open file descriptors.
  क्रम (i = 0; i < NOFILE; i++)
    अगर (p->ofile[i])
      np->ofile[i] = filedup(p->ofile[i]);
  np->cwd = idup(p->cwd);

  safestrcpy(np->name, p->name, माप(p->name));

  pid = np->pid;

  release(&np->lock);

  acquire(&wait_lock);
  np->parent = p;
  release(&wait_lock);

  acquire(&np->lock);
  np->state = RUNNABLE;
  release(&np->lock);

  वापस pid;
}

// Pass p's abandoned children to init.
// Caller must hold wait_lock.
व्योम
reparent(काष्ठा proc *p)
{
  काष्ठा proc *pp;

  क्रम (pp = proc; pp < &proc[NPROC]; pp++) {
    अगर (pp->parent == p) {
      pp->parent = initproc;
      wakeup(initproc);
    }
  }
}

// Exit the current process.  Does not return.
// An exited process remains in the zombie state
// until its parent calls wait().
व्योम
kexit(पूर्णांक status)
{
  काष्ठा proc *p = myproc();

  अगर (p == initproc)
    panic("init exiting");

  // Close all open files.
  क्रम (पूर्णांक fd = 0; fd < NOFILE; fd++) {
    अगर (p->ofile[fd]) {
      काष्ठा file *f = p->ofile[fd];
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
पूर्णांक
kwait(uint64 addr)
{
  काष्ठा proc *pp;
  पूर्णांक havekids, pid;
  काष्ठा proc *p = myproc();

  acquire(&wait_lock);

  क्रम (;;) {
    // Scan through table looking for exited children.
    havekids = 0;
    क्रम (pp = proc; pp < &proc[NPROC]; pp++) {
      अगर (pp->parent == p) {
        // make sure the child isn't still in exit() or swtch().
        acquire(&pp->lock);

        havekids = 1;
        अगर (pp->state == ZOMBIE) {
          // Found one.
          pid = pp->pid;
          अगर (addr != 0 &&
              copyout(p->pagetable, p->sz, addr, (अक्षर *)&pp->xstate,
                      माप(pp->xstate)) < 0) {
            release(&pp->lock);
            release(&wait_lock);
            वापस -1;
          }
          pp->parent = 0;
          freeproc(pp);
          release(&pp->lock);
          release(&wait_lock);
          वापस pid;
        }
        release(&pp->lock);
      }
    }

    // No point waiting if we don't have any children.
    अगर (!havekids || killed(p)) {
      release(&wait_lock);
      वापस -1;
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
व्योम
scheduler(व्योम)
{
  काष्ठा proc *p;
  काष्ठा cpu *c = mycpu();

  c->proc = 0;
  क्रम (;;) {
    // The most recent process to run may have had interrupts
    // turned off; enable them to avoid a deadlock if all
    // processes are waiting. Then turn them back off
    // to avoid a possible race between an interrupt
    // and wfi.
    intr_on();
    intr_off();

    पूर्णांक found = 0;
    क्रम (p = proc; p < &proc[NPROC]; p++) {
      acquire(&p->lock);
      अगर (p->state == RUNNABLE) {
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
    अगर (found == 0) {
      // nothing to run; stop running on this core until an interrupt.
      यंत्र अस्थिर("wfi");
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
व्योम
sched(व्योम)
{
  पूर्णांक intena;
  काष्ठा proc *p = myproc();

  अगर (!holding(&p->lock))
    panic("sched p->lock");
  अगर (mycpu()->noff != 1)
    panic("sched locks");
  अगर (p->state == RUNNING)
    panic("sched RUNNING");
  अगर (intr_get())
    panic("sched interruptible");

  intena = mycpu()->intena;
  swtch(&p->context, &mycpu()->context);
  mycpu()->intena = intena;
}

// Give up the CPU for one scheduling round.
व्योम
yield(व्योम)
{
  काष्ठा proc *p = myproc();
  acquire(&p->lock);
  p->state = RUNNABLE;
  sched();
  release(&p->lock);
}

// A fork child's very first scheduling by scheduler()
// will swtch to forkret.
व्योम
forkret(व्योम)
{
  बाह्य अक्षर userret[];
  अटल पूर्णांक first = 1;
  काष्ठा proc *p = myproc();

  // Still holding p->lock from scheduler.
  release(&p->lock);

  अगर (first) {
    first = 0;

    // File system initialization must be run in the context of a
    // regular process (e.g., because it calls sleep), and thus cannot
    // be run from main().
    fsinit(ROOTDEV);

    // We can invoke kexec() now that file system is initialized.
    // Put the return value (argc) of kexec into a0.
    p->trapframe->a0 = kexec("/init", (अक्षर *[]){"/init", 0});
    अगर (p->trapframe->a0 == -1) {
      panic("exec");
    }
  }

  // return to user space, mimicing usertrap()'s return.
  prepare_return();
  uint64 satp = MAKE_SATP(p->pagetable);
  uint64 trampoline_userret = TRAMPOLINE + (userret - trampoline);
  ((व्योम (*)(uint64))trampoline_userret)(satp);
}

// Register current process as waiting for wakeups on chan.
व्योम
sleep_prepare(व्योम *chan)
{
  काष्ठा proc *p = myproc();

  acquire(&p->lock);
  अगर (chan == 0)
    panic("sleep_prepare: zero chan");
  p->chan = chan;
  release(&p->lock);
}

// Put the thread to sleep.  Assumes sleep_prepare() was called before.
// If the channel registered by sleep_prepare() has been woken up in
// the meantime, do not go to sleep, and instead return immediately.
व्योम
sleep(व्योम)
{
  काष्ठा proc *p = myproc();

  acquire(&p->lock);
  अगर (p->chan != 0) {
    p->state = SLEEPING;
    sched();
  }
  release(&p->lock);
}

// Wake up all processes sleeping on channel chan.
व्योम
wakeup(व्योम *chan)
{
  काष्ठा proc *p;

  क्रम (p = proc; p < &proc[NPROC]; p++) {
    acquire(&p->lock);
    अगर (p->chan == chan) {
      // If the process is waiting for wakeups on this channel,
      // signal that the wakeup happened by clearing p->chan.
      p->chan = 0;

      // If this waiting process has gotten so far as to actually
      // go to sleep, also set it back to RUNNING.
      अगर (p->state == SLEEPING) {
        p->state = RUNNABLE;
      }
    }
    release(&p->lock);
  }
}

// Kill the process with the given pid.
// The victim won't exit until it tries to return
// to user space (see usertrap() in trap.c).
पूर्णांक
kkill(पूर्णांक pid)
{
  काष्ठा proc *p;

  अगर (pid == 0)
    वापस -1;

  क्रम (p = proc; p < &proc[NPROC]; p++) {
    acquire(&p->lock);
    अगर (p->pid == pid) {
      p->killed = 1;
      अगर (p->state == SLEEPING) {
        // Wake process from sleep().
        p->state = RUNNABLE;
      }
      release(&p->lock);
      वापस 0;
    }
    release(&p->lock);
  }
  वापस -1;
}

व्योम
setkilled(काष्ठा proc *p)
{
  acquire(&p->lock);
  p->killed = 1;
  release(&p->lock);
}

पूर्णांक
killed(काष्ठा proc *p)
{
  पूर्णांक k;

  acquire(&p->lock);
  k = p->killed;
  release(&p->lock);
  वापस k;
}

// Copy to either a user address, or kernel address,
// depending on usr_dst.
// Returns 0 on success, -1 on error.
पूर्णांक
either_copyout(पूर्णांक user_dst, uint64 dst, व्योम *src, uint64 len)
{
  काष्ठा proc *p = myproc();
  अगर (user_dst) {
    वापस copyout(p->pagetable, p->sz, dst, src, len);
  } अन्यथा {
    स_हटाओ((अक्षर *)dst, src, len);
    वापस 0;
  }
}

// Copy from either a user address, or kernel address,
// depending on usr_src.
// Returns 0 on success, -1 on error.
पूर्णांक
either_copyin(व्योम *dst, पूर्णांक user_src, uint64 src, uint64 len)
{
  काष्ठा proc *p = myproc();
  अगर (user_src) {
    वापस copyin(p->pagetable, p->sz, dst, src, len);
  } अन्यथा {
    स_हटाओ(dst, (अक्षर *)src, len);
    वापस 0;
  }
}

// Print a process listing to console.  For debugging.
// Runs when user types ^P on console.
// No lock to avoid wedging a stuck machine further.
व्योम
procdump(व्योम)
{
  अटल अक्षर *states[] = {
    // clang-format off
    [UNUSED]    = "unused",
    [USED]      = "used",
    [SLEEPING]  = "sleep ",
    [RUNNABLE]  = "runble",
    [RUNNING]   = "run   ",
    [ZOMBIE]    = "zombie"
    // clang-format on
  };
  काष्ठा proc *p;
  अक्षर *state;

  printk("\n");
  क्रम (p = proc; p < &proc[NPROC]; p++) {
    अगर (p->state == UNUSED)
      जारी;
    अगर (p->state >= 0 && p->state < NELEM(states) && states[p->state])
      state = states[p->state];
    अन्यथा
      state = "???";
    printk("%d %s %s", p->pid, state, p->name);
    printk("\n");
  }
}
