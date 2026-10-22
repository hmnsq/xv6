#അന്തര്ഭാവ "types.h"
#അന്തര്ഭാവ "param.h"
#അന്തര്ഭാവ "memlayout.h"
#അന്തര്ഭാവ "riscv.h"
#അന്തര്ഭാവ "spinlock.h"
#അന്തര്ഭാവ "proc.h"
#അന്തര്ഭാവ "defs.h"

ഘടന cpu cpus[NCPU];

ഘടന proc proc[NPROC];

ഘടന proc *initproc;

പൂർണ്ണസംഖ്യ nextpid = 1;
ഘടന spinlock pid_lock;

ബാഹ്യ ശൂന്യം forkret(ശൂന്യം);
സ്ഥിതം ശൂന്യം freeproc(ഘടന proc *p);

ബാഹ്യ അക്ഷരം trampoline[]; // trampoline.S

// helps ensure that wakeups of wait()ing
// parents are not lost. helps obey the
// memory model when using p->parent.
// must be acquired before any p->lock.
ഘടന spinlock wait_lock;

// Allocate a page for each process's kernel stack.
// Map it high in memory, followed by an invalid
// guard page.
ശൂന്യം
proc_mapstacks(pagetable_t kpgtbl)
{
  ഘടന proc *p;

  വേണ്ടി (p = proc; p < &proc[NPROC]; p++) {
    അക്ഷരം *pa = kalloc();
    എങ്കിൽ (pa == 0)
      panic("kalloc");
    uint64 va = KSTACK((പൂർണ്ണസംഖ്യ)(p - proc));
    kvmmap(kpgtbl, va, (uint64)pa, PGSIZE, PTE_R | PTE_W);
  }
}

// initialize the proc table.
ശൂന്യം
procinit(ശൂന്യം)
{
  ഘടന proc *p;

  initlock(&pid_lock, "nextpid");
  initlock(&wait_lock, "wait_lock");
  വേണ്ടി (p = proc; p < &proc[NPROC]; p++) {
    initlock(&p->lock, "proc");
    p->state = UNUSED;
    p->kstack = KSTACK((പൂർണ്ണസംഖ്യ)(p - proc));
  }
}

// Must be called with interrupts disabled,
// to prevent race with process being moved
// to a different CPU.
പൂർണ്ണസംഖ്യ
cpuid()
{
  പൂർണ്ണസംഖ്യ id = r_tp();
  തിരികെ id;
}

// Return this CPU's cpu struct.
// Interrupts must be disabled.
ഘടന cpu *
mycpu(ശൂന്യം)
{
  പൂർണ്ണസംഖ്യ id = cpuid();
  ഘടന cpu *c = &cpus[id];
  തിരികെ c;
}

// Return the current struct proc *, or zero if none.
ഘടന proc *
myproc(ശൂന്യം)
{
  തള്ളുക_off();
  ഘടന cpu *c = mycpu();
  ഘടന proc *p = c->proc;
  എടുക്കുക_off();
  തിരികെ p;
}

പൂർണ്ണസംഖ്യ
allocpid()
{
  പൂർണ്ണസംഖ്യ pid;

  acquire(&pid_lock);
  pid = nextpid;
  nextpid = nextpid + 1;
  release(&pid_lock);

  തിരികെ pid;
}

// Look in the process table for an UNUSED proc.
// If found, initialize state required to run in the kernel,
// and return with p->lock held.
// If there are no free procs, or a memory allocation fails, return 0.
സ്ഥിതം ഘടന proc *
allocproc(ശൂന്യം)
{
  ഘടന proc *p;

  വേണ്ടി (p = proc; p < &proc[NPROC]; p++) {
    acquire(&p->lock);
    എങ്കിൽ (p->state == UNUSED) {
      പോകുക found;
    } അല്ലെങ്കിൽ {
      release(&p->lock);
    }
  }
  തിരികെ 0;

found:
  p->pid = allocpid();
  p->state = USED;

  // Allocate a trapframe page.
  എങ്കിൽ ((p->trapframe = (ഘടന trapframe *)kalloc()) == 0) {
    freeproc(p);
    release(&p->lock);
    തിരികെ 0;
  }

  // An empty user page table.
  p->pagetable = proc_pagetable(p);
  എങ്കിൽ (p->pagetable == 0) {
    freeproc(p);
    release(&p->lock);
    തിരികെ 0;
  }

  // Set up new context to start executing at forkret,
  // which returns to user space.
  memset(&p->context, 0, വലുപ്പം(p->context));
  p->context.ra = (uint64)forkret;
  p->context.sp = p->kstack + PGSIZE;

  തിരികെ p;
}

