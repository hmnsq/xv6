// Mutual exclusion spin locks.

#समावेश "types.h"
#समावेश "param.h"
#समावेश "memlayout.h"
#समावेश "spinlock.h"
#समावेश "riscv.h"
#समावेश "proc.h"
#समावेश "defs.h"

व्योम
initlock(काष्ठा spinlock *lk, अक्षर *name)
{
  lk->name = name;
  lk->locked = 0;
  lk->cpu = 0;
}

// Acquire the lock.
// Loops (spins) until the lock is acquired.
व्योम
acquire(काष्ठा spinlock *lk)
{
  धकेलो_off(); // disable interrupts to avoid deadlock.
  अगर (holding(lk))
    panic("acquire");

  // On RISC-V, __atomic_exchange_n turns into an atomic swap:
  //   a5 = 1
  //   s1 = &lk->locked
  //   amoswap.w.aq a5, a5, (s1)
  //
  // Passing __ATOMIC_ACQUIRE to __atomic_exchange_n tells
  // the C compiler and the processor to not move loads or stores
  // past this point, to ensure that the critical section's memory
  // references happen strictly after the lock is acquired.
  जबतक (__atomic_exchange_n(&lk->locked, 1, __ATOMIC_ACQUIRE) != 0)
    ;

  // Record info about lock acquisition for holding() and debugging.
  lk->cpu = mycpu();
}

// Release the lock.
व्योम
release(काष्ठा spinlock *lk)
{
  अगर (!holding(lk))
    panic("release");

  lk->cpu = 0;

  // Release the lock, equivalent to lk->locked = 0.
  //
  // This code doesn't use a C assignment, since the C standard
  // implies that an assignment might be implemented with
  // multiple store instructions.
  //
  // On RISC-V, __atomic_store_n turns into a single atomic store:
  //   s1 = &lk->locked
  //   fence rw,w
  //   sw zero,0(s1)
  //
  // The __ATOMIC_RELEASE argument to __atomic_store_n tells the
  // the C compiler and the CPU to not move loads or stores past
  // this point, to ensure that all the stores in the critical
  // section are visible to other CPUs before the lock is released,
  // and that loads in the critical section occur strictly before
  // the lock is released.
  //
  // On RISC-V, this generates a fence instruction before the store:
  //   fence rw,w
  __atomic_store_n(&lk->locked, 0, __ATOMIC_RELEASE);

  निकालो_off();
}

// Check whether this cpu is holding the lock.
// Interrupts must be off.
पूर्णांक
holding(काष्ठा spinlock *lk)
{
  पूर्णांक r;
  r = (lk->locked && lk->cpu == mycpu());
  वापस r;
}

// push_off/pop_off are like intr_off()/intr_on() except that they are matched:
// it takes two pop_off()s to undo two push_off()s.  Also, if interrupts
// are initially off, then push_off, pop_off leaves them off.

व्योम
धकेलो_off(व्योम)
{
  // disable interrupts to prevent an involuntary context
  // switch while using mycpu().
  uint64 flags = rc_sstatus(SSTATUS_SIE);
  पूर्णांक old = !!(flags & SSTATUS_SIE);

  अगर (mycpu()->noff == 0)
    mycpu()->intena = old;
  mycpu()->noff += 1;
}

व्योम
निकालो_off(व्योम)
{
  काष्ठा cpu *c = mycpu();
  अगर (intr_get())
    panic("pop_off - interruptible");
  अगर (c->noff < 1)
    panic("pop_off");
  c->noff -= 1;
  अगर (c->noff == 0 && c->intena)
    intr_on();
}
