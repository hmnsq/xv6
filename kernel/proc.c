#شامل "types.h"
#شامل "param.h"
#شامل "memlayout.h"
#شامل "riscv.h"
#شامل "spinlock.h"
#شامل "proc.h"
#شامل "defs.h"

ساخت cpu cpus[NCPU];

ساخت proc proc[NPROC];

ساخت proc *initproc;

صحیح nextpid = 1;
ساخت spinlock pid_تالا;

خارجی خالی forkret(خالی);
ساکن خالی freeproc(ساخت proc *p);

خارجی حرف trampoline[]; // trampoline.S

// helps ensure that wakeups of wait()ing
// parents are not lost. helps obey the
// memory model when using p->parent.
// must be acquired before any p->lock.
ساخت spinlock wait_تالا;

// Allocate a page for each process's kernel stack.
// Map it high in memory, followed by an invalid
// guard page.
خالی
proc_mapstacks(pagetable_t kpgtbl)
{
  ساخت proc *p;

  برائے (p = proc; p < &proc[NPROC]; p++) {
    حرف *pa = kalloc();
    اگر (pa == 0)
      panic("kalloc");
    uint64 va = KSTACK((صحیح)(p - proc));
    kvmmap(kpgtbl, va, (uint64)pa, PGSIZE, PTE_R | PTE_W);
  }
}

