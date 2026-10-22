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
initsleeplock(ಸಂರಚನಾ sleeplock *lk, ವರ್ಣ *name)
{
  initlock(&lk->lk, "sleep lock");
  lk->name = name;
  lk->locked = 0;
  lk->pid = 0;
}

ಶೂನ್ಯ
acquiresleep(ಸಂರಚನಾ sleeplock *lk)
{
  acquire(&lk->lk);
  ಯಾವತ್ (lk->locked) {
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
releasesleep(ಸಂರಚನಾ sleeplock *lk)
{
  acquire(&lk->lk);
  lk->locked = 0;
  lk->pid = 0;
  wakeup(lk);
  release(&lk->lk);
}

ಪೂರ್ಣಾಙ್ಕ
holdingsleep(ಸಂರಚನಾ sleeplock *lk)
{
  ಪೂರ್ಣಾಙ್ಕ r;

  acquire(&lk->lk);
  r = lk->locked && (lk->pid == myproc()->pid);
  release(&lk->lk);
  ಪ್ರತ್ಯಾಗಮ r;
}
