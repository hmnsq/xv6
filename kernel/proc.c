#অন্তর্ভাব "types.h"
#অন্তর্ভাব "param.h"
#অন্তর্ভাব "memlayout.h"
#অন্তর্ভাব "riscv.h"
#অন্তর্ভাব "spinlock.h"
#অন্তর্ভাব "proc.h"
#অন্তর্ভাব "defs.h"

কাঠামো cpu cpus[NCPU];

কাঠামো proc proc[NPROC];

কাঠামো proc *initproc;

পূর্ণসংখ্যা nextpid = 1;
কাঠামো spinlock pid_lock;

বাহ্যিক শূন্য forkret(শূন্য);
স্থির শূন্য freeproc(কাঠামো proc *p);

বাহ্যিক অক্ষর trampoline[]; // trampoline.S

// helps ensure that wakeups of wait()ing
// parents are not lost. helps obey the
// memory model when using p->parent.
// must be acquired before any p->lock.
কাঠামো spinlock wait_lock;

// Allocate a page for each process's kernel stack.
// Map it high in memory, followed by an invalid
// guard page.
শূন্য
proc_mapstacks(pagetable_t kpgtbl)
{
  কাঠামো proc *p;

  জন্য (p = proc; p < &proc[NPROC]; p++) {
    অক্ষর *pa = kalloc();
    যদি (pa == 0)
      panic("kalloc");
    uint64 va = KSTACK((পূর্ণসংখ্যা)(p - proc));
    kvmmap(kpgtbl, va, (uint64)pa, PGSIZE, PTE_R | PTE_W);
  }
}

// initialize the proc table.
শূন্য
procinit(শূন্য)
{
  কাঠামো proc *p;

  initlock(&pid_lock, "nextpid");
  initlock(&wait_lock, "wait_lock");
  জন্য (p = proc; p < &proc[NPROC]; p++) {
    initlock(&p->lock, "proc");
    p->state = UNUSED;
    p->kstack = KSTACK((পূর্ণসংখ্যা)(p - proc));
  }
}

// Must be called with interrupts disabled,
// to prevent race with process being moved
// to a different CPU.
পূর্ণসংখ্যা
cpuid()
{
  পূর্ণসংখ্যা id = r_tp();
  ফেরত id;
}

// Return this CPU's cpu struct.
// Interrupts must be disabled.
কাঠামো cpu *
mycpu(শূন্য)
{
  পূর্ণসংখ্যা id = cpuid();
  কাঠামো cpu *c = &cpus[id];
  ফেরত c;
}

// Return the current struct proc *, or zero if none.
কাঠামো proc *
myproc(শূন্য)
{
  ঠেলো_off();
  কাঠামো cpu *c = mycpu();
  কাঠামো proc *p = c->proc;
  বের_করো_off();
  ফেরত p;
}

পূর্ণসংখ্যা
allocpid()
{
  পূর্ণসংখ্যা pid;

  acquire(&pid_lock);
  pid = nextpid;
  nextpid = nextpid + 1;
  release(&pid_lock);

  ফেরত pid;
}

// Look in the process table for an UNUSED proc.
// If found, initialize state required to run in the kernel,
// and return with p->lock held.
// If there are no free procs, or a memory allocation fails, return 0.
স্থির কাঠামো proc *
allocproc(শূন্য)
{
  কাঠামো proc *p;

  জন্য (p = proc; p < &proc[NPROC]; p++) {
    acquire(&p->lock);
    যদি (p->state == UNUSED) {
      যাও found;
    } অন্যথা {
      release(&p->lock);
    }
  }
  ফেরত 0;

found:
  p->pid = allocpid();
  p->state = USED;

  // Allocate a trapframe page.
  যদি ((p->trapframe = (কাঠামো trapframe *)kalloc()) == 0) {
    freeproc(p);
    release(&p->lock);
    ফেরত 0;
  }

  // An empty user page table.
  p->pagetable = proc_pagetable(p);
  যদি (p->pagetable == 0) {
    freeproc(p);
    release(&p->lock);
    ফেরত 0;
  }

  // Set up new context to start executing at forkret,
  // which returns to user space.
  memset(&p->context, 0, আকার(p->context));
  p->context.ra = (uint64)forkret;
  p->context.sp = p->kstack + PGSIZE;

  ফেরত p;
}

