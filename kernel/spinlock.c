// Mutual exclusion spin locks.

#અન્તર્ભાવ "types.h"
#અન્તર્ભાવ "param.h"
#અન્તર્ભાવ "memlayout.h"
#અન્તર્ભાવ "spinlock.h"
#અન્તર્ભાવ "riscv.h"
#અન્તર્ભાવ "proc.h"
#અન્તર્ભાવ "defs.h"

શૂન્ય
initlock(સંરચના spinlock *lk, અક્ષર *name)
{
  lk->name = name;
  lk->locked = 0;
  lk->cpu = 0;
}

// Acquire the lock.
// Loops (spins) until the lock is acquired.
શૂન્ય
acquire(સંરચના spinlock *lk)
{
  ધકેલો_off(); // disable interrupts to avoid deadlock.
  જો (holding(lk))
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
  જ્યારે (__atomic_exchange_n(&lk->locked, 1, __ATOMIC_ACQUIRE) != 0)
    ;

  // Record info about lock acquisition for holding() and debugging.
  lk->cpu = mycpu();
}

// Release the lock.
શૂન્ય
release(સંરચના spinlock *lk)
{
  જો (!holding(lk))
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

  કાઢો_off();
}

// Check whether this cpu is holding the lock.
// Interrupts must be off.
પૂર્ણાંક
holding(સંરચના spinlock *lk)
{
  પૂર્ણાંક r;
  r = (lk->locked && lk->cpu == mycpu());
  પાછા r;
}

// push_off/pop_off are like intr_off()/intr_on() except that they are matched:
// it takes two pop_off()s to undo two push_off()s.  Also, if interrupts
// are initially off, then push_off, pop_off leaves them off.

શૂન્ય
ધકેલો_off(શૂન્ય)
{
  // disable interrupts to prevent an involuntary context
  // switch while using mycpu().
  uint64 flags = rc_sstatus(SSTATUS_SIE);
  પૂર્ણાંક old = !!(flags & SSTATUS_SIE);

  જો (mycpu()->noff == 0)
    mycpu()->intena = old;
  mycpu()->noff += 1;
}

શૂન્ય
કાઢો_off(શૂન્ય)
{
  સંરચના cpu *c = mycpu();
  જો (intr_get())
    panic("pop_off - interruptible");
  જો (c->noff < 1)
    panic("pop_off");
  c->noff -= 1;
  જો (c->noff == 0 && c->intena)
    intr_on();
}
