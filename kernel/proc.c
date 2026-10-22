#அந்தர்பாவ "types.h"
#அந்தர்பாவ "param.h"
#அந்தர்பாவ "memlayout.h"
#அந்தர்பாவ "riscv.h"
#அந்தர்பாவ "spinlock.h"
#அந்தர்பாவ "proc.h"
#அந்தர்பாவ "defs.h"

கட்டமைப்பு cpu cpus[NCPU];

கட்டமைப்பு proc proc[NPROC];

கட்டமைப்பு proc *initproc;

முழு nextpid = 1;
கட்டமைப்பு spinlock pid_lock;

வெளி வெறுமை forkret(வெறுமை);
நிலையான வெறுமை freeproc(கட்டமைப்பு proc *p);

வெளி எழுத்து trampoline[]; // trampoline.S

// helps ensure that wakeups of wait()ing
// parents are not lost. helps obey the
// memory model when using p->parent.
// must be acquired before any p->lock.
கட்டமைப்பு spinlock wait_lock;

// Allocate a page for each process's kernel stack.
// Map it high in memory, followed by an invalid
// guard page.
வெறுமை
proc_mapstacks(pagetable_t kpgtbl)
{
  கட்டமைப்பு proc *p;

  க்கு (p = proc; p < &proc[NPROC]; p++) {
    எழுத்து *pa = kalloc();
    எனில் (pa == 0)
      panic("kalloc");
    uint64 va = KSTACK((முழு)(p - proc));
    kvmmap(kpgtbl, va, (uint64)pa, PGSIZE, PTE_R | PTE_W);
  }
}

// initialize the proc table.
வெறுமை
procinit(வெறுமை)
{
  கட்டமைப்பு proc *p;

  initlock(&pid_lock, "nextpid");
  initlock(&wait_lock, "wait_lock");
  க்கு (p = proc; p < &proc[NPROC]; p++) {
    initlock(&p->lock, "proc");
    p->state = UNUSED;
    p->kstack = KSTACK((முழு)(p - proc));
  }
}

// Must be called with interrupts disabled,
// to prevent race with process being moved
// to a different CPU.
முழு
cpuid()
{
  முழு id = r_tp();
  திரும்பு id;
}

// Return this CPU's cpu struct.
// Interrupts must be disabled.
கட்டமைப்பு cpu *
mycpu(வெறுமை)
{
  முழு id = cpuid();
  கட்டமைப்பு cpu *c = &cpus[id];
  திரும்பு c;
}

// Return the current struct proc *, or zero if none.
கட்டமைப்பு proc *
myproc(வெறுமை)
{
  தள்ளு_off();
  கட்டமைப்பு cpu *c = mycpu();
  கட்டமைப்பு proc *p = c->proc;
  எடு_off();
  திரும்பு p;
}

முழு
allocpid()
{
  முழு pid;

  acquire(&pid_lock);
  pid = nextpid;
  nextpid = nextpid + 1;
  release(&pid_lock);

  திரும்பு pid;
}

// Look in the process table for an UNUSED proc.
// If found, initialize state required to run in the kernel,
// and return with p->lock held.
// If there are no free procs, or a memory allocation fails, return 0.
நிலையான கட்டமைப்பு proc *
allocproc(வெறுமை)
{
  கட்டமைப்பு proc *p;

  க்கு (p = proc; p < &proc[NPROC]; p++) {
    acquire(&p->lock);
    எனில் (p->state == UNUSED) {
      செல் found;
    } இல்லையெனில் {
      release(&p->lock);
    }
  }
  திரும்பு 0;

found:
  p->pid = allocpid();
  p->state = USED;

  // Allocate a trapframe page.
  எனில் ((p->trapframe = (கட்டமைப்பு trapframe *)kalloc()) == 0) {
    freeproc(p);
    release(&p->lock);
    திரும்பு 0;
  }

  // An empty user page table.
  p->pagetable = proc_pagetable(p);
  எனில் (p->pagetable == 0) {
    freeproc(p);
    release(&p->lock);
    திரும்பு 0;
  }

  // Set up new context to start executing at forkret,
  // which returns to user space.
  memset(&p->context, 0, அளவு(p->context));
  p->context.ra = (uint64)forkret;
  p->context.sp = p->kstack + PGSIZE;

  திரும்பு p;
}