// free a proc structure and the data hanging from it,
// including user pages.
// p->lock must be held.
স্থির শূন্য
freeproc(কাঠামো proc *p)
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
proc_pagetable(কাঠামো proc *p)
{
  pagetable_t pagetable;

  // An empty page table.
  pagetable = uvmcreate();
  যদি (pagetable == 0)
    ফেরত 0;

  // map the trampoline code (for system call return)
  // at the highest user virtual address.
  // only the supervisor uses it, on the way
  // to/from user space, so not PTE_U.
  যদি (mappages(pagetable, TRAMPOLINE, PGSIZE, (uint64)trampoline,
               PTE_R | PTE_X) < 0) {
    uvmfree(pagetable, 0);
    ফেরত 0;
  }

  // map the trapframe page just below the trampoline page, for
  // trampoline.S.
  যদি (mappages(pagetable, TRAPFRAME, PGSIZE, (uint64)(p->trapframe),
               PTE_R | PTE_W) < 0) {
    uvmunmap(pagetable, TRAMPOLINE, 1, 0);
    uvmfree(pagetable, 0);
    ফেরত 0;
  }

  ফেরত pagetable;
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
  কাঠামো proc *p;

  p = allocproc();
  initproc = p;

  p->cwd = namei("/");

  p->state = RUNNABLE;

  release(&p->lock);
}

// Grow or shrink user memory by n bytes.
// Return 0 on success, -1 on failure.
পূর্ণসংখ্যা
growproc(পূর্ণসংখ্যা n)
{
  uint64 sz;
  কাঠামো proc *p = myproc();

  sz = p->sz;
  যদি (n > 0) {
    যদি (sz + n > TRAPFRAME) {
      ফেরত -1;
    }
    যদি ((sz = uvmalloc(p->pagetable, sz, sz + n, PTE_W)) == 0) {
      ফেরত -1;
    }
  } অন্যথা যদি (n < 0) {
    sz = uvmdealloc(p->pagetable, sz, sz + n);
  }
  p->sz = sz;
  ফেরত 0;
}

// Create a new process, copying the parent.
// Sets up child kernel stack to return as if from fork() system call.
পূর্ণসংখ্যা
kfork(শূন্য)
{
  পূর্ণসংখ্যা i, pid;
  কাঠামো proc *np;
  কাঠামো proc *p = myproc();

  // Allocate process.
  যদি ((np = allocproc()) == 0) {
    ফেরত -1;
  }

  // Copy user memory from parent to child.
  যদি (uvmcopy(p->pagetable, np->pagetable, p->sz) < 0) {
    freeproc(np);
    release(&np->lock);
    ফেরত -1;
  }
  np->sz = p->sz;

  // copy saved user registers.
  *(np->trapframe) = *(p->trapframe);

  // Cause fork to return 0 in the child.
  np->trapframe->a0 = 0;

  // increment reference counts on open file descriptors.
  জন্য (i = 0; i < NOFILE; i++)
    যদি (p->ofile[i])
      np->ofile[i] = filedup(p->ofile[i]);
  np->cwd = idup(p->cwd);

  safestrcpy(np->name, p->name, আকার(p->name));

  pid = np->pid;

  release(&np->lock);

  acquire(&wait_lock);
  np->parent = p;
  release(&wait_lock);

  acquire(&np->lock);
  np->state = RUNNABLE;
  release(&np->lock);

  ফেরত pid;
}

