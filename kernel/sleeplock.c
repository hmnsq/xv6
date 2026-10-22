// Sleeping locks

#అన్తర్భావ "types.h"
#అన్తర్భావ "riscv.h"
#అన్తర్భావ "defs.h"
#అన్తర్భావ "param.h"
#అన్తర్భావ "memlayout.h"
#అన్తర్భావ "spinlock.h"
#అన్తర్భావ "proc.h"
#అన్తర్భావ "sleeplock.h"

శూన్య
initsleeplock(నిర్మాణం sleeplock *lk, అక్షరం *name)
{
  initlock(&lk->lk, "sleep lock");
  lk->name = name;
  lk->locked = 0;
  lk->pid = 0;
}

శూన్య
acquiresleep(నిర్మాణం sleeplock *lk)
{
  acquire(&lk->lk);
  అయితే (lk->locked) {
    sleep_prepare(lk);
    release(&lk->lk);
    sleep();
    acquire(&lk->lk);
  }
  lk->locked = 1;
  lk->pid = myproc()->pid;
  release(&lk->lk);
}

శూన్య
releasesleep(నిర్మాణం sleeplock *lk)
{
  acquire(&lk->lk);
  lk->locked = 0;
  lk->pid = 0;
  wakeup(lk);
  release(&lk->lk);
}

పూర్ణాంకం
holdingsleep(నిర్మాణం sleeplock *lk)
{
  పూర్ణాంకం r;

  acquire(&lk->lk);
  r = lk->locked && (lk->pid == myproc()->pid);
  release(&lk->lk);
  తిరిగి r;
}
