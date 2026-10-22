// Mutual exclusion spin locks.

#ಅನ್ತರ್ಭಾವ "types.h"
#ಅನ್ತರ್ಭಾವ "param.h"
#ಅನ್ತರ್ಭಾವ "memlayout.h"
#ಅನ್ತರ್ಭಾವ "spinlock.h"
#ಅನ್ತರ್ಭಾವ "riscv.h"
#ಅನ್ತರ್ಭಾವ "proc.h"
#ಅನ್ತರ್ಭಾವ "defs.h"

ಶೂನ್ಯ
initlock(ಸಂರಚನಾ spinlock *lk, ವರ್ಣ *name)
{
  lk->name = name;
  lk->locked = 0;
  lk->cpu = 0;
}

// Acquire the lock.
// Loops (spins) until the lock is acquired.
ಶೂನ್ಯ
acquire(ಸಂರಚನಾ spinlock *lk)
{
  ತಳ್ಳು_off(); // disable interrupts to avoid deadlock.
  ಯದಿ (holding(lk))
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
  ಯಾವತ್ (__atomic_exchange_n(&lk->locked, 1, __ATOMIC_ACQUIRE) != 0)
    ;

  // Record info about lock acquisition for holding() and debugging.
  lk->cpu = mycpu();
}

// Release the lock.
ಶೂನ್ಯ
release(ಸಂರಚನಾ spinlock *lk)
{
  ಯದಿ (!holding(lk))
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

  ತೆಗೆ_off();
}

// Check whether this cpu is holding the lock.
// Interrupts must be off.
ಪೂರ್ಣಾಙ್ಕ
holding(ಸಂರಚನಾ spinlock *lk)
{
  ಪೂರ್ಣಾಙ್ಕ r;
  r = (lk->locked && lk->cpu == mycpu());
  ಪ್ರತ್ಯಾಗಮ r;
}

// push_off/pop_off are like intr_off()/intr_on() except that they are matched:
// it takes two pop_off()s to undo two push_off()s.  Also, if interrupts
// are initially off, then push_off, pop_off leaves them off.

ಶೂನ್ಯ
ತಳ್ಳು_off(ಶೂನ್ಯ)
{
  // disable interrupts to prevent an involuntary context
  // switch while using mycpu().
  uint64 flags = rc_sstatus(SSTATUS_SIE);
  ಪೂರ್ಣಾಙ್ಕ old = !!(flags & SSTATUS_SIE);

  ಯದಿ (mycpu()->noff == 0)
    mycpu()->intena = old;
  mycpu()->noff += 1;
}

ಶೂನ್ಯ
ತೆಗೆ_off(ಶೂನ್ಯ)
{
  ಸಂರಚನಾ cpu *c = mycpu();
  ಯದಿ (intr_get())
    panic("pop_off - interruptible");
  ಯದಿ (c->noff < 1)
    panic("pop_off");
  c->noff -= 1;
  ಯದಿ (c->noff == 0 && c->intena)
    intr_on();
}