// free a proc structure and the data hanging from it,
// including user pages.
// p->lock must be held.
நிலையான வெறுமை
freeproc(கட்டமைப்பு proc *p)
{
  எனில் (p->trapframe)
    kfree((வெறுமை *)p->trapframe);
  p->trapframe = 0;
  எனில் (p->pagetable)
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
proc_pagetable(கட்டமைப்பு proc *p)
{
  pagetable_t pagetable;

  // An empty page table.
  pagetable = uvmcreate();
  எனில் (pagetable == 0)
    திரும்பு 0;

  // map the trampoline code (for system call return)
  // at the highest user virtual address.
  // only the supervisor uses it, on the way
  // to/from user space, so not PTE_U.
  எனில் (mappages(pagetable, TRAMPOLINE, PGSIZE, (uint64)trampoline,
               PTE_R | PTE_X) < 0) {
    uvmfree(pagetable, 0);
    திரும்பு 0;
  }

  // map the trapframe page just below the trampoline page, for
  // trampoline.S.
  எனில் (mappages(pagetable, TRAPFRAME, PGSIZE, (uint64)(p->trapframe),
               PTE_R | PTE_W) < 0) {
    uvmunmap(pagetable, TRAMPOLINE, 1, 0);
    uvmfree(pagetable, 0);
    திரும்பு 0;
  }

  திரும்பு pagetable;
}

// Free a process's page table, and free the
// physical memory it refers to.
வெறுமை
proc_freepagetable(pagetable_t pagetable, uint64 sz)
{
  uvmunmap(pagetable, TRAMPOLINE, 1, 0);
  uvmunmap(pagetable, TRAPFRAME, 1, 0);
  uvmfree(pagetable, sz);
}

// Set up first user process.
வெறுமை
userinit(வெறுமை)
{
  கட்டமைப்பு proc *p;

  p = allocproc();
  initproc = p;

  p->cwd = namei("/");

  p->state = RUNNABLE;

  release(&p->lock);
}

// Grow or shrink user memory by n bytes.
// Return 0 on success, -1 on failure.
முழு
growproc(முழு n)
{
  uint64 sz;
  கட்டமைப்பு proc *p = myproc();

  sz = p->sz;
  எனில் (n > 0) {
    எனில் (sz + n > TRAPFRAME) {
      திரும்பு -1;
    }
    எனில் ((sz = uvmalloc(p->pagetable, sz, sz + n, PTE_W)) == 0) {
      திரும்பு -1;
    }
  } இல்லையெனில் எனில் (n < 0) {
    sz = uvmdealloc(p->pagetable, sz, sz + n);
  }
  p->sz = sz;
  திரும்பு 0;
}

// Create a new process, copying the parent.
// Sets up child kernel stack to return as if from fork() system call.
முழு
kfork(வெறுமை)
{
  முழு i, pid;
  கட்டமைப்பு proc *np;
  கட்டமைப்பு proc *p = myproc();

  // Allocate process.
  எனில் ((np = allocproc()) == 0) {
    திரும்பு -1;
  }

  // Copy user memory from parent to child.
  எனில் (uvmcopy(p->pagetable, np->pagetable, p->sz) < 0) {
    freeproc(np);
    release(&np->lock);
    திரும்பு -1;
  }
  np->sz = p->sz;

  // copy saved user registers.
  *(np->trapframe) = *(p->trapframe);

  // Cause fork to return 0 in the child.
  np->trapframe->a0 = 0;

  // increment reference counts on open file descriptors.
  க்கு (i = 0; i < NOFILE; i++)
    எனில் (p->ofile[i])
      np->ofile[i] = filedup(p->ofile[i]);
  np->cwd = idup(p->cwd);

  safestrcpy(np->name, p->name, அளவு(p->name));

  pid = np->pid;

  release(&np->lock);

  acquire(&wait_lock);
  np->parent = p;
  release(&wait_lock);

  acquire(&np->lock);
  np->state = RUNNABLE;
  release(&np->lock);

  திரும்பு pid;
}

// Pass p's abandoned children to init.
// Caller must hold wait_lock.
வெறுமை
reparent(கட்டமைப்பு proc *p)
{
  கட்டமைப்பு proc *pp;

  க்கு (pp = proc; pp < &proc[NPROC]; pp++) {
    எனில் (pp->parent == p) {
      pp->parent = initproc;
      wakeup(initproc);
    }
  }
}

// Exit the current process.  Does not return.
// An exited process remains in the zombie state
// until its parent calls wait().
வெறுமை
kexit(முழு status)
{
  கட்டமைப்பு proc *p = myproc();

  எனில் (p == initproc)
    panic("init exiting");

  // Close all open files.
  க்கு (முழு fd = 0; fd < NOFILE; fd++) {
    எனில் (p->ofile[fd]) {
      கட்டமைப்பு file *f = p->ofile[fd];
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
முழு
kwait(uint64 addr)
{
  கட்டமைப்பு proc *pp;
  முழு havekids, pid;
  கட்டமைப்பு proc *p = myproc();

  acquire(&wait_lock);

  க்கு (;;) {
    // Scan through table looking for exited children.
    havekids = 0;
    க்கு (pp = proc; pp < &proc[NPROC]; pp++) {
      எனில் (pp->parent == p) {
        // make sure the child isn't still in exit() or swtch().
        acquire(&pp->lock);

        havekids = 1;
        எனில் (pp->state == ZOMBIE) {
          // Found one.
          pid = pp->pid;
          எனில் (addr != 0 &&
              copyout(p->pagetable, p->sz, addr, (எழுத்து *)&pp->xstate,
                      அளவு(pp->xstate)) < 0) {
            release(&pp->lock);
            release(&wait_lock);
            திரும்பு -1;
          }
          pp->parent = 0;
          freeproc(pp);
          release(&pp->lock);
          release(&wait_lock);
          திரும்பு pid;
        }
        release(&pp->lock);
      }
    }

    // No point waiting if we don't have any children.
    எனில் (!havekids || killed(p)) {
      release(&wait_lock);
      திரும்பு -1;
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
வெறுமை
scheduler(வெறுமை)
{
  கட்டமைப்பு proc *p;
  கட்டமைப்பு cpu *c = mycpu();

  c->proc = 0;
  க்கு (;;) {
    // The most recent process to run may have had interrupts
    // turned off; enable them to avoid a deadlock if all
    // processes are waiting. Then turn them back off
    // to avoid a possible race between an interrupt
    // and wfi.
    intr_on();
    intr_off();

    முழு found = 0;
    க்கு (p = proc; p < &proc[NPROC]; p++) {
      acquire(&p->lock);
      எனில் (p->state == RUNNABLE) {
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
    எனில் (found == 0) {
      // nothing to run; stop running on this core until an interrupt.
      asm நிலையற்ற("wfi");
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
வெறுமை
sched(வெறுமை)
{
  முழு intena;
  கட்டமைப்பு proc *p = myproc();

  எனில் (!holding(&p->lock))
    panic("sched p->lock");
  எனில் (mycpu()->noff != 1)
    panic("sched locks");
  எனில் (p->state == RUNNING)
    panic("sched RUNNING");
  எனில் (intr_get())
    panic("sched interruptible");

  intena = mycpu()->intena;
  swtch(&p->context, &mycpu()->context);
  mycpu()->intena = intena;
}

// Give up the CPU for one scheduling round.
வெறுமை
yield(வெறுமை)
{
  கட்டமைப்பு proc *p = myproc();
  acquire(&p->lock);
  p->state = RUNNABLE;
  sched();
  release(&p->lock);
}

// A fork child's very first scheduling by scheduler()
// will swtch to forkret.
வெறுமை
forkret(வெறுமை)
{
  வெளி எழுத்து userret[];
  நிலையான முழு first = 1;
  கட்டமைப்பு proc *p = myproc();

  // Still holding p->lock from scheduler.
  release(&p->lock);

  எனில் (first) {
    first = 0;

    // File system initialization must be run in the context of a
    // regular process (e.g., because it calls sleep), and thus cannot
    // be run from main().
    fsinit(ROOTDEV);

    // We can invoke kexec() now that file system is initialized.
    // Put the return value (argc) of kexec into a0.
    p->trapframe->a0 = kexec("/init", (எழுத்து *[]){"/init", 0});
    எனில் (p->trapframe->a0 == -1) {
      panic("exec");
    }
  }

  // return to user space, mimicing usertrap()'s return.
  prepare_return();
  uint64 satp = MAKE_SATP(p->pagetable);
  uint64 trampoline_userret = TRAMPOLINE + (userret - trampoline);
  ((வெறுமை (*)(uint64))trampoline_userret)(satp);
}

// Register current process as waiting for wakeups on chan.
வெறுமை
sleep_prepare(வெறுமை *chan)
{
  கட்டமைப்பு proc *p = myproc();

  acquire(&p->lock);
  எனில் (chan == 0)
    panic("sleep_prepare: zero chan");
  p->chan = chan;
  release(&p->lock);
}

// Put the thread to sleep.  Assumes sleep_prepare() was called before.
// If the channel registered by sleep_prepare() has been woken up in
// the meantime, do not go to sleep, and instead return immediately.
வெறுமை
sleep(வெறுமை)
{
  கட்டமைப்பு proc *p = myproc();

  acquire(&p->lock);
  எனில் (p->chan != 0) {
    p->state = SLEEPING;
    sched();
  }
  release(&p->lock);
}

// Wake up all processes sleeping on channel chan.
வெறுமை
wakeup(வெறுமை *chan)
{
  கட்டமைப்பு proc *p;

  க்கு (p = proc; p < &proc[NPROC]; p++) {
    acquire(&p->lock);
    எனில் (p->chan == chan) {
      // If the process is waiting for wakeups on this channel,
      // signal that the wakeup happened by clearing p->chan.
      p->chan = 0;

      // If this waiting process has gotten so far as to actually
      // go to sleep, also set it back to RUNNING.
      எனில் (p->state == SLEEPING) {
        p->state = RUNNABLE;
      }
    }
    release(&p->lock);
  }
}

// Kill the process with the given pid.
// The victim won't exit until it tries to return
// to user space (see usertrap() in trap.c).
முழு
kkill(முழு pid)
{
  கட்டமைப்பு proc *p;

  எனில் (pid == 0)
    திரும்பு -1;

  க்கு (p = proc; p < &proc[NPROC]; p++) {
    acquire(&p->lock);
    எனில் (p->pid == pid) {
      p->killed = 1;
      எனில் (p->state == SLEEPING) {
        // Wake process from sleep().
        p->state = RUNNABLE;
      }
      release(&p->lock);
      திரும்பு 0;
    }
    release(&p->lock);
  }
  திரும்பு -1;
}

வெறுமை
setkilled(கட்டமைப்பு proc *p)
{
  acquire(&p->lock);
  p->killed = 1;
  release(&p->lock);
}

முழு
killed(கட்டமைப்பு proc *p)
{
  முழு k;

  acquire(&p->lock);
  k = p->killed;
  release(&p->lock);
  திரும்பு k;
}

// Copy to either a user address, or kernel address,
// depending on usr_dst.
// Returns 0 on success, -1 on error.
முழு
either_copyout(முழு user_dst, uint64 dst, வெறுமை *src, uint64 len)
{
  கட்டமைப்பு proc *p = myproc();
  எனில் (user_dst) {
    திரும்பு copyout(p->pagetable, p->sz, dst, src, len);
  } இல்லையெனில் {
    memmove((எழுத்து *)dst, src, len);
    திரும்பு 0;
  }
}

// Copy from either a user address, or kernel address,
// depending on usr_src.
// Returns 0 on success, -1 on error.
முழு
either_copyin(வெறுமை *dst, முழு user_src, uint64 src, uint64 len)
{
  கட்டமைப்பு proc *p = myproc();
  எனில் (user_src) {
    திரும்பு copyin(p->pagetable, p->sz, dst, src, len);
  } இல்லையெனில் {
    memmove(dst, (எழுத்து *)src, len);
    திரும்பு 0;
  }
}

// Print a process listing to console.  For debugging.
// Runs when user types ^P on console.
// No lock to avoid wedging a stuck machine further.
வெறுமை
procdump(வெறுமை)
{
  நிலையான எழுத்து *states[] = {
    // clang-format off
    [UNUSED]    = "unused",
    [USED]      = "used",
    [SLEEPING]  = "sleep ",
    [RUNNABLE]  = "runble",
    [RUNNING]   = "run   ",
    [ZOMBIE]    = "zombie"
    // clang-format on
  };
  கட்டமைப்பு proc *p;
  எழுத்து *state;

  printk("\n");
  க்கு (p = proc; p < &proc[NPROC]; p++) {
    எனில் (p->state == UNUSED)
      தொடர்;
    எனில் (p->state >= 0 && p->state < NELEM(states) && states[p->state])
      state = states[p->state];
    இல்லையெனில்
      state = "???";
    printk("%d %s %s", p->pid, state, p->name);
    printk("\n");
  }
}
