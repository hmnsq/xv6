// Sleeping locks

#অন্তর্ভাৱ "types.h"
#অন্তর্ভাৱ "riscv.h"
#অন্তর্ভাৱ "defs.h"
#অন্তর্ভাৱ "param.h"
#অন্তর্ভাৱ "memlayout.h"
#অন্তর্ভাৱ "spinlock.h"
#অন্তর্ভাৱ "proc.h"
#অন্তর্ভাৱ "sleeplock.h"

শূন্য
initsleeplock(সংরচনা sleeplock *lk, ৱর্ণ *name)
{
  initlock(&lk->lk, "sleep lock");
  lk->name = name;
  lk->locked = 0;
  lk->pid = 0;
}

শূন্য
acquiresleep(সংরচনা sleeplock *lk)
{
  acquire(&lk->lk);
  যেতিয়ালৈ (lk->locked) {
    sleep_prepare(lk);
    release(&lk->lk);
    sleep();
    acquire(&lk->lk);
  }
  lk->locked = 1;
  lk->pid = myproc()->pid;
  release(&lk->lk);
}

শূন্য
releasesleep(সংরচনা sleeplock *lk)
{
  acquire(&lk->lk);
  lk->locked = 0;
  lk->pid = 0;
  wakeup(lk);
  release(&lk->lk);
}

পূর্ণাঙ্ক
holdingsleep(সংরচনা sleeplock *lk)
{
  পূর্ণাঙ্ক r;

  acquire(&lk->lk);
  r = lk->locked && (lk->pid == myproc()->pid);
  release(&lk->lk);
  ঘূৰাই r;
}
