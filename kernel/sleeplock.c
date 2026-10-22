// Sleeping locks

#ಅನ್ತರ್ಭಾವ "types.h"
#ಅನ್ತರ್ಭಾವ "riscv.h"
#ಅನ್ತರ್ಭಾವ "defs.h"
#ಅನ್ತರ್ಭಾವ "param.h"
#ಅನ್ತರ್ಭಾವ "memlayout.h"
#ಅನ್ತರ್ಭಾವ "spinlock.h"
#ಅನ್ತರ್ಭಾವ "proc.h"
#ಅನ್ತರ್ಭಾವ "sleeplock.h"

ಶೂನ್ಯ
initsleeplock(ರಚನೆ sleeplock *lk, ಅಕ್ಷರ *name)
{
  initlock(&lk->lk, "sleep lock");
  lk->name = name;
  lk->locked = 0;
  lk->pid = 0;
}

ಶೂನ್ಯ
acquiresleep(ರಚನೆ sleeplock *lk)
{
  acquire(&lk->lk);
  ಆಗ (lk->locked) {
    sleep_prepare(lk);
    release(&lk->lk);
    sleep();
    acquire(&lk->lk);
  }
  lk->locked = 1;
  lk->pid = myproc()->pid;
  release(&lk->lk);
}

ಶೂನ್ಯ
releasesleep(ರಚನೆ sleeplock *lk)
{
  acquire(&lk->lk);
  lk->locked = 0;
  lk->pid = 0;
  wakeup(lk);
  release(&lk->lk);
}

ಪೂರ್ಣಾಂಕ
holdingsleep(ರಚನೆ sleeplock *lk)
{
  ಪೂರ್ಣಾಂಕ r;

  acquire(&lk->lk);
  r = lk->locked && (lk->pid == myproc()->pid);
  release(&lk->lk);
  ಹಿಂತಿರುಗಿ r;
}
