#ضمن "types.h"
#ضمن "param.h"
#ضمن "memlayout.h"
#ضمن "riscv.h"
#ضمن "spinlock.h"
#ضمن "proc.h"
#ضمن "defs.h"

بنية cpu cpus[NCPU];

بنية proc proc[NPROC];

بنية proc *initproc;

صحيح nextpid = 1;
بنية spinlock pid_lock;

خارجي فراغ forkret(فراغ);
ساكن فراغ freeproc(بنية proc *p);

خارجي حرف trampoline[]; // trampoline.S

// helps ensure that wakeups of wait()ing
// parents are not lost. helps obey the
// memory model when using p->parent.
// must be acquired before any p->lock.
بنية spinlock wait_lock;

// Allocate a page for each process's kernel stack.
// Map it high in memory, followed by an invalid
// guard page.
فراغ
proc_mapstacks(pagetable_t kpgtbl)
{
  بنية proc *p;

  لكل (p = proc; p < &proc[NPROC]; p++) {
    حرف *pa = kalloc();
    إذا (pa == 0)
      panic("kalloc");
    uint64 va = KSTACK((صحيح)(p - proc));
    kvmmap(kpgtbl, va, (uint64)pa, PGSIZE, PTE_R | PTE_W);
  }
}

// initialize the proc table.
فراغ
procinit(فراغ)
{
  بنية proc *p;

  initlock(&pid_lock, "nextpid");
  initlock(&wait_lock, "wait_lock");
  لكل (p = proc; p < &proc[NPROC]; p++) {
    initlock(&p->lock, "proc");
    p->state = UNUSED;
    p->kstack = KSTACK((صحيح)(p - proc));
  }
}

// Must be called with interrupts disabled,
// to prevent race with process being moved
// to a different CPU.
صحيح
cpuid()
{
  صحيح id = r_tp();
  أرجع id;
}

// Return this CPU's cpu struct.
// Interrupts must be disabled.
بنية cpu *
mycpu(فراغ)
{
  صحيح id = cpuid();
  بنية cpu *c = &cpus[id];
  أرجع c;
}

// Return the current struct proc *, or zero if none.
بنية proc *
myproc(فراغ)
{
  ئىتتىرىش_off();
  بنية cpu *c = mycpu();
  بنية proc *p = c->proc;
  چىقىرىش_off();
  أرجع p;
}

صحيح
allocpid()
{
  صحيح pid;

  acquire(&pid_lock);
  pid = nextpid;
  nextpid = nextpid + 1;
  release(&pid_lock);

  أرجع pid;
}

// Look in the process table for an UNUSED proc.
// If found, initialize state required to run in the kernel,
// and return with p->lock held.
// If there are no free procs, or a memory allocation fails, return 0.
ساكن بنية proc *
allocproc(فراغ)
{
  بنية proc *p;

  لكل (p = proc; p < &proc[NPROC]; p++) {
    acquire(&p->lock);
    إذا (p->state == UNUSED) {
      اذهب found;
    } وإلا {
      release(&p->lock);
    }
  }
  أرجع 0;

found:
  p->pid = allocpid();
  p->state = USED;

  // Allocate a trapframe page.
  إذا ((p->trapframe = (بنية trapframe *)kalloc()) == 0) {
    freeproc(p);
    release(&p->lock);
    أرجع 0;
  }

  // An empty user page table.
  p->pagetable = proc_pagetable(p);
  إذا (p->pagetable == 0) {
    freeproc(p);
    release(&p->lock);
    أرجع 0;
  }

  // Set up new context to start executing at forkret,
  // which returns to user space.
  memset(&p->context, 0, حجم(p->context));
  p->context.ra = (uint64)forkret;
  p->context.sp = p->kstack + PGSIZE;

  أرجع p;
}

