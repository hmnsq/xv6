#अन्तर्भाव "types.h"
#अन्तर्भाव "param.h"
#अन्तर्भाव "memlayout.h"
#अन्तर्भाव "riscv.h"
#अन्तर्भाव "spinlock.h"
#अन्तर्भाव "proc.h"
#अन्तर्भाव "defs.h"

संरचना cpu cpus[NCPU];

संरचना proc proc[NPROC];

संरचना proc *initproc;

पूर्णाङ्क nextpid = 1;
संरचना spinlock pid_lock;

बाह्य शून्य forkret(शून्य);
स्थिर शून्य freeproc(संरचना proc *p);

बाह्य वर्ण trampoline[]; // trampoline.S

// helps ensure that wakeups of wait()ing
// parents are not lost. helps obey the
// memory model when using p->parent.
// must be acquired before any p->lock.
संरचना spinlock wait_lock;

// Allocate a page for each process's kernel stack.
// Map it high in memory, followed by an invalid
// guard page.
शून्य
proc_mapstacks(pagetable_t kpgtbl)
{
  संरचना proc *p;

  प्रति (p = proc; p < &proc[NPROC]; p++) {
    वर्ण *pa = kalloc();
    यदि (pa == 0)
      panic("kalloc");
    uint64 va = KSTACK((पूर्णाङ्क)(p - proc));
    kvmmap(kpgtbl, va, (uint64)pa, PGSIZE, PTE_R | PTE_W);
  }
}

// initialize the proc table.
शून्य
procinit(शून्य)
{
  संरचना proc *p;

  initlock(&pid_lock, "nextpid");
  initlock(&wait_lock, "wait_lock");
  प्रति (p = proc; p < &proc[NPROC]; p++) {
    initlock(&p->lock, "proc");
    p->state = UNUSED;
    p->kstack = KSTACK((पूर्णाङ्क)(p - proc));
  }
}

// Must be called with interrupts disabled,
// to prevent race with process being moved
// to a different CPU.
पूर्णाङ्क
cpuid()
{
  पूर्णाङ्क id = r_tp();
  प्रत्यागम id;
}

// Return this CPU's cpu struct.
// Interrupts must be disabled.
संरचना cpu *
mycpu(शून्य)
{
  पूर्णाङ्क id = cpuid();
  संरचना cpu *c = &cpus[id];
  प्रत्यागम c;
}

// Return the current struct proc *, or zero if none.
संरचना proc *
myproc(शून्य)
{
  धकेलो_off();
  संरचना cpu *c = mycpu();
  संरचना proc *p = c->proc;
  निकालो_off();
  प्रत्यागम p;
}

पूर्णाङ्क
allocpid()
{
  पूर्णाङ्क pid;

  acquire(&pid_lock);
  pid = nextpid;
  nextpid = nextpid + 1;
  release(&pid_lock);

  प्रत्यागम pid;
}

// Look in the process table for an UNUSED proc.
// If found, initialize state required to run in the kernel,
// and return with p->lock held.
// If there are no free procs, or a memory allocation fails, return 0.
स्थिर संरचना proc *
allocproc(शून्य)
{
  संरचना proc *p;

  प्रति (p = proc; p < &proc[NPROC]; p++) {
    acquire(&p->lock);
    यदि (p->state == UNUSED) {
      गच्छ found;
    } अन्यथा {
      release(&p->lock);
    }
  }
  प्रत्यागम 0;

found:
  p->pid = allocpid();
  p->state = USED;

  // Allocate a trapframe page.
  यदि ((p->trapframe = (संरचना trapframe *)kalloc()) == 0) {
    freeproc(p);
    release(&p->lock);
    प्रत्यागम 0;
  }

  // An empty user page table.
  p->pagetable = proc_pagetable(p);
  यदि (p->pagetable == 0) {
    freeproc(p);
    release(&p->lock);
    प्रत्यागम 0;
  }

  // Set up new context to start executing at forkret,
  // which returns to user space.
  memset(&p->context, 0, परिमाण(p->context));
  p->context.ra = (uint64)forkret;
  p->context.sp = p->kstack + PGSIZE;

  प्रत्यागम p;
}