// free a proc structure and the data hanging from it,
// including user pages.
// p->lock must be held.
സ്ഥിതം ശൂന്യം
freeproc(ഘടന proc *p)
{
  എങ്കിൽ (p->trapframe)
    kfree((ശൂന്യം *)p->trapframe);
  p->trapframe = 0;
  എങ്കിൽ (p->pagetable)
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
proc_pagetable(ഘടന proc *p)
{
  pagetable_t pagetable;

  // An empty page table.
  pagetable = uvmcreate();
  എങ്കിൽ (pagetable == 0)
    തിരികെ 0;

  // map the trampoline code (for system call return)
  // at the highest user virtual address.
  // only the supervisor uses it, on the way
  // to/from user space, so not PTE_U.
  എങ്കിൽ (mappages(pagetable, TRAMPOLINE, PGSIZE, (uint64)trampoline,
               PTE_R | PTE_X) < 0) {
    uvmfree(pagetable, 0);
    തിരികെ 0;
  }

  // map the trapframe page just below the trampoline page, for
  // trampoline.S.
  എങ്കിൽ (mappages(pagetable, TRAPFRAME, PGSIZE, (uint64)(p->trapframe),
               PTE_R | PTE_W) < 0) {
    uvmunmap(pagetable, TRAMPOLINE, 1, 0);
    uvmfree(pagetable, 0);
    തിരികെ 0;
  }

  തിരികെ pagetable;
}

// Free a process's page table, and free the
// physical memory it refers to.
ശൂന്യം
proc_freepagetable(pagetable_t pagetable, uint64 sz)
{
  uvmunmap(pagetable, TRAMPOLINE, 1, 0);
  uvmunmap(pagetable, TRAPFRAME, 1, 0);
  uvmfree(pagetable, sz);
}

// Set up first user process.
ശൂന്യം
userinit(ശൂന്യം)
{
  ഘടന proc *p;

  p = allocproc();
  initproc = p;

  p->cwd = namei("/");

  p->state = RUNNABLE;

  release(&p->lock);
}

// Grow or shrink user memory by n bytes.
// Return 0 on success, -1 on failure.
പൂർണ്ണസംഖ്യ
growproc(പൂർണ്ണസംഖ്യ n)
{
  uint64 sz;
  ഘടന proc *p = myproc();

  sz = p->sz;
  എങ്കിൽ (n > 0) {
    എങ്കിൽ (sz + n > TRAPFRAME) {
      തിരികെ -1;
    }
    എങ്കിൽ ((sz = uvmalloc(p->pagetable, sz, sz + n, PTE_W)) == 0) {
      തിരികെ -1;
    }
  } അല്ലെങ്കിൽ എങ്കിൽ (n < 0) {
    sz = uvmdealloc(p->pagetable, sz, sz + n);
  }
  p->sz = sz;
  തിരികെ 0;
}

// Create a new process, copying the parent.
// Sets up child kernel stack to return as if from fork() system call.
പൂർണ്ണസംഖ്യ
kfork(ശൂന്യം)
{
  പൂർണ്ണസംഖ്യ i, pid;
  ഘടന proc *np;
  ഘടന proc *p = myproc();

  // Allocate process.
  എങ്കിൽ ((np = allocproc()) == 0) {
    തിരികെ -1;
  }

  // Copy user memory from parent to child.
  എങ്കിൽ (uvmcopy(p->pagetable, np->pagetable, p->sz) < 0) {
    freeproc(np);
    release(&np->lock);
    തിരികെ -1;
  }
  np->sz = p->sz;

  // copy saved user registers.
  *(np->trapframe) = *(p->trapframe);

  // Cause fork to return 0 in the child.
  np->trapframe->a0 = 0;

  // increment reference counts on open file descriptors.
  വേണ്ടി (i = 0; i < NOFILE; i++)
    എങ്കിൽ (p->ofile[i])
      np->ofile[i] = filedup(p->ofile[i]);
  np->cwd = idup(p->cwd);

  safestrcpy(np->name, p->name, വലുപ്പം(p->name));

  pid = np->pid;

  release(&np->lock);

  acquire(&wait_lock);
  np->parent = p;
  release(&wait_lock);

  acquire(&np->lock);
  np->state = RUNNABLE;
  release(&np->lock);

  തിരികെ pid;
}

// Pass p's abandoned children to init.
// Caller must hold wait_lock.
ശൂന്യം
reparent(ഘടന proc *p)
{
  ഘടന proc *pp;

  വേണ്ടി (pp = proc; pp < &proc[NPROC]; pp++) {
    എങ്കിൽ (pp->parent == p) {
      pp->parent = initproc;
      wakeup(initproc);
    }
  }
}

// Exit the current process.  Does not return.
// An exited process remains in the zombie state
// until its parent calls wait().
ശൂന്യം
kexit(പൂർണ്ണസംഖ്യ status)
{
  ഘടന proc *p = myproc();

  എങ്കിൽ (p == initproc)
    panic("init exiting");

  // Close all open files.
  വേണ്ടി (പൂർണ്ണസംഖ്യ fd = 0; fd < NOFILE; fd++) {
    എങ്കിൽ (p->ofile[fd]) {
      ഘടന file *f = p->ofile[fd];
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
പൂർണ്ണസംഖ്യ
kwait(uint64 addr)
{
  ഘടന proc *pp;
  പൂർണ്ണസംഖ്യ havekids, pid;
  ഘടന proc *p = myproc();

  acquire(&wait_lock);

  വേണ്ടി (;;) {
    // Scan through table looking for exited children.
    havekids = 0;
    വേണ്ടി (pp = proc; pp < &proc[NPROC]; pp++) {
      എങ്കിൽ (pp->parent == p) {
        // make sure the child isn't still in exit() or swtch().
        acquire(&pp->lock);

        havekids = 1;
        എങ്കിൽ (pp->state == ZOMBIE) {
          // Found one.
          pid = pp->pid;
          എങ്കിൽ (addr != 0 &&
              copyout(p->pagetable, p->sz, addr, (അക്ഷരം *)&pp->xstate,
                      വലുപ്പം(pp->xstate)) < 0) {
            release(&pp->lock);
            release(&wait_lock);
            തിരികെ -1;
          }
          pp->parent = 0;
          freeproc(pp);
          release(&pp->lock);
          release(&wait_lock);
          തിരികെ pid;
        }
        release(&pp->lock);
      }
    }

    // No point waiting if we don't have any children.
    എങ്കിൽ (!havekids || killed(p)) {
      release(&wait_lock);
      തിരികെ -1;
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
ശൂന്യം
scheduler(ശൂന്യം)
{
  ഘടന proc *p;
  ഘടന cpu *c = mycpu();

  c->proc = 0;
  വേണ്ടി (;;) {
    // The most recent process to run may have had interrupts
    // turned off; enable them to avoid a deadlock if all
    // processes are waiting. Then turn them back off
    // to avoid a possible race between an interrupt
    // and wfi.
    intr_on();
    intr_off();

    പൂർണ്ണസംഖ്യ found = 0;
    വേണ്ടി (p = proc; p < &proc[NPROC]; p++) {
      acquire(&p->lock);
      എങ്കിൽ (p->state == RUNNABLE) {
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
    എങ്കിൽ (found == 0) {
      // nothing to run; stop running on this core until an interrupt.
      asm അസ്ഥിര("wfi");
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
ശൂന്യം
sched(ശൂന്യം)
{
  പൂർണ്ണസംഖ്യ intena;
  ഘടന proc *p = myproc();

  എങ്കിൽ (!holding(&p->lock))
    panic("sched p->lock");
  എങ്കിൽ (mycpu()->noff != 1)
    panic("sched locks");
  എങ്കിൽ (p->state == RUNNING)
    panic("sched RUNNING");
  എങ്കിൽ (intr_get())
    panic("sched interruptible");

  intena = mycpu()->intena;
  swtch(&p->context, &mycpu()->context);
  mycpu()->intena = intena;
}

// Give up the CPU for one scheduling round.
ശൂന്യം
yield(ശൂന്യം)
{
  ഘടന proc *p = myproc();
  acquire(&p->lock);
  p->state = RUNNABLE;
  sched();
  release(&p->lock);
}

// A fork child's very first scheduling by scheduler()
// will swtch to forkret.
ശൂന്യം
forkret(ശൂന്യം)
{
  ബാഹ്യ അക്ഷരം userret[];
  സ്ഥിതം പൂർണ്ണസംഖ്യ first = 1;
  ഘടന proc *p = myproc();

  // Still holding p->lock from scheduler.
  release(&p->lock);

  എങ്കിൽ (first) {
    first = 0;

    // File system initialization must be run in the context of a
    // regular process (e.g., because it calls sleep), and thus cannot
    // be run from main().
    fsinit(ROOTDEV);

    // We can invoke kexec() now that file system is initialized.
    // Put the return value (argc) of kexec into a0.
    p->trapframe->a0 = kexec("/init", (അക്ഷരം *[]){"/init", 0});
    എങ്കിൽ (p->trapframe->a0 == -1) {
      panic("exec");
    }
  }

  // return to user space, mimicing usertrap()'s return.
  prepare_return();
  uint64 satp = MAKE_SATP(p->pagetable);
  uint64 trampoline_userret = TRAMPOLINE + (userret - trampoline);
  ((ശൂന്യം (*)(uint64))trampoline_userret)(satp);
}

// Register current process as waiting for wakeups on chan.
ശൂന്യം
sleep_prepare(ശൂന്യം *chan)
{
  ഘടന proc *p = myproc();

  acquire(&p->lock);
  എങ്കിൽ (chan == 0)
    panic("sleep_prepare: zero chan");
  p->chan = chan;
  release(&p->lock);
}

// Put the thread to sleep.  Assumes sleep_prepare() was called before.
// If the channel registered by sleep_prepare() has been woken up in
// the meantime, do not go to sleep, and instead return immediately.
ശൂന്യം
sleep(ശൂന്യം)
{
  ഘടന proc *p = myproc();

  acquire(&p->lock);
  എങ്കിൽ (p->chan != 0) {
    p->state = SLEEPING;
    sched();
  }
  release(&p->lock);
}

// Wake up all processes sleeping on channel chan.
ശൂന്യം
wakeup(ശൂന്യം *chan)
{
  ഘടന proc *p;

  വേണ്ടി (p = proc; p < &proc[NPROC]; p++) {
    acquire(&p->lock);
    എങ്കിൽ (p->chan == chan) {
      // If the process is waiting for wakeups on this channel,
      // signal that the wakeup happened by clearing p->chan.
      p->chan = 0;

      // If this waiting process has gotten so far as to actually
      // go to sleep, also set it back to RUNNING.
      എങ്കിൽ (p->state == SLEEPING) {
        p->state = RUNNABLE;
      }
    }
    release(&p->lock);
  }
}

// Kill the process with the given pid.
// The victim won't exit until it tries to return
// to user space (see usertrap() in trap.c).
പൂർണ്ണസംഖ്യ
kkill(പൂർണ്ണസംഖ്യ pid)
{
  ഘടന proc *p;

  എങ്കിൽ (pid == 0)
    തിരികെ -1;

  വേണ്ടി (p = proc; p < &proc[NPROC]; p++) {
    acquire(&p->lock);
    എങ്കിൽ (p->pid == pid) {
      p->killed = 1;
      എങ്കിൽ (p->state == SLEEPING) {
        // Wake process from sleep().
        p->state = RUNNABLE;
      }
      release(&p->lock);
      തിരികെ 0;
    }
    release(&p->lock);
  }
  തിരികെ -1;
}

ശൂന്യം
setkilled(ഘടന proc *p)
{
  acquire(&p->lock);
  p->killed = 1;
  release(&p->lock);
}

പൂർണ്ണസംഖ്യ
killed(ഘടന proc *p)
{
  പൂർണ്ണസംഖ്യ k;

  acquire(&p->lock);
  k = p->killed;
  release(&p->lock);
  തിരികെ k;
}

// Copy to either a user address, or kernel address,
// depending on usr_dst.
// Returns 0 on success, -1 on error.
പൂർണ്ണസംഖ്യ
either_copyout(പൂർണ്ണസംഖ്യ user_dst, uint64 dst, ശൂന്യം *src, uint64 len)
{
  ഘടന proc *p = myproc();
  എങ്കിൽ (user_dst) {
    തിരികെ copyout(p->pagetable, p->sz, dst, src, len);
  } അല്ലെങ്കിൽ {
    memmove((അക്ഷരം *)dst, src, len);
    തിരികെ 0;
  }
}

// Copy from either a user address, or kernel address,
// depending on usr_src.
// Returns 0 on success, -1 on error.
പൂർണ്ണസംഖ്യ
either_copyin(ശൂന്യം *dst, പൂർണ്ണസംഖ്യ user_src, uint64 src, uint64 len)
{
  ഘടന proc *p = myproc();
  എങ്കിൽ (user_src) {
    തിരികെ copyin(p->pagetable, p->sz, dst, src, len);
  } അല്ലെങ്കിൽ {
    memmove(dst, (അക്ഷരം *)src, len);
    തിരികെ 0;
  }
}

// Print a process listing to console.  For debugging.
// Runs when user types ^P on console.
// No lock to avoid wedging a stuck machine further.
ശൂന്യം
procdump(ശൂന്യം)
{
  സ്ഥിതം അക്ഷരം *states[] = {
    // clang-format off
    [UNUSED]    = "unused",
    [USED]      = "used",
    [SLEEPING]  = "sleep ",
    [RUNNABLE]  = "runble",
    [RUNNING]   = "run   ",
    [ZOMBIE]    = "zombie"
    // clang-format on
  };
  ഘടന proc *p;
  അക്ഷരം *state;

  printk("\n");
  വേണ്ടി (p = proc; p < &proc[NPROC]; p++) {
    എങ്കിൽ (p->state == UNUSED)
      തുടരുക;
    എങ്കിൽ (p->state >= 0 && p->state < NELEM(states) && states[p->state])
      state = states[p->state];
    അല്ലെങ്കിൽ
      state = "???";
    printk("%d %s %s", p->pid, state, p->name);
    printk("\n");
  }
}