// free a proc structure and the data hanging from it,
// including user pages.
// p->lock must be held.
ساكن فراغ
freeproc(بنية proc *p)
{
  إذا (p->trapframe)
    kfree((فراغ *)p->trapframe);
  p->trapframe = 0;
  إذا (p->pagetable)
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
proc_pagetable(بنية proc *p)
{
  pagetable_t pagetable;

  // An empty page table.
  pagetable = uvmcreate();
  إذا (pagetable == 0)
    أرجع 0;

  // map the trampoline code (for system call return)
  // at the highest user virtual address.
  // only the supervisor uses it, on the way
  // to/from user space, so not PTE_U.
  إذا (mappages(pagetable, TRAMPOLINE, PGSIZE, (uint64)trampoline,
               PTE_R | PTE_X) < 0) {
    uvmfree(pagetable, 0);
    أرجع 0;
  }

  // map the trapframe page just below the trampoline page, for
  // trampoline.S.
  إذا (mappages(pagetable, TRAPFRAME, PGSIZE, (uint64)(p->trapframe),
               PTE_R | PTE_W) < 0) {
    uvmunmap(pagetable, TRAMPOLINE, 1, 0);
    uvmfree(pagetable, 0);
    أرجع 0;
  }

  أرجع pagetable;
}

// Free a process's page table, and free the
// physical memory it refers to.
فراغ
proc_freepagetable(pagetable_t pagetable, uint64 sz)
{
  uvmunmap(pagetable, TRAMPOLINE, 1, 0);
  uvmunmap(pagetable, TRAPFRAME, 1, 0);
  uvmfree(pagetable, sz);
}

// Set up first user process.
فراغ
userinit(فراغ)
{
  بنية proc *p;

  p = allocproc();
  initproc = p;

  p->cwd = namei("/");

  p->state = RUNNABLE;

  release(&p->lock);
}

// Grow or shrink user memory by n bytes.
// Return 0 on success, -1 on failure.
صحيح
growproc(صحيح n)
{
  uint64 sz;
  بنية proc *p = myproc();

  sz = p->sz;
  إذا (n > 0) {
    إذا (sz + n > TRAPFRAME) {
      أرجع -1;
    }
    إذا ((sz = uvmalloc(p->pagetable, sz, sz + n, PTE_W)) == 0) {
      أرجع -1;
    }
  } وإلا إذا (n < 0) {
    sz = uvmdealloc(p->pagetable, sz, sz + n);
  }
  p->sz = sz;
  أرجع 0;
}

// Create a new process, copying the parent.
// Sets up child kernel stack to return as if from fork() system call.
صحيح
kfork(فراغ)
{
  صحيح i, pid;
  بنية proc *np;
  بنية proc *p = myproc();

  // Allocate process.
  إذا ((np = allocproc()) == 0) {
    أرجع -1;
  }

  // Copy user memory from parent to child.
  إذا (uvmcopy(p->pagetable, np->pagetable, p->sz) < 0) {
    freeproc(np);
    release(&np->lock);
    أرجع -1;
  }
  np->sz = p->sz;

  // copy saved user registers.
  *(np->trapframe) = *(p->trapframe);

  // Cause fork to return 0 in the child.
  np->trapframe->a0 = 0;

  // increment reference counts on open file descriptors.
  لكل (i = 0; i < NOFILE; i++)
    إذا (p->ofile[i])
      np->ofile[i] = filedup(p->ofile[i]);
  np->cwd = idup(p->cwd);

  safestrcpy(np->name, p->name, حجم(p->name));

  pid = np->pid;

  release(&np->lock);

  acquire(&wait_lock);
  np->parent = p;
  release(&wait_lock);

  acquire(&np->lock);
  np->state = RUNNABLE;
  release(&np->lock);

  أرجع pid;
}

// Pass p's abandoned children to init.
// Caller must hold wait_lock.
فراغ
reparent(بنية proc *p)
{
  بنية proc *pp;

  لكل (pp = proc; pp < &proc[NPROC]; pp++) {
    إذا (pp->parent == p) {
      pp->parent = initproc;
      wakeup(initproc);
    }
  }
}

// Exit the current process.  Does not return.
// An exited process remains in the zombie state
// until its parent calls wait().
فراغ
kexit(صحيح status)
{
  بنية proc *p = myproc();

  إذا (p == initproc)
    panic("init exiting");

  // Close all open files.
  لكل (صحيح fd = 0; fd < NOFILE; fd++) {
    إذا (p->ofile[fd]) {
      بنية file *f = p->ofile[fd];
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
صحيح
kwait(uint64 addr)
{
  بنية proc *pp;
  صحيح havekids, pid;
  بنية proc *p = myproc();

  acquire(&wait_lock);

  لكل (;;) {
    // Scan through table looking for exited children.
    havekids = 0;
    لكل (pp = proc; pp < &proc[NPROC]; pp++) {
      إذا (pp->parent == p) {
        // make sure the child isn't still in exit() or swtch().
        acquire(&pp->lock);

        havekids = 1;
        إذا (pp->state == ZOMBIE) {
          // Found one.
          pid = pp->pid;
          إذا (addr != 0 &&
              copyout(p->pagetable, p->sz, addr, (حرف *)&pp->xstate,
                      حجم(pp->xstate)) < 0) {
            release(&pp->lock);
            release(&wait_lock);
            أرجع -1;
          }
          pp->parent = 0;
          freeproc(pp);
          release(&pp->lock);
          release(&wait_lock);
          أرجع pid;
        }
        release(&pp->lock);
      }
    }

    // No point waiting if we don't have any children.
    إذا (!havekids || killed(p)) {
      release(&wait_lock);
      أرجع -1;
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
فراغ
scheduler(فراغ)
{
  بنية proc *p;
  بنية cpu *c = mycpu();

  c->proc = 0;
  لكل (;;) {
    // The most recent process to run may have had interrupts
    // turned off; enable them to avoid a deadlock if all
    // processes are waiting. Then turn them back off
    // to avoid a possible race between an interrupt
    // and wfi.
    intr_on();
    intr_off();

    صحيح found = 0;
    لكل (p = proc; p < &proc[NPROC]; p++) {
      acquire(&p->lock);
      إذا (p->state == RUNNABLE) {
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
    إذا (found == 0) {
      // nothing to run; stop running on this core until an interrupt.
      asm متطاير("wfi");
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
فراغ
sched(فراغ)
{
  صحيح intena;
  بنية proc *p = myproc();

  إذا (!holding(&p->lock))
    panic("sched p->lock");
  إذا (mycpu()->noff != 1)
    panic("sched locks");
  إذا (p->state == RUNNING)
    panic("sched RUNNING");
  إذا (intr_get())
    panic("sched interruptible");

  intena = mycpu()->intena;
  swtch(&p->context, &mycpu()->context);
  mycpu()->intena = intena;
}

// Give up the CPU for one scheduling round.
فراغ
yield(فراغ)
{
  بنية proc *p = myproc();
  acquire(&p->lock);
  p->state = RUNNABLE;
  sched();
  release(&p->lock);
}

// A fork child's very first scheduling by scheduler()
// will swtch to forkret.
فراغ
forkret(فراغ)
{
  خارجي حرف userret[];
  ساكن صحيح first = 1;
  بنية proc *p = myproc();

  // Still holding p->lock from scheduler.
  release(&p->lock);

  إذا (first) {
    first = 0;

    // File system initialization must be run in the context of a
    // regular process (e.g., because it calls sleep), and thus cannot
    // be run from main().
    fsinit(ROOTDEV);

    // We can invoke kexec() now that file system is initialized.
    // Put the return value (argc) of kexec into a0.
    p->trapframe->a0 = kexec("/init", (حرف *[]){"/init", 0});
    إذا (p->trapframe->a0 == -1) {
      panic("exec");
    }
  }

  // return to user space, mimicing usertrap()'s return.
  prepare_return();
  uint64 satp = MAKE_SATP(p->pagetable);
  uint64 trampoline_userret = TRAMPOLINE + (userret - trampoline);
  ((فراغ (*)(uint64))trampoline_userret)(satp);
}

// Register current process as waiting for wakeups on chan.
فراغ
sleep_prepare(فراغ *chan)
{
  بنية proc *p = myproc();

  acquire(&p->lock);
  إذا (chan == 0)
    panic("sleep_prepare: zero chan");
  p->chan = chan;
  release(&p->lock);
}

// Put the thread to sleep.  Assumes sleep_prepare() was called before.
// If the channel registered by sleep_prepare() has been woken up in
// the meantime, do not go to sleep, and instead return immediately.
فراغ
sleep(فراغ)
{
  بنية proc *p = myproc();

  acquire(&p->lock);
  إذا (p->chan != 0) {
    p->state = SLEEPING;
    sched();
  }
  release(&p->lock);
}

// Wake up all processes sleeping on channel chan.
فراغ
wakeup(فراغ *chan)
{
  بنية proc *p;

  لكل (p = proc; p < &proc[NPROC]; p++) {
    acquire(&p->lock);
    إذا (p->chan == chan) {
      // If the process is waiting for wakeups on this channel,
      // signal that the wakeup happened by clearing p->chan.
      p->chan = 0;

      // If this waiting process has gotten so far as to actually
      // go to sleep, also set it back to RUNNING.
      إذا (p->state == SLEEPING) {
        p->state = RUNNABLE;
      }
    }
    release(&p->lock);
  }
}

// Kill the process with the given pid.
// The victim won't exit until it tries to return
// to user space (see usertrap() in trap.c).
صحيح
kkill(صحيح pid)
{
  بنية proc *p;

  إذا (pid == 0)
    أرجع -1;

  لكل (p = proc; p < &proc[NPROC]; p++) {
    acquire(&p->lock);
    إذا (p->pid == pid) {
      p->killed = 1;
      إذا (p->state == SLEEPING) {
        // Wake process from sleep().
        p->state = RUNNABLE;
      }
      release(&p->lock);
      أرجع 0;
    }
    release(&p->lock);
  }
  أرجع -1;
}

فراغ
setkilled(بنية proc *p)
{
  acquire(&p->lock);
  p->killed = 1;
  release(&p->lock);
}

صحيح
killed(بنية proc *p)
{
  صحيح k;

  acquire(&p->lock);
  k = p->killed;
  release(&p->lock);
  أرجع k;
}

// Copy to either a user address, or kernel address,
// depending on usr_dst.
// Returns 0 on success, -1 on error.
صحيح
either_copyout(صحيح user_dst, uint64 dst, فراغ *src, uint64 len)
{
  بنية proc *p = myproc();
  إذا (user_dst) {
    أرجع copyout(p->pagetable, p->sz, dst, src, len);
  } وإلا {
    memmove((حرف *)dst, src, len);
    أرجع 0;
  }
}

// Copy from either a user address, or kernel address,
// depending on usr_src.
// Returns 0 on success, -1 on error.
صحيح
either_copyin(فراغ *dst, صحيح user_src, uint64 src, uint64 len)
{
  بنية proc *p = myproc();
  إذا (user_src) {
    أرجع copyin(p->pagetable, p->sz, dst, src, len);
  } وإلا {
    memmove(dst, (حرف *)src, len);
    أرجع 0;
  }
}

// Print a process listing to console.  For debugging.
// Runs when user types ^P on console.
// No lock to avoid wedging a stuck machine further.
فراغ
procdump(فراغ)
{
  ساكن حرف *states[] = {
    // clang-format off
    [UNUSED]    = "unused",
    [USED]      = "used",
    [SLEEPING]  = "sleep ",
    [RUNNABLE]  = "runble",
    [RUNNING]   = "run   ",
    [ZOMBIE]    = "zombie"
    // clang-format on
  };
  بنية proc *p;
  حرف *state;

  printk("\n");
  لكل (p = proc; p < &proc[NPROC]; p++) {
    إذا (p->state == UNUSED)
      استمر;
    إذا (p->state >= 0 && p->state < NELEM(states) && states[p->state])
      state = states[p->state];
    وإلا
      state = "???";
    printk("%d %s %s", p->pid, state, p->name);
    printk("\n");
  }
}
