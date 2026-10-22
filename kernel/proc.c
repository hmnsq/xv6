#ಅನ್ತರ್ಭಾವ "types.h"
#ಅನ್ತರ್ಭಾವ "param.h"
#ಅನ್ತರ್ಭಾವ "memlayout.h"
#ಅನ್ತರ್ಭಾವ "riscv.h"
#ಅನ್ತರ್ಭಾವ "spinlock.h"
#ಅನ್ತರ್ಭಾವ "proc.h"
#ಅನ್ತರ್ಭಾವ "defs.h"

ರಚನೆ cpu cpus[NCPU];

ರಚನೆ proc proc[NPROC];

ರಚನೆ proc *initproc;

ಪೂರ್ಣಾಂಕ nextpid = 1;
ರಚನೆ spinlock pid_ಬೀಗ;

ಬಾಹ್ಯ ಶೂನ್ಯ forkret(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ freeproc(ರಚನೆ proc *p);

ಬಾಹ್ಯ ಅಕ್ಷರ trampoline[]; // trampoline.S

// helps ensure that wakeups of wait()ing
// parents are not lost. helps obey the
// memory model when using p->parent.
// must be acquired before any p->lock.
ರಚನೆ spinlock wait_ಬೀಗ;

// Allocate a page for each process's kernel stack.
// Map it high in memory, followed by an invalid
// guard page.
ಶೂನ್ಯ
proc_mapstacks(pagetable_t kpgtbl)
{
  ರಚನೆ proc *p;

  ಗಾಗಿ (p = proc; p < &proc[NPROC]; p++) {
    ಅಕ್ಷರ *pa = kalloc();
    ಆದರೆ (pa == 0)
      panic("kalloc");
    uint64 va = KSTACK((ಪೂರ್ಣಾಂಕ)(p - proc));
    kvmmap(kpgtbl, va, (uint64)pa, PGSIZE, PTE_R | PTE_W);
  }
}

// initialize the proc table.
ಶೂನ್ಯ
procinit(ಶೂನ್ಯ)
{
  ರಚನೆ proc *p;

  initlock(&pid_ಬೀಗ, "nextpid");
  initlock(&wait_ಬೀಗ, "wait_lock");
  ಗಾಗಿ (p = proc; p < &proc[NPROC]; p++) {
    initlock(&p->ಬೀಗ, "proc");
    p->state = ಬಳಸದ;
    p->kstack = KSTACK((ಪೂರ್ಣಾಂಕ)(p - proc));
  }
}

// Must be called with interrupts disabled,
// to prevent race with process being moved
// to a different CPU.
ಪೂರ್ಣಾಂಕ
cpuid()
{
  ಪೂರ್ಣಾಂಕ id = r_tp();
  ಹಿಂತಿರುಗಿ id;
}

// Return this CPU's cpu struct.
// Interrupts must be disabled.
ರಚನೆ cpu *
mycpu(ಶೂನ್ಯ)
{
  ಪೂರ್ಣಾಂಕ id = cpuid();
  ರಚನೆ cpu *c = &cpus[id];
  ಹಿಂತಿರುಗಿ c;
}

// Return the current struct proc *, or zero if none.
ರಚನೆ proc *
myproc(ಶೂನ್ಯ)
{
  ತಳ್ಳು_off();
  ರಚನೆ cpu *c = mycpu();
  ರಚನೆ proc *p = c->proc;
  ತೆಗೆ_off();
  ಹಿಂತಿರುಗಿ p;
}

ಪೂರ್ಣಾಂಕ
allocpid()
{
  ಪೂರ್ಣಾಂಕ pid;

  acquire(&pid_ಬೀಗ);
  pid = nextpid;
  nextpid = nextpid + 1;
  release(&pid_ಬೀಗ);

  ಹಿಂತಿರುಗಿ pid;
}

// Look in the process table for an UNUSED proc.
// If found, initialize state required to run in the kernel,
// and return with p->lock held.
// If there are no free procs, or a memory allocation fails, return 0.
ಸ್ಥಿರ ರಚನೆ proc *
allocproc(ಶೂನ್ಯ)
{
  ರಚನೆ proc *p;

  ಗಾಗಿ (p = proc; p < &proc[NPROC]; p++) {
    acquire(&p->ಬೀಗ);
    ಆದರೆ (p->state == ಬಳಸದ) {
      ಹೋಗು ಸಿಕ್ಕಿತು;
    } ಇಲ್ಲದಿದ್ದರೆ {
      release(&p->ಬೀಗ);
    }
  }
  ಹಿಂತಿರುಗಿ 0;

ಸಿಕ್ಕಿತು:
  p->pid = allocpid();
  p->state = ಬಳಸಿದ;

  // Allocate a trapframe page.
  ಆದರೆ ((p->trapframe = (ರಚನೆ trapframe *)kalloc()) == 0) {
    freeproc(p);
    release(&p->ಬೀಗ);
    ಹಿಂತಿರುಗಿ 0;
  }

  // An empty user page table.
  p->pagetable = proc_pagetable(p);
  ಆದರೆ (p->pagetable == 0) {
    freeproc(p);
    release(&p->ಬೀಗ);
    ಹಿಂತಿರುಗಿ 0;
  }

  // Set up new context to start executing at forkret,
  // which returns to user space.
  memset(&p->context, 0, ಗಾತ್ರ(p->context));
  p->context.ra = (uint64)forkret;
  p->context.sp = p->kstack + PGSIZE;

  ಹಿಂತಿರುಗಿ p;
}

// free a proc structure and the data hanging from it,
// including user pages.
// p->lock must be held.
ಸ್ಥಿರ ಶೂನ್ಯ
freeproc(ರಚನೆ proc *p)
{
  ಆದರೆ (p->trapframe)
    kfree((ಶೂನ್ಯ *)p->trapframe);
  p->trapframe = 0;
  ಆದರೆ (p->pagetable)
    proc_freepagetable(p->pagetable, p->sz);
  p->pagetable = 0;
  p->sz = 0;
  p->pid = 0;
  p->name[0] = 0;
  p->chan = 0;
  p->killed = 0;
  p->xstate = 0;
  p->state = ಬಳಸದ;
}

// Create a user page table for a given process, with no user memory,
// but with trampoline and trapframe pages.
pagetable_t
proc_pagetable(ರಚನೆ proc *p)
{
  pagetable_t pagetable;

  // An empty page table.
  pagetable = uvmcreate();
  ಆದರೆ (pagetable == 0)
    ಹಿಂತಿರುಗಿ 0;

  // map the trampoline code (for system call return)
  // at the highest user virtual address.
  // only the supervisor uses it, on the way
  // to/from user space, so not PTE_U.
  ಆದರೆ (mappages(pagetable, TRAMPOLINE, PGSIZE, (uint64)trampoline,
               PTE_R | PTE_X) < 0) {
    uvmfree(pagetable, 0);
    ಹಿಂತಿರುಗಿ 0;
  }

  // map the trapframe page just below the trampoline page, for
  // trampoline.S.
  ಆದರೆ (mappages(pagetable, TRAPFRAME, PGSIZE, (uint64)(p->trapframe),
               PTE_R | PTE_W) < 0) {
    uvmunmap(pagetable, TRAMPOLINE, 1, 0);
    uvmfree(pagetable, 0);
    ಹಿಂತಿರುಗಿ 0;
  }

  ಹಿಂತಿರುಗಿ pagetable;
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
  ರಚನೆ proc *p;

  p = allocproc();
  initproc = p;

  p->cwd = namei("/");

  p->state = RUNNABLE;

  release(&p->ಬೀಗ);
}

// Grow or shrink user memory by n bytes.
// Return 0 on success, -1 on failure.
ಪೂರ್ಣಾಂಕ
growproc(ಪೂರ್ಣಾಂಕ n)
{
  uint64 sz;
  ರಚನೆ proc *p = myproc();

  sz = p->sz;
  ಆದರೆ (n > 0) {
    ಆದರೆ (sz + n > TRAPFRAME) {
      ಹಿಂತಿರುಗಿ -1;
    }
    ಆದರೆ ((sz = uvmalloc(p->pagetable, sz, sz + n, PTE_W)) == 0) {
      ಹಿಂತಿರುಗಿ -1;
    }
  } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (n < 0) {
    sz = uvmdealloc(p->pagetable, sz, sz + n);
  }
  p->sz = sz;
  ಹಿಂತಿರುಗಿ 0;
}

// Create a new process, copying the parent.
// Sets up child kernel stack to return as if from fork() system call.
ಪೂರ್ಣಾಂಕ
kfork(ಶೂನ್ಯ)
{
  ಪೂರ್ಣಾಂಕ i, pid;
  ರಚನೆ proc *np;
  ರಚನೆ proc *p = myproc();

  // Allocate process.
  ಆದರೆ ((np = allocproc()) == 0) {
    ಹಿಂತಿರುಗಿ -1;
  }

  // Copy user memory from parent to child.
  ಆದರೆ (uvmcopy(p->pagetable, np->pagetable, p->sz) < 0) {
    freeproc(np);
    release(&np->ಬೀಗ);
    ಹಿಂತಿರುಗಿ -1;
  }
  np->sz = p->sz;

  // copy saved user registers.
  *(np->trapframe) = *(p->trapframe);

  // Cause fork to return 0 in the child.
  np->trapframe->a0 = 0;

  // increment reference counts on open file descriptors.
  ಗಾಗಿ (i = 0; i < NOFILE; i++)
    ಆದರೆ (p->ofile[i])
      np->ofile[i] = filedup(p->ofile[i]);
  np->cwd = idup(p->cwd);

  safestrcpy(np->name, p->name, ಗಾತ್ರ(p->name));

  pid = np->pid;

  release(&np->ಬೀಗ);

  acquire(&wait_ಬೀಗ);
  np->ಪೋಷಕ = p;
  release(&wait_ಬೀಗ);

  acquire(&np->ಬೀಗ);
  np->state = RUNNABLE;
  release(&np->ಬೀಗ);

  ಹಿಂತಿರುಗಿ pid;
}

// Pass p's abandoned children to init.
// Caller must hold wait_lock.
ಶೂನ್ಯ
reparent(ರಚನೆ proc *p)
{
  ರಚನೆ proc *pp;

  ಗಾಗಿ (pp = proc; pp < &proc[NPROC]; pp++) {
    ಆದರೆ (pp->ಪೋಷಕ == p) {
      pp->ಪೋಷಕ = initproc;
      wakeup(initproc);
    }
  }
}

// Exit the current process.  Does not return.
// An exited process remains in the zombie state
// until its parent calls wait().
ಶೂನ್ಯ
kexit(ಪೂರ್ಣಾಂಕ ಸ್ಥಿತಿ_2)
{
  ರಚನೆ proc *p = myproc();

  ಆದರೆ (p == initproc)
    panic("init exiting");

  // Close all open files.
  ಗಾಗಿ (ಪೂರ್ಣಾಂಕ fd = 0; fd < NOFILE; fd++) {
    ಆದರೆ (p->ofile[fd]) {
      ರಚನೆ file *f = p->ofile[fd];
      fileclose(f);
      p->ofile[fd] = 0;
    }
  }

  begin_op();
  iput(p->cwd);
  ಕೊನೆ_op();
  p->cwd = 0;

  acquire(&wait_ಬೀಗ);

  // Give any children to init.
  reparent(p);

  // Parent might be sleeping in wait().
  wakeup(p->ಪೋಷಕ);

  acquire(&p->ಬೀಗ);

  p->xstate = ಸ್ಥಿತಿ_2;
  p->state = ZOMBIE;

  release(&wait_ಬೀಗ);

  // Jump into the scheduler, never to return.
  sched();
  panic("zombie exit");
}

// Wait for a child process to exit and return its pid.
// Return -1 if this process has no children.
ಪೂರ್ಣಾಂಕ
kwait(uint64 addr)
{
  ರಚನೆ proc *pp;
  ಪೂರ್ಣಾಂಕ havekids, pid;
  ರಚನೆ proc *p = myproc();

  acquire(&wait_ಬೀಗ);

  ಗಾಗಿ (;;) {
    // Scan through table looking for exited children.
    havekids = 0;
    ಗಾಗಿ (pp = proc; pp < &proc[NPROC]; pp++) {
      ಆದರೆ (pp->ಪೋಷಕ == p) {
        // make sure the child isn't still in exit() or swtch().
        acquire(&pp->ಬೀಗ);

        havekids = 1;
        ಆದರೆ (pp->state == ZOMBIE) {
          // Found one.
          pid = pp->pid;
          ಆದರೆ (addr != 0 &&
              copyout(p->pagetable, p->sz, addr, (ಅಕ್ಷರ *)&pp->xstate,
                      ಗಾತ್ರ(pp->xstate)) < 0) {
            release(&pp->ಬೀಗ);
            release(&wait_ಬೀಗ);
            ಹಿಂತಿರುಗಿ -1;
          }
          pp->ಪೋಷಕ = 0;
          freeproc(pp);
          release(&pp->ಬೀಗ);
          release(&wait_ಬೀಗ);
          ಹಿಂತಿರುಗಿ pid;
        }
        release(&pp->ಬೀಗ);
      }
    }

    // No point waiting if we don't have any children.
    ಆದರೆ (!havekids || killed(p)) {
      release(&wait_ಬೀಗ);
      ಹಿಂತಿರುಗಿ -1;
    }

    // Wait for a child to exit.
    sleep_prepare(p); //DOC: wait-sleep
    release(&wait_ಬೀಗ);
    sleep();
    acquire(&wait_ಬೀಗ);
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
  ರಚನೆ proc *p;
  ರಚನೆ cpu *c = mycpu();

  c->proc = 0;
  ಗಾಗಿ (;;) {
    // The most recent process to run may have had interrupts
    // turned off; enable them to avoid a deadlock if all
    // processes are waiting. Then turn them back off
    // to avoid a possible race between an interrupt
    // and wfi.
    intr_on();
    intr_off();

    ಪೂರ್ಣಾಂಕ ಸಿಕ್ಕಿತು = 0;
    ಗಾಗಿ (p = proc; p < &proc[NPROC]; p++) {
      acquire(&p->ಬೀಗ);
      ಆದರೆ (p->state == RUNNABLE) {
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
        ಸಿಕ್ಕಿತು = 1;
      }
      release(&p->ಬೀಗ);
    }
    ಆದರೆ (ಸಿಕ್ಕಿತು == 0) {
      // nothing to run; stop running on this core until an interrupt.
      asm ಅಸ್ಥಿರ("wfi");
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
  ಪೂರ್ಣಾಂಕ intena;
  ರಚನೆ proc *p = myproc();

  ಆದರೆ (!holding(&p->ಬೀಗ))
    panic("sched p->lock");
  ಆದರೆ (mycpu()->noff != 1)
    panic("sched locks");
  ಆದರೆ (p->state == RUNNING)
    panic("sched RUNNING");
  ಆದರೆ (intr_get())
    panic("sched interruptible");

  intena = mycpu()->intena;
  swtch(&p->context, &mycpu()->context);
  mycpu()->intena = intena;
}

// Give up the CPU for one scheduling round.
ಶೂನ್ಯ
yield(ಶೂನ್ಯ)
{
  ರಚನೆ proc *p = myproc();
  acquire(&p->ಬೀಗ);
  p->state = RUNNABLE;
  sched();
  release(&p->ಬೀಗ);
}

// A fork child's very first scheduling by scheduler()
// will swtch to forkret.
ಶೂನ್ಯ
forkret(ಶೂನ್ಯ)
{
  ಬಾಹ್ಯ ಅಕ್ಷರ userret[];
  ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಮೊದಲ = 1;
  ರಚನೆ proc *p = myproc();

  // Still holding p->lock from scheduler.
  release(&p->ಬೀಗ);

  ಆದರೆ (ಮೊದಲ) {
    ಮೊದಲ = 0;

    // File system initialization must be run in the context of a
    // regular process (e.g., because it calls sleep), and thus cannot
    // be run from main().
    fsinit(ROOTDEV);

    // We can invoke kexec() now that file system is initialized.
    // Put the return value (argc) of kexec into a0.
    p->trapframe->a0 = kexec("/init", (ಅಕ್ಷರ *[]){"/init", 0});
    ಆದರೆ (p->trapframe->a0 == -1) {
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
  ರಚನೆ proc *p = myproc();

  acquire(&p->ಬೀಗ);
  ಆದರೆ (chan == 0)
    panic("sleep_prepare: zero chan");
  p->chan = chan;
  release(&p->ಬೀಗ);
}

// Put the thread to sleep.  Assumes sleep_prepare() was called before.
// If the channel registered by sleep_prepare() has been woken up in
// the meantime, do not go to sleep, and instead return immediately.
ಶೂನ್ಯ
sleep(ಶೂನ್ಯ)
{
  ರಚನೆ proc *p = myproc();

  acquire(&p->ಬೀಗ);
  ಆದರೆ (p->chan != 0) {
    p->state = SLEEPING;
    sched();
  }
  release(&p->ಬೀಗ);
}

// Wake up all processes sleeping on channel chan.
ಶೂನ್ಯ
wakeup(ಶೂನ್ಯ *chan)
{
  ರಚನೆ proc *p;

  ಗಾಗಿ (p = proc; p < &proc[NPROC]; p++) {
    acquire(&p->ಬೀಗ);
    ಆದರೆ (p->chan == chan) {
      // If the process is waiting for wakeups on this channel,
      // signal that the wakeup happened by clearing p->chan.
      p->chan = 0;

      // If this waiting process has gotten so far as to actually
      // go to sleep, also set it back to RUNNING.
      ಆದರೆ (p->state == SLEEPING) {
        p->state = RUNNABLE;
      }
    }
    release(&p->ಬೀಗ);
  }
}

// Kill the process with the given pid.
// The victim won't exit until it tries to return
// to user space (see usertrap() in trap.c).
ಪೂರ್ಣಾಂಕ
kkill(ಪೂರ್ಣಾಂಕ pid)
{
  ರಚನೆ proc *p;

  ಆದರೆ (pid == 0)
    ಹಿಂತಿರುಗಿ -1;

  ಗಾಗಿ (p = proc; p < &proc[NPROC]; p++) {
    acquire(&p->ಬೀಗ);
    ಆದರೆ (p->pid == pid) {
      p->killed = 1;
      ಆದರೆ (p->state == SLEEPING) {
        // Wake process from sleep().
        p->state = RUNNABLE;
      }
      release(&p->ಬೀಗ);
      ಹಿಂತಿರುಗಿ 0;
    }
    release(&p->ಬೀಗ);
  }
  ಹಿಂತಿರುಗಿ -1;
}

ಶೂನ್ಯ
setkilled(ರಚನೆ proc *p)
{
  acquire(&p->ಬೀಗ);
  p->killed = 1;
  release(&p->ಬೀಗ);
}

ಪೂರ್ಣಾಂಕ
killed(ರಚನೆ proc *p)
{
  ಪೂರ್ಣಾಂಕ k;

  acquire(&p->ಬೀಗ);
  k = p->killed;
  release(&p->ಬೀಗ);
  ಹಿಂತಿರುಗಿ k;
}

// Copy to either a user address, or kernel address,
// depending on usr_dst.
// Returns 0 on success, -1 on error.
ಪೂರ್ಣಾಂಕ
either_copyout(ಪೂರ್ಣಾಂಕ user_dst, uint64 dst, ಶೂನ್ಯ *src, uint64 len)
{
  ರಚನೆ proc *p = myproc();
  ಆದರೆ (user_dst) {
    ಹಿಂತಿರುಗಿ copyout(p->pagetable, p->sz, dst, src, len);
  } ಇಲ್ಲದಿದ್ದರೆ {
    memmove((ಅಕ್ಷರ *)dst, src, len);
    ಹಿಂತಿರುಗಿ 0;
  }
}

// Copy from either a user address, or kernel address,
// depending on usr_src.
// Returns 0 on success, -1 on error.
ಪೂರ್ಣಾಂಕ
either_copyin(ಶೂನ್ಯ *dst, ಪೂರ್ಣಾಂಕ user_src, uint64 src, uint64 len)
{
  ರಚನೆ proc *p = myproc();
  ಆದರೆ (user_src) {
    ಹಿಂತಿರುಗಿ copyin(p->pagetable, p->sz, dst, src, len);
  } ಇಲ್ಲದಿದ್ದರೆ {
    memmove(dst, (ಅಕ್ಷರ *)src, len);
    ಹಿಂತಿರುಗಿ 0;
  }
}

// Print a process listing to console.  For debugging.
// Runs when user types ^P on console.
// No lock to avoid wedging a stuck machine further.
ಶೂನ್ಯ
procdump(ಶೂನ್ಯ)
{
  ಸ್ಥಿರ ಅಕ್ಷರ *ಸ್ಥಿತಿಗಳು[] = {
    // clang-format off
    [ಬಳಸದ]    = "unused",
    [ಬಳಸಿದ]      = "used",
    [SLEEPING]  = "sleep ",
    [RUNNABLE]  = "runble",
    [RUNNING]   = "run   ",
    [ZOMBIE]    = "zombie"
    // clang-format on
  };
  ರಚನೆ proc *p;
  ಅಕ್ಷರ *state;

  printk("\n");
  ಗಾಗಿ (p = proc; p < &proc[NPROC]; p++) {
    ಆದರೆ (p->state == ಬಳಸದ)
      ಮುಂದುವರಿ;
    ಆದರೆ (p->state >= 0 && p->state < NELEM(ಸ್ಥಿತಿಗಳು) && ಸ್ಥಿತಿಗಳು[p->state])
      state = ಸ್ಥಿತಿಗಳು[p->state];
    ಇಲ್ಲದಿದ್ದರೆ
      state = "???";
    printk("%d %s %s", p->pid, state, p->name);
    printk("\n");
  }
}
