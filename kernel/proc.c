#অন্তর্ভাৱ "types.h"
#অন্তর্ভাৱ "param.h"
#অন্তর্ভাৱ "memlayout.h"
#অন্তর্ভাৱ "riscv.h"
#অন্তর্ভাৱ "spinlock.h"
#অন্তর্ভাৱ "proc.h"
#অন্তর্ভাৱ "defs.h"

সংরচনা cpu cpus[NCPU];

সংরচনা proc proc[NPROC];

সংরচনা proc *initproc;

পূর্ণাঙ্ক nextpid = 1;
সংরচনা spinlock pid_lock;

বাহ্য শূন্য forkret(শূন্য);
স্থির শূন্য freeproc(সংরচনা proc *p);

বাহ্য ৱর্ণ trampoline[]; // trampoline.S

// helps ensure that wakeups of wait()ing
// parents are not lost. helps obey the
// memory model when using p->parent.
// must be acquired before any p->lock.
সংরচনা spinlock wait_lock;

// Allocate a page for each process's kernel stack.
// Map it high in memory, followed by an invalid
// guard page.
শূন্য
proc_mapstacks(pagetable_t kpgtbl)
{
  সংরচনা proc *p;

  বাবে (p = proc; p < &proc[NPROC]; p++) {
    ৱর্ণ *pa = kalloc();
    যদি (pa == 0)
      panic("kalloc");
    uint64 va = KSTACK((পূর্ণাঙ্ক)(p - proc));
    kvmmap(kpgtbl, va, (uint64)pa, PGSIZE, PTE_R | PTE_W);
  }
}

// initialize the proc table.
শূন্য
procinit(শূন্য)
{
  সংরচনা proc *p;

  initlock(&pid_lock, "nextpid");
  initlock(&wait_lock, "wait_lock");
  বাবে (p = proc; p < &proc[NPROC]; p++) {
    initlock(&p->lock, "proc");
    p->state = UNUSED;
    p->kstack = KSTACK((পূর্ণাঙ্ক)(p - proc));
  }
}

// Must be called with interrupts disabled,
// to prevent race with process being moved
// to a different CPU.
পূর্ণাঙ্ক
cpuid()
{
  পূর্ণাঙ্ক id = r_tp();
  ঘূৰাই id;
}

// Return this CPU's cpu struct.
// Interrupts must be disabled.
সংরচনা cpu *
mycpu(শূন্য)
{
  পূর্ণাঙ্ক id = cpuid();
  সংরচনা cpu *c = &cpus[id];
  ঘূৰাই c;
}

// Return the current struct proc *, or zero if none.
সংরচনা proc *
myproc(শূন্য)
{
  ঠেলো_off();
  সংরচনা cpu *c = mycpu();
  সংরচনা proc *p = c->proc;
  বেৰ_কৰো_off();
  ঘূৰাই p;
}

পূর্ণাঙ্ক
allocpid()
{
  পূর্ণাঙ্ক pid;

  acquire(&pid_lock);
  pid = nextpid;
  nextpid = nextpid + 1;
  release(&pid_lock);

  ঘূৰাই pid;
}

// Look in the process table for an UNUSED proc.
// If found, initialize state required to run in the kernel,
// and return with p->lock held.
// If there are no free procs, or a memory allocation fails, return 0.
স্থির সংরচনা proc *
allocproc(শূন্য)
{
  সংরচনা proc *p;

  বাবে (p = proc; p < &proc[NPROC]; p++) {
    acquire(&p->lock);
    যদি (p->state == UNUSED) {
      গচ্ছ found;
    } অন্যথা {
      release(&p->lock);
    }
  }
  ঘূৰাই 0;

found:
  p->pid = allocpid();
  p->state = USED;

  // Allocate a trapframe page.
  যদি ((p->trapframe = (সংরচনা trapframe *)kalloc()) == 0) {
    freeproc(p);
    release(&p->lock);
    ঘূৰাই 0;
  }

  // An empty user page table.
  p->pagetable = proc_pagetable(p);
  যদি (p->pagetable == 0) {
    freeproc(p);
    release(&p->lock);
    ঘূৰাই 0;
  }

  // Set up new context to start executing at forkret,
  // which returns to user space.
  memset(&p->context, 0, পরিমাণ(p->context));
  p->context.ra = (uint64)forkret;
  p->context.sp = p->kstack + PGSIZE;

  ঘূৰাই p;
}