// Pass p's abandoned children to init.
// Caller must hold wait_lock.
শূন্য
reparent(কাঠামো proc *p)
{
  কাঠামো proc *pp;

  জন্য (pp = proc; pp < &proc[NPROC]; pp++) {
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
kexit(পূর্ণসংখ্যা status)
{
  কাঠামো proc *p = myproc();

  যদি (p == initproc)
    panic("init exiting");

  // Close all open files.
  জন্য (পূর্ণসংখ্যা fd = 0; fd < NOFILE; fd++) {
    যদি (p->ofile[fd]) {
      কাঠামো file *f = p->ofile[fd];
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
পূর্ণসংখ্যা
kwait(uint64 addr)
{
  কাঠামো proc *pp;
  পূর্ণসংখ্যা havekids, pid;
  কাঠামো proc *p = myproc();

  acquire(&wait_lock);

  জন্য (;;) {
    // Scan through table looking for exited children.
    havekids = 0;
    জন্য (pp = proc; pp < &proc[NPROC]; pp++) {
      যদি (pp->parent == p) {
        // make sure the child isn't still in exit() or swtch().
        acquire(&pp->lock);

        havekids = 1;
        যদি (pp->state == ZOMBIE) {
          // Found one.
          pid = pp->pid;
          যদি (addr != 0 &&
              copyout(p->pagetable, p->sz, addr, (অক্ষর *)&pp->xstate,
                      আকার(pp->xstate)) < 0) {
            release(&pp->lock);
            release(&wait_lock);
            ফেরত -1;
          }
          pp->parent = 0;
          freeproc(pp);
          release(&pp->lock);
          release(&wait_lock);
          ফেরত pid;
        }
        release(&pp->lock);
      }
    }

    // No point waiting if we don't have any children.
    যদি (!havekids || killed(p)) {
      release(&wait_lock);
      ফেরত -1;
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
  কাঠামো proc *p;
  কাঠামো cpu *c = mycpu();

  c->proc = 0;
  জন্য (;;) {
    // The most recent process to run may have had interrupts
    // turned off; enable them to avoid a deadlock if all
    // processes are waiting. Then turn them back off
    // to avoid a possible race between an interrupt
    // and wfi.
    intr_on();
    intr_off();

    পূর্ণসংখ্যা found = 0;
    জন্য (p = proc; p < &proc[NPROC]; p++) {
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
      asm অস্থির("wfi");
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
  পূর্ণসংখ্যা intena;
  কাঠামো proc *p = myproc();

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
  কাঠামো proc *p = myproc();
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
  বাহ্যিক অক্ষর userret[];
  স্থির পূর্ণসংখ্যা first = 1;
  কাঠামো proc *p = myproc();

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
    p->trapframe->a0 = kexec("/init", (অক্ষর *[]){"/init", 0});
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
  কাঠামো proc *p = myproc();

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
  কাঠামো proc *p = myproc();

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
  কাঠামো proc *p;

  জন্য (p = proc; p < &proc[NPROC]; p++) {
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
পূর্ণসংখ্যা
kkill(পূর্ণসংখ্যা pid)
{
  কাঠামো proc *p;

  যদি (pid == 0)
    ফেরত -1;

  জন্য (p = proc; p < &proc[NPROC]; p++) {
    acquire(&p->lock);
    যদি (p->pid == pid) {
      p->killed = 1;
      যদি (p->state == SLEEPING) {
        // Wake process from sleep().
        p->state = RUNNABLE;
      }
      release(&p->lock);
      ফেরত 0;
    }
    release(&p->lock);
  }
  ফেরত -1;
}

শূন্য
setkilled(কাঠামো proc *p)
{
  acquire(&p->lock);
  p->killed = 1;
  release(&p->lock);
}

পূর্ণসংখ্যা
killed(কাঠামো proc *p)
{
  পূর্ণসংখ্যা k;

  acquire(&p->lock);
  k = p->killed;
  release(&p->lock);
  ফেরত k;
}

// Copy to either a user address, or kernel address,
// depending on usr_dst.
// Returns 0 on success, -1 on error.
পূর্ণসংখ্যা
either_copyout(পূর্ণসংখ্যা user_dst, uint64 dst, শূন্য *src, uint64 len)
{
  কাঠামো proc *p = myproc();
  যদি (user_dst) {
    ফেরত copyout(p->pagetable, p->sz, dst, src, len);
  } অন্যথা {
    memmove((অক্ষর *)dst, src, len);
    ফেরত 0;
  }
}

// Copy from either a user address, or kernel address,
// depending on usr_src.
// Returns 0 on success, -1 on error.
পূর্ণসংখ্যা
either_copyin(শূন্য *dst, পূর্ণসংখ্যা user_src, uint64 src, uint64 len)
{
  কাঠামো proc *p = myproc();
  যদি (user_src) {
    ফেরত copyin(p->pagetable, p->sz, dst, src, len);
  } অন্যথা {
    memmove(dst, (অক্ষর *)src, len);
    ফেরত 0;
  }
}

// Print a process listing to console.  For debugging.
// Runs when user types ^P on console.
// No lock to avoid wedging a stuck machine further.
শূন্য
procdump(শূন্য)
{
  স্থির অক্ষর *states[] = {
    // clang-format off
    [UNUSED]    = "unused",
    [USED]      = "used",
    [SLEEPING]  = "sleep ",
    [RUNNABLE]  = "runble",
    [RUNNING]   = "run   ",
    [ZOMBIE]    = "zombie"
    // clang-format on
  };
  কাঠামো proc *p;
  অক্ষর *state;

  printk("\n");
  জন্য (p = proc; p < &proc[NPROC]; p++) {
    যদি (p->state == UNUSED)
      চালু;
    যদি (p->state >= 0 && p->state < NELEM(states) && states[p->state])
      state = states[p->state];
    অন্যথা
      state = "???";
    printk("%d %s %s", p->pid, state, p->name);
    printk("\n");
  }
}
