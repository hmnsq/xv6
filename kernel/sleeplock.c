// Sleeping locks

#অন্তর্ভাব "types.h"
#অন্তর্ভাব "riscv.h"
#অন্তর্ভাব "defs.h"
#অন্তর্ভাব "param.h"
#অন্তর্ভাব "memlayout.h"
#অন্তর্ভাব "spinlock.h"
#অন্তর্ভাব "proc.h"
#অন্তর্ভাব "sleeplock.h"

শূন্য
initsleeplock(সংরচনা sleeplock *lk, বর্ণ *name)
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
  যাবত্ (lk->locked) {
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
  প্রত্যাগম r;
}