// free a proc structure and the data hanging from it,
// including user pages.
// p->lock must be held.
স্থির শূন্য
freeproc(সংরচনা proc *p)
{
  যদি (p->trapframe)
    kfree((শূন্য *)p->trapframe);
  p->trapframe = 0;
  যদি (p->pagetable)
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
proc_pagetable(সংরচনা proc *p)
{
  pagetable_t pagetable;

  // An empty page table.
  pagetable = uvmcreate();
  যদি (pagetable == 0)
    ঘূৰাই 0;

  // map the trampoline code (for system call return)
  // at the highest user virtual address.
  // only the supervisor uses it, on the way
  // to/from user space, so not PTE_U.
  যদি (mappages(pagetable, TRAMPOLINE, PGSIZE, (uint64)trampoline,
               PTE_R | PTE_X) < 0) {
    uvmfree(pagetable, 0);
    ঘূৰাই 0;
  }

  // map the trapframe page just below the trampoline page, for
  // trampoline.S.
  যদি (mappages(pagetable, TRAPFRAME, PGSIZE, (uint64)(p->trapframe),
               PTE_R | PTE_W) < 0) {
    uvmunmap(pagetable, TRAMPOLINE, 1, 0);
    uvmfree(pagetable, 0);
    ঘূৰাই 0;
  }

  ঘূৰাই pagetable;
}

// Free a process's page table, and free the
// physical memory it refers to.
শূন্য
proc_freepagetable(pagetable_t pagetable, uint64 sz)
{
  uvmunmap(pagetable, TRAMPOLINE, 1, 0);
  uvmunmap(pagetable, TRAPFRAME, 1, 0);
  uvmfree(pagetable, sz);
}

// Set up first user process.
শূন্য
userinit(শূন্য)
{
  সংরচনা proc *p;

  p = allocproc();
  initproc = p;

  p->cwd = namei("/");

  p->state = RUNNABLE;

  release(&p->lock);
}

// Grow or shrink user memory by n bytes.
// Return 0 on success, -1 on failure.
পূর্ণাঙ্ক
growproc(পূর্ণাঙ্ক n)
{
  uint64 sz;
  সংরচনা proc *p = myproc();

  sz = p->sz;
  যদি (n > 0) {
    যদি (sz + n > TRAPFRAME) {
      ঘূৰাই -1;
    }
    যদি ((sz = uvmalloc(p->pagetable, sz, sz + n, PTE_W)) == 0) {
      ঘূৰাই -1;
    }
  } অন্যথা যদি (n < 0) {
    sz = uvmdealloc(p->pagetable, sz, sz + n);
  }
  p->sz = sz;
  ঘূৰাই 0;
}

// Create a new process, copying the parent.
// Sets up child kernel stack to return as if from fork() system call.
পূর্ণাঙ্ক
kfork(শূন্য)
{
  পূর্ণাঙ্ক i, pid;
  সংরচনা proc *np;
  সংরচনা proc *p = myproc();

  // Allocate process.
  যদি ((np = allocproc()) == 0) {
    ঘূৰাই -1;
  }

  // Copy user memory from parent to child.
  যদি (uvmcopy(p->pagetable, np->pagetable, p->sz) < 0) {
    freeproc(np);
    release(&np->lock);
    ঘূৰাই -1;
  }
  np->sz = p->sz;

  // copy saved user registers.
  *(np->trapframe) = *(p->trapframe);

  // Cause fork to return 0 in the child.
  np->trapframe->a0 = 0;

  // increment reference counts on open file descriptors.
  বাবে (i = 0; i < NOFILE; i++)
    যদি (p->ofile[i])
      np->ofile[i] = filedup(p->ofile[i]);
  np->cwd = idup(p->cwd);

  safestrcpy(np->name, p->name, পরিমাণ(p->name));

  pid = np->pid;

  release(&np->lock);

  acquire(&wait_lock);
  np->parent = p;
  release(&wait_lock);

  acquire(&np->lock);
  np->state = RUNNABLE;
  release(&np->lock);

  ঘূৰাই pid;
}

// Pass p's abandoned children to init.
// Caller must hold wait_lock.
শূন্য
reparent(সংরচনা proc *p)
{
  সংরচনা proc *pp;

  বাবে (pp = proc; pp < &proc[NPROC]; pp++) {
    যদি (pp->parent == p) {
      pp->parent = initproc;
      wakeup(initproc);
    }
  }
}

// Exit the current process.  Does not return.
// An exited process remains in the zombie state
// until its parent calls wait().
শূন্য
kexit(পূর্ণাঙ্ক status)
{
  সংরচনা proc *p = myproc();

  যদি (p == initproc)
    panic("init exiting");

  // Close all open files.
  বাবে (পূর্ণাঙ্ক fd = 0; fd < NOFILE; fd++) {
    যদি (p->ofile[fd]) {
      সংরচনা file *f = p->ofile[fd];
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
পূর্ণাঙ্ক
kwait(uint64 addr)
{
  সংরচনা proc *pp;
  পূর্ণাঙ্ক havekids, pid;
  সংরচনা proc *p = myproc();

  acquire(&wait_lock);

  বাবে (;;) {
    // Scan through table looking for exited children.
    havekids = 0;
    বাবে (pp = proc; pp < &proc[NPROC]; pp++) {
      যদি (pp->parent == p) {
        // make sure the child isn't still in exit() or swtch().
        acquire(&pp->lock);

        havekids = 1;
        যদি (pp->state == ZOMBIE) {
          // Found one.
          pid = pp->pid;
          যদি (addr != 0 &&
              copyout(p->pagetable, p->sz, addr, (ৱর্ণ *)&pp->xstate,
                      পরিমাণ(pp->xstate)) < 0) {
            release(&pp->lock);
            release(&wait_lock);
            ঘূৰাই -1;
          }
          pp->parent = 0;
          freeproc(pp);
          release(&pp->lock);
          release(&wait_lock);
          ঘূৰাই pid;
        }
        release(&pp->lock);
      }
    }

    // No point waiting if we don't have any children.
    যদি (!havekids || killed(p)) {
      release(&wait_lock);
      ঘূৰাই -1;
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
শূন্য
scheduler(শূন্য)
{
  সংরচনা proc *p;
  সংরচনা cpu *c = mycpu();

  c->proc = 0;
  বাবে (;;) {
    // The most recent process to run may have had interrupts
    // turned off; enable them to avoid a deadlock if all
    // processes are waiting. Then turn them back off
    // to avoid a possible race between an interrupt
    // and wfi.
    intr_on();
    intr_off();

    পূর্ণাঙ্ক found = 0;
    বাবে (p = proc; p < &proc[NPROC]; p++) {
      acquire(&p->lock);
      যদি (p->state == RUNNABLE) {
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
    যদি (found == 0) {
      // nothing to run; stop running on this core until an interrupt.
      asm চঞ্চল("wfi");
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
শূন্য
sched(শূন্য)
{
  পূর্ণাঙ্ক intena;
  সংরচনা proc *p = myproc();

  যদি (!holding(&p->lock))
    panic("sched p->lock");
  যদি (mycpu()->noff != 1)
    panic("sched locks");
  যদি (p->state == RUNNING)
    panic("sched RUNNING");
  যদি (intr_get())
    panic("sched interruptible");

  intena = mycpu()->intena;
  swtch(&p->context, &mycpu()->context);
  mycpu()->intena = intena;
}

// Give up the CPU for one scheduling round.
শূন্য
yield(শূন্য)
{
  সংরচনা proc *p = myproc();
  acquire(&p->lock);
  p->state = RUNNABLE;
  sched();
  release(&p->lock);
}

// A fork child's very first scheduling by scheduler()
// will swtch to forkret.
শূন্য
forkret(শূন্য)
{
  বাহ্য ৱর্ণ userret[];
  স্থির পূর্ণাঙ্ক first = 1;
  সংরচনা proc *p = myproc();

  // Still holding p->lock from scheduler.
  release(&p->lock);

  যদি (first) {
    first = 0;

    // File system initialization must be run in the context of a
    // regular process (e.g., because it calls sleep), and thus cannot
    // be run from main().
    fsinit(ROOTDEV);

    // We can invoke kexec() now that file system is initialized.
    // Put the return value (argc) of kexec into a0.
    p->trapframe->a0 = kexec("/init", (ৱর্ণ *[]){"/init", 0});
    যদি (p->trapframe->a0 == -1) {
      panic("exec");
    }
  }

  // return to user space, mimicing usertrap()'s return.
  prepare_return();
  uint64 satp = MAKE_SATP(p->pagetable);
  uint64 trampoline_userret = TRAMPOLINE + (userret - trampoline);
  ((শূন্য (*)(uint64))trampoline_userret)(satp);
}

// Register current process as waiting for wakeups on chan.
শূন্য
sleep_prepare(শূন্য *chan)
{
  সংরচনা proc *p = myproc();

  acquire(&p->lock);
  যদি (chan == 0)
    panic("sleep_prepare: zero chan");
  p->chan = chan;
  release(&p->lock);
}

// Put the thread to sleep.  Assumes sleep_prepare() was called before.
// If the channel registered by sleep_prepare() has been woken up in
// the meantime, do not go to sleep, and instead return immediately.
শূন্য
sleep(শূন্য)
{
  সংরচনা proc *p = myproc();

  acquire(&p->lock);
  যদি (p->chan != 0) {
    p->state = SLEEPING;
    sched();
  }
  release(&p->lock);
}

// Wake up all processes sleeping on channel chan.
শূন্য
wakeup(শূন্য *chan)
{
  সংরচনা proc *p;

  বাবে (p = proc; p < &proc[NPROC]; p++) {
    acquire(&p->lock);
    যদি (p->chan == chan) {
      // If the process is waiting for wakeups on this channel,
      // signal that the wakeup happened by clearing p->chan.
      p->chan = 0;

      // If this waiting process has gotten so far as to actually
      // go to sleep, also set it back to RUNNING.
      যদি (p->state == SLEEPING) {
        p->state = RUNNABLE;
      }
    }
    release(&p->lock);
  }
}

// Kill the process with the given pid.
// The victim won't exit until it tries to return
// to user space (see usertrap() in trap.c).
পূর্ণাঙ্ক
kkill(পূর্ণাঙ্ক pid)
{
  সংরচনা proc *p;

  যদি (pid == 0)
    ঘূৰাই -1;

  বাবে (p = proc; p < &proc[NPROC]; p++) {
    acquire(&p->lock);
    যদি (p->pid == pid) {
      p->killed = 1;
      যদি (p->state == SLEEPING) {
        // Wake process from sleep().
        p->state = RUNNABLE;
      }
      release(&p->lock);
      ঘূৰাই 0;
    }
    release(&p->lock);
  }
  ঘূৰাই -1;
}

শূন্য
setkilled(সংরচনা proc *p)
{
  acquire(&p->lock);
  p->killed = 1;
  release(&p->lock);
}

পূর্ণাঙ্ক
killed(সংরচনা proc *p)
{
  পূর্ণাঙ্ক k;

  acquire(&p->lock);
  k = p->killed;
  release(&p->lock);
  ঘূৰাই k;
}

// Copy to either a user address, or kernel address,
// depending on usr_dst.
// Returns 0 on success, -1 on error.
পূর্ণাঙ্ক
either_copyout(পূর্ণাঙ্ক user_dst, uint64 dst, শূন্য *src, uint64 len)
{
  সংরচনা proc *p = myproc();
  যদি (user_dst) {
    ঘূৰাই copyout(p->pagetable, p->sz, dst, src, len);
  } অন্যথা {
    memmove((ৱর্ণ *)dst, src, len);
    ঘূৰাই 0;
  }
}

// Copy from either a user address, or kernel address,
// depending on usr_src.
// Returns 0 on success, -1 on error.
পূর্ণাঙ্ক
either_copyin(শূন্য *dst, পূর্ণাঙ্ক user_src, uint64 src, uint64 len)
{
  সংরচনা proc *p = myproc();
  যদি (user_src) {
    ঘূৰাই copyin(p->pagetable, p->sz, dst, src, len);
  } অন্যথা {
    memmove(dst, (ৱর্ণ *)src, len);
    ঘূৰাই 0;
  }
}

// Print a process listing to console.  For debugging.
// Runs when user types ^P on console.
// No lock to avoid wedging a stuck machine further.
শূন্য
procdump(শূন্য)
{
  স্থির ৱর্ণ *states[] = {
    // clang-format off
    [UNUSED]    = "unused",
    [USED]      = "used",
    [SLEEPING]  = "sleep ",
    [RUNNABLE]  = "runble",
    [RUNNING]   = "run   ",
    [ZOMBIE]    = "zombie"
    // clang-format on
  };
  সংরচনা proc *p;
  ৱর্ণ *state;

  printk("\n");
  বাবে (p = proc; p < &proc[NPROC]; p++) {
    যদি (p->state == UNUSED)
      অনুৱর্ত;
    যদি (p->state >= 0 && p->state < NELEM(states) && states[p->state])
      state = states[p->state];
    অন্যথা
      state = "???";
    printk("%d %s %s", p->pid, state, p->name);
    printk("\n");
  }
}
