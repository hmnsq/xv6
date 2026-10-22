#અન્તર્ભાવ "types.h"
#અન્તર્ભાવ "param.h"
#અન્તર્ભાવ "memlayout.h"
#અન્તર્ભાવ "riscv.h"
#અન્તર્ભાવ "spinlock.h"
#અન્તર્ભાવ "proc.h"
#અન્તર્ભાવ "defs.h"

સંરચના cpu cpus[NCPU];

સંરચના proc proc[NPROC];

સંરચના proc *initproc;

પૂર્ણાંક nextpid = 1;
સંરચના spinlock pid_lock;

બાહ્ય શૂન્ય forkret(શૂન્ય);
સ્થિર શૂન્ય freeproc(સંરચના proc *p);

બાહ્ય અક્ષર trampoline[]; // trampoline.S

// helps ensure that wakeups of wait()ing
// parents are not lost. helps obey the
// memory model when using p->parent.
// must be acquired before any p->lock.
સંરચના spinlock wait_lock;

// Allocate a page for each process's kernel stack.
// Map it high in memory, followed by an invalid
// guard page.
શૂન્ય
proc_mapstacks(pagetable_t kpgtbl)
{
  સંરચના proc *p;

  માટે (p = proc; p < &proc[NPROC]; p++) {
    અક્ષર *pa = kalloc();
    જો (pa == 0)
      panic("kalloc");
    uint64 va = KSTACK((પૂર્ણાંક)(p - proc));
    kvmmap(kpgtbl, va, (uint64)pa, PGSIZE, PTE_R | PTE_W);
  }
}

// initialize the proc table.
શૂન્ય
procinit(શૂન્ય)
{
  સંરચના proc *p;

  initlock(&pid_lock, "nextpid");
  initlock(&wait_lock, "wait_lock");
  માટે (p = proc; p < &proc[NPROC]; p++) {
    initlock(&p->lock, "proc");
    p->state = UNUSED;
    p->kstack = KSTACK((પૂર્ણાંક)(p - proc));
  }
}

// Must be called with interrupts disabled,
// to prevent race with process being moved
// to a different CPU.
પૂર્ણાંક
cpuid()
{
  પૂર્ણાંક id = r_tp();
  પાછા id;
}

// Return this CPU's cpu struct.
// Interrupts must be disabled.
સંરચના cpu *
mycpu(શૂન્ય)
{
  પૂર્ણાંક id = cpuid();
  સંરચના cpu *c = &cpus[id];
  પાછા c;
}

// Return the current struct proc *, or zero if none.
સંરચના proc *
myproc(શૂન્ય)
{
  ધકેલો_off();
  સંરચના cpu *c = mycpu();
  સંરચના proc *p = c->proc;
  કાઢો_off();
  પાછા p;
}

પૂર્ણાંક
allocpid()
{
  પૂર્ણાંક pid;

  acquire(&pid_lock);
  pid = nextpid;
  nextpid = nextpid + 1;
  release(&pid_lock);

  પાછા pid;
}

// Look in the process table for an UNUSED proc.
// If found, initialize state required to run in the kernel,
// and return with p->lock held.
// If there are no free procs, or a memory allocation fails, return 0.
સ્થિર સંરચના proc *
allocproc(શૂન્ય)
{
  સંરચના proc *p;

  માટે (p = proc; p < &proc[NPROC]; p++) {
    acquire(&p->lock);
    જો (p->state == UNUSED) {
      જાઓ found;
    } નહીંતર {
      release(&p->lock);
    }
  }
  પાછા 0;

found:
  p->pid = allocpid();
  p->state = USED;

  // Allocate a trapframe page.
  જો ((p->trapframe = (સંરચના trapframe *)kalloc()) == 0) {
    freeproc(p);
    release(&p->lock);
    પાછા 0;
  }

  // An empty user page table.
  p->pagetable = proc_pagetable(p);
  જો (p->pagetable == 0) {
    freeproc(p);
    release(&p->lock);
    પાછા 0;
  }

  // Set up new context to start executing at forkret,
  // which returns to user space.
  memset(&p->context, 0, કદ(p->context));
  p->context.ra = (uint64)forkret;
  p->context.sp = p->kstack + PGSIZE;

  પાછા p;
}