// free a proc structure and the data hanging from it,
// including user pages.
// p->lock must be held.
स्थिर शून्य
freeproc(संरचना proc *p)
{
  यदि (p->trapframe)
    kfree((शून्य *)p->trapframe);
  p->trapframe = 0;
  यदि (p->pagetable)
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
proc_pagetable(संरचना proc *p)
{
  pagetable_t pagetable;

  // An empty page table.
  pagetable = uvmcreate();
  यदि (pagetable == 0)
    प्रत्यागम 0;

  // map the trampoline code (for system call return)
  // at the highest user virtual address.
  // only the supervisor uses it, on the way
  // to/from user space, so not PTE_U.
  यदि (mappages(pagetable, TRAMPOLINE, PGSIZE, (uint64)trampoline,
               PTE_R | PTE_X) < 0) {
    uvmfree(pagetable, 0);
    प्रत्यागम 0;
  }

  // map the trapframe page just below the trampoline page, for
  // trampoline.S.
  यदि (mappages(pagetable, TRAPFRAME, PGSIZE, (uint64)(p->trapframe),
               PTE_R | PTE_W) < 0) {
    uvmunmap(pagetable, TRAMPOLINE, 1, 0);
    uvmfree(pagetable, 0);
    प्रत्यागम 0;
  }

  प्रत्यागम pagetable;
}

// Free a process's page table, and free the
// physical memory it refers to.
शून्य
proc_freepagetable(pagetable_t pagetable, uint64 sz)
{
  uvmunmap(pagetable, TRAMPOLINE, 1, 0);
  uvmunmap(pagetable, TRAPFRAME, 1, 0);
  uvmfree(pagetable, sz);
}

// Set up first user process.
शून्य
userinit(शून्य)
{
  संरचना proc *p;

  p = allocproc();
  initproc = p;

  p->cwd = namei("/");

  p->state = RUNNABLE;

  release(&p->lock);
}

// Grow or shrink user memory by n bytes.
// Return 0 on success, -1 on failure.
पूर्णाङ्क
growproc(पूर्णाङ्क n)
{
  uint64 sz;
  संरचना proc *p = myproc();

  sz = p->sz;
  यदि (n > 0) {
    यदि (sz + n > TRAPFRAME) {
      प्रत्यागम -1;
    }
    यदि ((sz = uvmalloc(p->pagetable, sz, sz + n, PTE_W)) == 0) {
      प्रत्यागम -1;
    }
  } अन्यथा यदि (n < 0) {
    sz = uvmdealloc(p->pagetable, sz, sz + n);
  }
  p->sz = sz;
  प्रत्यागम 0;
}

// Create a new process, copying the parent.
// Sets up child kernel stack to return as if from fork() system call.
पूर्णाङ्क
kfork(शून्य)
{
  पूर्णाङ्क i, pid;
  संरचना proc *np;
  संरचना proc *p = myproc();

  // Allocate process.
  यदि ((np = allocproc()) == 0) {
    प्रत्यागम -1;
  }

  // Copy user memory from parent to child.
  यदि (uvmcopy(p->pagetable, np->pagetable, p->sz) < 0) {
    freeproc(np);
    release(&np->lock);
    प्रत्यागम -1;
  }
  np->sz = p->sz;

  // copy saved user registers.
  *(np->trapframe) = *(p->trapframe);

  // Cause fork to return 0 in the child.
  np->trapframe->a0 = 0;

  // increment reference counts on open file descriptors.
  प्रति (i = 0; i < NOFILE; i++)
    यदि (p->ofile[i])
      np->ofile[i] = filedup(p->ofile[i]);
  np->cwd = idup(p->cwd);

  safestrcpy(np->name, p->name, परिमाण(p->name));

  pid = np->pid;

  release(&np->lock);

  acquire(&wait_lock);
  np->parent = p;
  release(&wait_lock);

  acquire(&np->lock);
  np->state = RUNNABLE;
  release(&np->lock);

  प्रत्यागम pid;
}

// Pass p's abandoned children to init.
// Caller must hold wait_lock.
शून्य
reparent(संरचना proc *p)
{
  संरचना proc *pp;

  प्रति (pp = proc; pp < &proc[NPROC]; pp++) {
    यदि (pp->parent == p) {
      pp->parent = initproc;
      wakeup(initproc);
    }
  }
}

// Exit the current process.  Does not return.
// An exited process remains in the zombie state
// until its parent calls wait().
शून्य
kexit(पूर्णाङ्क status)
{
  संरचना proc *p = myproc();

  यदि (p == initproc)
    panic("init exiting");

  // Close all open files.
  प्रति (पूर्णाङ्क fd = 0; fd < NOFILE; fd++) {
    यदि (p->ofile[fd]) {
      संरचना file *f = p->ofile[fd];
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
पूर्णाङ्क
kwait(uint64 addr)
{
  संरचना proc *pp;
  पूर्णाङ्क havekids, pid;
  संरचना proc *p = myproc();

  acquire(&wait_lock);

  प्रति (;;) {
    // Scan through table looking for exited children.
    havekids = 0;
    प्रति (pp = proc; pp < &proc[NPROC]; pp++) {
      यदि (pp->parent == p) {
        // make sure the child isn't still in exit() or swtch().
        acquire(&pp->lock);

        havekids = 1;
        यदि (pp->state == ZOMBIE) {
          // Found one.
          pid = pp->pid;
          यदि (addr != 0 &&
              copyout(p->pagetable, p->sz, addr, (वर्ण *)&pp->xstate,
                      परिमाण(pp->xstate)) < 0) {
            release(&pp->lock);
            release(&wait_lock);
            प्रत्यागम -1;
          }
          pp->parent = 0;
          freeproc(pp);
          release(&pp->lock);
          release(&wait_lock);
          प्रत्यागम pid;
        }
        release(&pp->lock);
      }
    }

    // No point waiting if we don't have any children.
    यदि (!havekids || killed(p)) {
      release(&wait_lock);
      प्रत्यागम -1;
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
शून्य
scheduler(शून्य)
{
  संरचना proc *p;
  संरचना cpu *c = mycpu();

  c->proc = 0;
  प्रति (;;) {
    // The most recent process to run may have had interrupts
    // turned off; enable them to avoid a deadlock if all
    // processes are waiting. Then turn them back off
    // to avoid a possible race between an interrupt
    // and wfi.
    intr_on();
    intr_off();

    पूर्णाङ्क found = 0;
    प्रति (p = proc; p < &proc[NPROC]; p++) {
      acquire(&p->lock);
      यदि (p->state == RUNNABLE) {
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
    यदि (found == 0) {
      // nothing to run; stop running on this core until an interrupt.
      asm चञ्चल("wfi");
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
शून्य
sched(शून्य)
{
  पूर्णाङ्क intena;
  संरचना proc *p = myproc();

  यदि (!holding(&p->lock))
    panic("sched p->lock");
  यदि (mycpu()->noff != 1)
    panic("sched locks");
  यदि (p->state == RUNNING)
    panic("sched RUNNING");
  यदि (intr_get())
    panic("sched interruptible");

  intena = mycpu()->intena;
  swtch(&p->context, &mycpu()->context);
  mycpu()->intena = intena;
}

// Give up the CPU for one scheduling round.
शून्य
yield(शून्य)
{
  संरचना proc *p = myproc();
  acquire(&p->lock);
  p->state = RUNNABLE;
  sched();
  release(&p->lock);
}

// A fork child's very first scheduling by scheduler()
// will swtch to forkret.
शून्य
forkret(शून्य)
{
  बाह्य वर्ण userret[];
  स्थिर पूर्णाङ्क first = 1;
  संरचना proc *p = myproc();

  // Still holding p->lock from scheduler.
  release(&p->lock);

  यदि (first) {
    first = 0;

    // File system initialization must be run in the context of a
    // regular process (e.g., because it calls sleep), and thus cannot
    // be run from main().
    fsinit(ROOTDEV);

    // We can invoke kexec() now that file system is initialized.
    // Put the return value (argc) of kexec into a0.
    p->trapframe->a0 = kexec("/init", (वर्ण *[]){"/init", 0});
    यदि (p->trapframe->a0 == -1) {
      panic("exec");
    }
  }

  // return to user space, mimicing usertrap()'s return.
  prepare_return();
  uint64 satp = MAKE_SATP(p->pagetable);
  uint64 trampoline_userret = TRAMPOLINE + (userret - trampoline);
  ((शून्य (*)(uint64))trampoline_userret)(satp);
}

// Register current process as waiting for wakeups on chan.
शून्य
sleep_prepare(शून्य *chan)
{
  संरचना proc *p = myproc();

  acquire(&p->lock);
  यदि (chan == 0)
    panic("sleep_prepare: zero chan");
  p->chan = chan;
  release(&p->lock);
}

// Put the thread to sleep.  Assumes sleep_prepare() was called before.
// If the channel registered by sleep_prepare() has been woken up in
// the meantime, do not go to sleep, and instead return immediately.
शून्य
sleep(शून्य)
{
  संरचना proc *p = myproc();

  acquire(&p->lock);
  यदि (p->chan != 0) {
    p->state = SLEEPING;
    sched();
  }
  release(&p->lock);
}

// Wake up all processes sleeping on channel chan.
शून्य
wakeup(शून्य *chan)
{
  संरचना proc *p;

  प्रति (p = proc; p < &proc[NPROC]; p++) {
    acquire(&p->lock);
    यदि (p->chan == chan) {
      // If the process is waiting for wakeups on this channel,
      // signal that the wakeup happened by clearing p->chan.
      p->chan = 0;

      // If this waiting process has gotten so far as to actually
      // go to sleep, also set it back to RUNNING.
      यदि (p->state == SLEEPING) {
        p->state = RUNNABLE;
      }
    }
    release(&p->lock);
  }
}

// Kill the process with the given pid.
// The victim won't exit until it tries to return
// to user space (see usertrap() in trap.c).
पूर्णाङ्क
kkill(पूर्णाङ्क pid)
{
  संरचना proc *p;

  यदि (pid == 0)
    प्रत्यागम -1;

  प्रति (p = proc; p < &proc[NPROC]; p++) {
    acquire(&p->lock);
    यदि (p->pid == pid) {
      p->killed = 1;
      यदि (p->state == SLEEPING) {
        // Wake process from sleep().
        p->state = RUNNABLE;
      }
      release(&p->lock);
      प्रत्यागम 0;
    }
    release(&p->lock);
  }
  प्रत्यागम -1;
}

शून्य
setkilled(संरचना proc *p)
{
  acquire(&p->lock);
  p->killed = 1;
  release(&p->lock);
}

पूर्णाङ्क
killed(संरचना proc *p)
{
  पूर्णाङ्क k;

  acquire(&p->lock);
  k = p->killed;
  release(&p->lock);
  प्रत्यागम k;
}

// Copy to either a user address, or kernel address,
// depending on usr_dst.
// Returns 0 on success, -1 on error.
पूर्णाङ्क
either_copyout(पूर्णाङ्क user_dst, uint64 dst, शून्य *src, uint64 len)
{
  संरचना proc *p = myproc();
  यदि (user_dst) {
    प्रत्यागम copyout(p->pagetable, p->sz, dst, src, len);
  } अन्यथा {
    memmove((वर्ण *)dst, src, len);
    प्रत्यागम 0;
  }
}

// Copy from either a user address, or kernel address,
// depending on usr_src.
// Returns 0 on success, -1 on error.
पूर्णाङ्क
either_copyin(शून्य *dst, पूर्णाङ्क user_src, uint64 src, uint64 len)
{
  संरचना proc *p = myproc();
  यदि (user_src) {
    प्रत्यागम copyin(p->pagetable, p->sz, dst, src, len);
  } अन्यथा {
    memmove(dst, (वर्ण *)src, len);
    प्रत्यागम 0;
  }
}

// Print a process listing to console.  For debugging.
// Runs when user types ^P on console.
// No lock to avoid wedging a stuck machine further.
शून्य
procdump(शून्य)
{
  स्थिर वर्ण *states[] = {
    // clang-format off
    [UNUSED]    = "unused",
    [USED]      = "used",
    [SLEEPING]  = "sleep ",
    [RUNNABLE]  = "runble",
    [RUNNING]   = "run   ",
    [ZOMBIE]    = "zombie"
    // clang-format on
  };
  संरचना proc *p;
  वर्ण *state;

  printk("\n");
  प्रति (p = proc; p < &proc[NPROC]; p++) {
    यदि (p->state == UNUSED)
      अनुवर्त;
    यदि (p->state >= 0 && p->state < NELEM(states) && states[p->state])
      state = states[p->state];
    अन्यथा
      state = "???";
    printk("%d %s %s", p->pid, state, p->name);
    printk("\n");
  }
}