// initialize the proc table.
خالی
procinit(خالی)
{
  ساخت proc *p;

  initlock(&pid_تالا, "nextpid");
  initlock(&wait_تالا, "wait_lock");
  برائے (p = proc; p < &proc[NPROC]; p++) {
    initlock(&p->تالا, "proc");
    p->state = غیر_مستعمل;
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
  واپس id;
}

// Return this CPU's cpu struct.
// Interrupts must be disabled.
ساخت cpu *
mycpu(خالی)
{
  صحیح id = cpuid();
  ساخت cpu *c = &cpus[id];
  واپس c;
}

// Return the current struct proc *, or zero if none.
ساخت proc *
myproc(خالی)
{
  ڈالو_off();
  ساخت cpu *c = mycpu();
  ساخت proc *p = c->proc;
  نکالو_off();
  واپس p;
}

صحیح
allocpid()
{
  صحیح pid;

  acquire(&pid_تالا);
  pid = nextpid;
  nextpid = nextpid + 1;
  release(&pid_تالا);

  واپس pid;
}

// Look in the process table for an UNUSED proc.
// If found, initialize state required to run in the kernel,
// and return with p->lock held.
// If there are no free procs, or a memory allocation fails, return 0.
ساکن ساخت proc *
allocproc(خالی)
{
  ساخت proc *p;

  برائے (p = proc; p < &proc[NPROC]; p++) {
    acquire(&p->تالا);
    اگر (p->state == غیر_مستعمل) {
      جاؤ ملا;
    } ورنہ {
      release(&p->تالا);
    }
  }
  واپس 0;

ملا:
  p->pid = allocpid();
  p->state = مستعمل;

  // Allocate a trapframe page.
  اگر ((p->trapframe = (ساخت trapframe *)kalloc()) == 0) {
    freeproc(p);
    release(&p->تالا);
    واپس 0;
  }

  // An empty user page table.
  p->pagetable = proc_pagetable(p);
  اگر (p->pagetable == 0) {
    freeproc(p);
    release(&p->تالا);
    واپس 0;
  }

  // Set up new context to start executing at forkret,
  // which returns to user space.
  memset(&p->context, 0, حجم(p->context));
  p->context.ra = (uint64)forkret;
  p->context.sp = p->kstack + PGSIZE;

  واپس p;
}

// free a proc structure and the data hanging from it,
// including user pages.
// p->lock must be held.
ساکن خالی
freeproc(ساخت proc *p)
{
  اگر (p->trapframe)
    kfree((خالی *)p->trapframe);
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
  p->state = غیر_مستعمل;
}

// Create a user page table for a given process, with no user memory,
// but with trampoline and trapframe pages.
pagetable_t
proc_pagetable(ساخت proc *p)
{
  pagetable_t pagetable;

  // An empty page table.
  pagetable = uvmcreate();
  اگر (pagetable == 0)
    واپس 0;

  // map the trampoline code (for system call return)
  // at the highest user virtual address.
  // only the supervisor uses it, on the way
  // to/from user space, so not PTE_U.
  اگر (mappages(pagetable, TRAMPOLINE, PGSIZE, (uint64)trampoline,
               PTE_R | PTE_X) < 0) {
    uvmfree(pagetable, 0);
    واپس 0;
  }

  // map the trapframe page just below the trampoline page, for
  // trampoline.S.
  اگر (mappages(pagetable, TRAPFRAME, PGSIZE, (uint64)(p->trapframe),
               PTE_R | PTE_W) < 0) {
    uvmunmap(pagetable, TRAMPOLINE, 1, 0);
    uvmfree(pagetable, 0);
    واپس 0;
  }

  واپس pagetable;
}

// Free a process's page table, and free the
// physical memory it refers to.
خالی
proc_freepagetable(pagetable_t pagetable, uint64 sz)
{
  uvmunmap(pagetable, TRAMPOLINE, 1, 0);
  uvmunmap(pagetable, TRAPFRAME, 1, 0);
  uvmfree(pagetable, sz);
}

// Set up first user process.
خالی
userinit(خالی)
{
  ساخت proc *p;

  p = allocproc();
  initproc = p;

  p->cwd = namei("/");

  p->state = RUNNABLE;

  release(&p->تالا);
}

// Grow or shrink user memory by n bytes.
// Return 0 on success, -1 on failure.
صحیح
growproc(صحیح n)
{
  uint64 sz;
  ساخت proc *p = myproc();

  sz = p->sz;
  اگر (n > 0) {
    اگر (sz + n > TRAPFRAME) {
      واپس -1;
    }
    اگر ((sz = uvmalloc(p->pagetable, sz, sz + n, PTE_W)) == 0) {
      واپس -1;
    }
  } ورنہ اگر (n < 0) {
    sz = uvmdealloc(p->pagetable, sz, sz + n);
  }
  p->sz = sz;
  واپس 0;
}

// Create a new process, copying the parent.
// Sets up child kernel stack to return as if from fork() system call.
صحیح
kfork(خالی)
{
  صحیح i, pid;
  ساخت proc *np;
  ساخت proc *p = myproc();

  // Allocate process.
  اگر ((np = allocproc()) == 0) {
    واپس -1;
  }

  // Copy user memory from parent to child.
  اگر (uvmcopy(p->pagetable, np->pagetable, p->sz) < 0) {
    freeproc(np);
    release(&np->تالا);
    واپس -1;
  }
  np->sz = p->sz;

  // copy saved user registers.
  *(np->trapframe) = *(p->trapframe);

  // Cause fork to return 0 in the child.
  np->trapframe->a0 = 0;

  // increment reference counts on open file descriptors.
  برائے (i = 0; i < NOFILE; i++)
    اگر (p->ofile[i])
      np->ofile[i] = filedup(p->ofile[i]);
  np->cwd = idup(p->cwd);

  safestrcpy(np->name, p->name, حجم(p->name));

  pid = np->pid;

  release(&np->تالا);

  acquire(&wait_تالا);
  np->والد = p;
  release(&wait_تالا);

  acquire(&np->تالا);
  np->state = RUNNABLE;
  release(&np->تالا);

  واپس pid;
}

// Pass p's abandoned children to init.
// Caller must hold wait_lock.
خالی
reparent(ساخت proc *p)
{
  ساخت proc *pp;

  برائے (pp = proc; pp < &proc[NPROC]; pp++) {
    اگر (pp->والد == p) {
      pp->والد = initproc;
      wakeup(initproc);
    }
  }
}

// Exit the current process.  Does not return.
// An exited process remains in the zombie state
// until its parent calls wait().
خالی
kexit(صحیح حالت_2)
{
  ساخت proc *p = myproc();

  اگر (p == initproc)
    panic("init exiting");

  // Close all open files.
  برائے (صحیح fd = 0; fd < NOFILE; fd++) {
    اگر (p->ofile[fd]) {
      ساخت file *f = p->ofile[fd];
      fileclose(f);
      p->ofile[fd] = 0;
    }
  }

  begin_op();
  iput(p->cwd);
  انتہا_op();
  p->cwd = 0;

  acquire(&wait_تالا);

  // Give any children to init.
  reparent(p);

  // Parent might be sleeping in wait().
  wakeup(p->والد);

  acquire(&p->تالا);

  p->xstate = حالت_2;
  p->state = ZOMBIE;

  release(&wait_تالا);

  // Jump into the scheduler, never to return.
  sched();
  panic("zombie exit");
}

// Wait for a child process to exit and return its pid.
// Return -1 if this process has no children.
صحیح
kwait(uint64 addr)
{
  ساخت proc *pp;
  صحیح havekids, pid;
  ساخت proc *p = myproc();

  acquire(&wait_تالا);

  برائے (;;) {
    // Scan through table looking for exited children.
    havekids = 0;
    برائے (pp = proc; pp < &proc[NPROC]; pp++) {
      اگر (pp->والد == p) {
        // make sure the child isn't still in exit() or swtch().
        acquire(&pp->تالا);

        havekids = 1;
        اگر (pp->state == ZOMBIE) {
          // Found one.
          pid = pp->pid;
          اگر (addr != 0 &&
              copyout(p->pagetable, p->sz, addr, (حرف *)&pp->xstate,
                      حجم(pp->xstate)) < 0) {
            release(&pp->تالا);
            release(&wait_تالا);
            واپس -1;
          }
          pp->والد = 0;
          freeproc(pp);
          release(&pp->تالا);
          release(&wait_تالا);
          واپس pid;
        }
        release(&pp->تالا);
      }
    }

    // No point waiting if we don't have any children.
    اگر (!havekids || killed(p)) {
      release(&wait_تالا);
      واپس -1;
    }

    // Wait for a child to exit.
    sleep_prepare(p); //DOC: wait-sleep
    release(&wait_تالا);
    sleep();
    acquire(&wait_تالا);
  }
}

// Per-CPU process scheduler.
// Each CPU calls scheduler() after setting itself up.
// Scheduler never returns.  It loops, doing:
//  - choose a process to run.
//  - swtch to start running that process.
//  - eventually that process transfers control
//    via swtch back to the scheduler.
خالی
scheduler(خالی)
{
  ساخت proc *p;
  ساخت cpu *c = mycpu();

  c->proc = 0;
  برائے (;;) {
    // The most recent process to run may have had interrupts
    // turned off; enable them to avoid a deadlock if all
    // processes are waiting. Then turn them back off
    // to avoid a possible race between an interrupt
    // and wfi.
    intr_on();
    intr_off();

    صحیح ملا = 0;
    برائے (p = proc; p < &proc[NPROC]; p++) {
      acquire(&p->تالا);
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
        ملا = 1;
      }
      release(&p->تالا);
    }
    اگر (ملا == 0) {
      // nothing to run; stop running on this core until an interrupt.
      asm متلون("wfi");
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
خالی
sched(خالی)
{
  صحیح intena;
  ساخت proc *p = myproc();

  اگر (!holding(&p->تالا))
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
خالی
yield(خالی)
{
  ساخت proc *p = myproc();
  acquire(&p->تالا);
  p->state = RUNNABLE;
  sched();
  release(&p->تالا);
}

// A fork child's very first scheduling by scheduler()
// will swtch to forkret.
خالی
forkret(خالی)
{
  خارجی حرف userret[];
  ساکن صحیح پہلا = 1;
  ساخت proc *p = myproc();

  // Still holding p->lock from scheduler.
  release(&p->تالا);

  اگر (پہلا) {
    پہلا = 0;

    // File system initialization must be run in the context of a
    // regular process (e.g., because it calls sleep), and thus cannot
    // be run from main().
    fsinit(ROOTDEV);

    // We can invoke kexec() now that file system is initialized.
    // Put the return value (argc) of kexec into a0.
    p->trapframe->a0 = kexec("/init", (حرف *[]){"/init", 0});
    اگر (p->trapframe->a0 == -1) {
      panic("exec");
    }
  }

  // return to user space, mimicing usertrap()'s return.
  prepare_return();
  uint64 satp = MAKE_SATP(p->pagetable);
  uint64 trampoline_userret = TRAMPOLINE + (userret - trampoline);
  ((خالی (*)(uint64))trampoline_userret)(satp);
}

// Register current process as waiting for wakeups on chan.
خالی
sleep_prepare(خالی *chan)
{
  ساخت proc *p = myproc();

  acquire(&p->تالا);
  اگر (chan == 0)
    panic("sleep_prepare: zero chan");
  p->chan = chan;
  release(&p->تالا);
}

// Put the thread to sleep.  Assumes sleep_prepare() was called before.
// If the channel registered by sleep_prepare() has been woken up in
// the meantime, do not go to sleep, and instead return immediately.
خالی
sleep(خالی)
{
  ساخت proc *p = myproc();

  acquire(&p->تالا);
  اگر (p->chan != 0) {
    p->state = SLEEPING;
    sched();
  }
  release(&p->تالا);
}

// Wake up all processes sleeping on channel chan.
خالی
wakeup(خالی *chan)
{
  ساخت proc *p;

  برائے (p = proc; p < &proc[NPROC]; p++) {
    acquire(&p->تالا);
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
    release(&p->تالا);
  }
}

// Kill the process with the given pid.
// The victim won't exit until it tries to return
// to user space (see usertrap() in trap.c).
صحیح
kkill(صحیح pid)
{
  ساخت proc *p;

  اگر (pid == 0)
    واپس -1;

  برائے (p = proc; p < &proc[NPROC]; p++) {
    acquire(&p->تالا);
    اگر (p->pid == pid) {
      p->killed = 1;
      اگر (p->state == SLEEPING) {
        // Wake process from sleep().
        p->state = RUNNABLE;
      }
      release(&p->تالا);
      واپس 0;
    }
    release(&p->تالا);
  }
  واپس -1;
}

خالی
setkilled(ساخت proc *p)
{
  acquire(&p->تالا);
  p->killed = 1;
  release(&p->تالا);
}

صحیح
killed(ساخت proc *p)
{
  صحیح k;

  acquire(&p->تالا);
  k = p->killed;
  release(&p->تالا);
  واپس k;
}

// Copy to either a user address, or kernel address,
// depending on usr_dst.
// Returns 0 on success, -1 on error.
صحیح
either_copyout(صحیح user_dst, uint64 dst, خالی *src, uint64 len)
{
  ساخت proc *p = myproc();
  اگر (user_dst) {
    واپس copyout(p->pagetable, p->sz, dst, src, len);
  } ورنہ {
    memmove((حرف *)dst, src, len);
    واپس 0;
  }
}

// Copy from either a user address, or kernel address,
// depending on usr_src.
// Returns 0 on success, -1 on error.
صحیح
either_copyin(خالی *dst, صحیح user_src, uint64 src, uint64 len)
{
  ساخت proc *p = myproc();
  اگر (user_src) {
    واپس copyin(p->pagetable, p->sz, dst, src, len);
  } ورنہ {
    memmove(dst, (حرف *)src, len);
    واپس 0;
  }
}

// Print a process listing to console.  For debugging.
// Runs when user types ^P on console.
// No lock to avoid wedging a stuck machine further.
خالی
procdump(خالی)
{
  ساکن حرف *حالتیں[] = {
    // clang-format off
    [غیر_مستعمل]    = "unused",
    [مستعمل]      = "used",
    [SLEEPING]  = "sleep ",
    [RUNNABLE]  = "runble",
    [RUNNING]   = "run   ",
    [ZOMBIE]    = "zombie"
    // clang-format on
  };
  ساخت proc *p;
  حرف *state;

  printk("\n");
  برائے (p = proc; p < &proc[NPROC]; p++) {
    اگر (p->state == غیر_مستعمل)
      جاری;
    اگر (p->state >= 0 && p->state < NELEM(حالتیں) && حالتیں[p->state])
      state = حالتیں[p->state];
    ورنہ
      state = "???";
    printk("%d %s %s", p->pid, state, p->name);
    printk("\n");
  }
}