// free a proc structure and the data hanging from it,
// including user pages.
// p->lock must be held.
સ્થિર શૂન્ય
freeproc(સંરચના proc *p)
{
  જો (p->trapframe)
    kfree((શૂન્ય *)p->trapframe);
  p->trapframe = 0;
  જો (p->pagetable)
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
proc_pagetable(સંરચના proc *p)
{
  pagetable_t pagetable;

  // An empty page table.
  pagetable = uvmcreate();
  જો (pagetable == 0)
    પાછા 0;

  // map the trampoline code (for system call return)
  // at the highest user virtual address.
  // only the supervisor uses it, on the way
  // to/from user space, so not PTE_U.
  જો (mappages(pagetable, TRAMPOLINE, PGSIZE, (uint64)trampoline,
               PTE_R | PTE_X) < 0) {
    uvmfree(pagetable, 0);
    પાછા 0;
  }

  // map the trapframe page just below the trampoline page, for
  // trampoline.S.
  જો (mappages(pagetable, TRAPFRAME, PGSIZE, (uint64)(p->trapframe),
               PTE_R | PTE_W) < 0) {
    uvmunmap(pagetable, TRAMPOLINE, 1, 0);
    uvmfree(pagetable, 0);
    પાછા 0;
  }

  પાછા pagetable;
}

// Free a process's page table, and free the
// physical memory it refers to.
શૂન્ય
proc_freepagetable(pagetable_t pagetable, uint64 sz)
{
  uvmunmap(pagetable, TRAMPOLINE, 1, 0);
  uvmunmap(pagetable, TRAPFRAME, 1, 0);
  uvmfree(pagetable, sz);
}

// Set up first user process.
શૂન્ય
userinit(શૂન્ય)
{
  સંરચના proc *p;

  p = allocproc();
  initproc = p;

  p->cwd = namei("/");

  p->state = RUNNABLE;

  release(&p->lock);
}

// Grow or shrink user memory by n bytes.
// Return 0 on success, -1 on failure.
પૂર્ણાંક
growproc(પૂર્ણાંક n)
{
  uint64 sz;
  સંરચના proc *p = myproc();

  sz = p->sz;
  જો (n > 0) {
    જો (sz + n > TRAPFRAME) {
      પાછા -1;
    }
    જો ((sz = uvmalloc(p->pagetable, sz, sz + n, PTE_W)) == 0) {
      પાછા -1;
    }
  } નહીંતર જો (n < 0) {
    sz = uvmdealloc(p->pagetable, sz, sz + n);
  }
  p->sz = sz;
  પાછા 0;
}

// Create a new process, copying the parent.
// Sets up child kernel stack to return as if from fork() system call.
પૂર્ણાંક
kfork(શૂન્ય)
{
  પૂર્ણાંક i, pid;
  સંરચના proc *np;
  સંરચના proc *p = myproc();

  // Allocate process.
  જો ((np = allocproc()) == 0) {
    પાછા -1;
  }

  // Copy user memory from parent to child.
  જો (uvmcopy(p->pagetable, np->pagetable, p->sz) < 0) {
    freeproc(np);
    release(&np->lock);
    પાછા -1;
  }
  np->sz = p->sz;

  // copy saved user registers.
  *(np->trapframe) = *(p->trapframe);

  // Cause fork to return 0 in the child.
  np->trapframe->a0 = 0;

  // increment reference counts on open file descriptors.
  માટે (i = 0; i < NOFILE; i++)
    જો (p->ofile[i])
      np->ofile[i] = filedup(p->ofile[i]);
  np->cwd = idup(p->cwd);

  safestrcpy(np->name, p->name, કદ(p->name));

  pid = np->pid;

  release(&np->lock);

  acquire(&wait_lock);
  np->parent = p;
  release(&wait_lock);

  acquire(&np->lock);
  np->state = RUNNABLE;
  release(&np->lock);

  પાછા pid;
}

// Pass p's abandoned children to init.
// Caller must hold wait_lock.
શૂન્ય
reparent(સંરચના proc *p)
{
  સંરચના proc *pp;

  માટે (pp = proc; pp < &proc[NPROC]; pp++) {
    જો (pp->parent == p) {
      pp->parent = initproc;
      wakeup(initproc);
    }
  }
}

// Exit the current process.  Does not return.
// An exited process remains in the zombie state
// until its parent calls wait().
શૂન્ય
kexit(પૂર્ણાંક status)
{
  સંરચના proc *p = myproc();

  જો (p == initproc)
    panic("init exiting");

  // Close all open files.
  માટે (પૂર્ણાંક fd = 0; fd < NOFILE; fd++) {
    જો (p->ofile[fd]) {
      સંરચના file *f = p->ofile[fd];
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
પૂર્ણાંક
kwait(uint64 addr)
{
  સંરચના proc *pp;
  પૂર્ણાંક havekids, pid;
  સંરચના proc *p = myproc();

  acquire(&wait_lock);

  માટે (;;) {
    // Scan through table looking for exited children.
    havekids = 0;
    માટે (pp = proc; pp < &proc[NPROC]; pp++) {
      જો (pp->parent == p) {
        // make sure the child isn't still in exit() or swtch().
        acquire(&pp->lock);

        havekids = 1;
        જો (pp->state == ZOMBIE) {
          // Found one.
          pid = pp->pid;
          જો (addr != 0 &&
              copyout(p->pagetable, p->sz, addr, (અક્ષર *)&pp->xstate,
                      કદ(pp->xstate)) < 0) {
            release(&pp->lock);
            release(&wait_lock);
            પાછા -1;
          }
          pp->parent = 0;
          freeproc(pp);
          release(&pp->lock);
          release(&wait_lock);
          પાછા pid;
        }
        release(&pp->lock);
      }
    }

    // No point waiting if we don't have any children.
    જો (!havekids || killed(p)) {
      release(&wait_lock);
      પાછા -1;
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
શૂન્ય
scheduler(શૂન્ય)
{
  સંરચના proc *p;
  સંરચના cpu *c = mycpu();

  c->proc = 0;
  માટે (;;) {
    // The most recent process to run may have had interrupts
    // turned off; enable them to avoid a deadlock if all
    // processes are waiting. Then turn them back off
    // to avoid a possible race between an interrupt
    // and wfi.
    intr_on();
    intr_off();

    પૂર્ણાંક found = 0;
    માટે (p = proc; p < &proc[NPROC]; p++) {
      acquire(&p->lock);
      જો (p->state == RUNNABLE) {
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
    જો (found == 0) {
      // nothing to run; stop running on this core until an interrupt.
      asm અસ્થિર("wfi");
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
શૂન્ય
sched(શૂન્ય)
{
  પૂર્ણાંક intena;
  સંરચના proc *p = myproc();

  જો (!holding(&p->lock))
    panic("sched p->lock");
  જો (mycpu()->noff != 1)
    panic("sched locks");
  જો (p->state == RUNNING)
    panic("sched RUNNING");
  જો (intr_get())
    panic("sched interruptible");

  intena = mycpu()->intena;
  swtch(&p->context, &mycpu()->context);
  mycpu()->intena = intena;
}

// Give up the CPU for one scheduling round.
શૂન્ય
yield(શૂન્ય)
{
  સંરચના proc *p = myproc();
  acquire(&p->lock);
  p->state = RUNNABLE;
  sched();
  release(&p->lock);
}

// A fork child's very first scheduling by scheduler()
// will swtch to forkret.
શૂન્ય
forkret(શૂન્ય)
{
  બાહ્ય અક્ષર userret[];
  સ્થિર પૂર્ણાંક first = 1;
  સંરચના proc *p = myproc();

  // Still holding p->lock from scheduler.
  release(&p->lock);

  જો (first) {
    first = 0;

    // File system initialization must be run in the context of a
    // regular process (e.g., because it calls sleep), and thus cannot
    // be run from main().
    fsinit(ROOTDEV);

    // We can invoke kexec() now that file system is initialized.
    // Put the return value (argc) of kexec into a0.
    p->trapframe->a0 = kexec("/init", (અક્ષર *[]){"/init", 0});
    જો (p->trapframe->a0 == -1) {
      panic("exec");
    }
  }

  // return to user space, mimicing usertrap()'s return.
  prepare_return();
  uint64 satp = MAKE_SATP(p->pagetable);
  uint64 trampoline_userret = TRAMPOLINE + (userret - trampoline);
  ((શૂન્ય (*)(uint64))trampoline_userret)(satp);
}

// Register current process as waiting for wakeups on chan.
શૂન્ય
sleep_prepare(શૂન્ય *chan)
{
  સંરચના proc *p = myproc();

  acquire(&p->lock);
  જો (chan == 0)
    panic("sleep_prepare: zero chan");
  p->chan = chan;
  release(&p->lock);
}

// Put the thread to sleep.  Assumes sleep_prepare() was called before.
// If the channel registered by sleep_prepare() has been woken up in
// the meantime, do not go to sleep, and instead return immediately.
શૂન્ય
sleep(શૂન્ય)
{
  સંરચના proc *p = myproc();

  acquire(&p->lock);
  જો (p->chan != 0) {
    p->state = SLEEPING;
    sched();
  }
  release(&p->lock);
}

// Wake up all processes sleeping on channel chan.
શૂન્ય
wakeup(શૂન્ય *chan)
{
  સંરચના proc *p;

  માટે (p = proc; p < &proc[NPROC]; p++) {
    acquire(&p->lock);
    જો (p->chan == chan) {
      // If the process is waiting for wakeups on this channel,
      // signal that the wakeup happened by clearing p->chan.
      p->chan = 0;

      // If this waiting process has gotten so far as to actually
      // go to sleep, also set it back to RUNNING.
      જો (p->state == SLEEPING) {
        p->state = RUNNABLE;
      }
    }
    release(&p->lock);
  }
}

// Kill the process with the given pid.
// The victim won't exit until it tries to return
// to user space (see usertrap() in trap.c).
પૂર્ણાંક
kkill(પૂર્ણાંક pid)
{
  સંરચના proc *p;

  જો (pid == 0)
    પાછા -1;

  માટે (p = proc; p < &proc[NPROC]; p++) {
    acquire(&p->lock);
    જો (p->pid == pid) {
      p->killed = 1;
      જો (p->state == SLEEPING) {
        // Wake process from sleep().
        p->state = RUNNABLE;
      }
      release(&p->lock);
      પાછા 0;
    }
    release(&p->lock);
  }
  પાછા -1;
}

શૂન્ય
setkilled(સંરચના proc *p)
{
  acquire(&p->lock);
  p->killed = 1;
  release(&p->lock);
}

પૂર્ણાંક
killed(સંરચના proc *p)
{
  પૂર્ણાંક k;

  acquire(&p->lock);
  k = p->killed;
  release(&p->lock);
  પાછા k;
}

// Copy to either a user address, or kernel address,
// depending on usr_dst.
// Returns 0 on success, -1 on error.
પૂર્ણાંક
either_copyout(પૂર્ણાંક user_dst, uint64 dst, શૂન્ય *src, uint64 len)
{
  સંરચના proc *p = myproc();
  જો (user_dst) {
    પાછા copyout(p->pagetable, p->sz, dst, src, len);
  } નહીંતર {
    memmove((અક્ષર *)dst, src, len);
    પાછા 0;
  }
}

// Copy from either a user address, or kernel address,
// depending on usr_src.
// Returns 0 on success, -1 on error.
પૂર્ણાંક
either_copyin(શૂન્ય *dst, પૂર્ણાંક user_src, uint64 src, uint64 len)
{
  સંરચના proc *p = myproc();
  જો (user_src) {
    પાછા copyin(p->pagetable, p->sz, dst, src, len);
  } નહીંતર {
    memmove(dst, (અક્ષર *)src, len);
    પાછા 0;
  }
}

// Print a process listing to console.  For debugging.
// Runs when user types ^P on console.
// No lock to avoid wedging a stuck machine further.
શૂન્ય
procdump(શૂન્ય)
{
  સ્થિર અક્ષર *states[] = {
    // clang-format off
    [UNUSED]    = "unused",
    [USED]      = "used",
    [SLEEPING]  = "sleep ",
    [RUNNABLE]  = "runble",
    [RUNNING]   = "run   ",
    [ZOMBIE]    = "zombie"
    // clang-format on
  };
  સંરચના proc *p;
  અક્ષર *state;

  printk("\n");
  માટે (p = proc; p < &proc[NPROC]; p++) {
    જો (p->state == UNUSED)
      ચાલુ;
    જો (p->state >= 0 && p->state < NELEM(states) && states[p->state])
      state = states[p->state];
    નહીંતર
      state = "???";
    printk("%d %s %s", p->pid, state, p->name);
    printk("\n");
  }
}
