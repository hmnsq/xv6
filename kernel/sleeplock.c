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
initsleeplock(কাঠামো sleeplock *lk, অক্ষর *name)
{
  initlock(&lk->lk, "sleep lock");
  lk->name = name;
  lk->locked = 0;
  lk->pid = 0;
}

শূন্য
acquiresleep(কাঠামো sleeplock *lk)
{
  acquire(&lk->lk);
  যখন (lk->locked) {
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
releasesleep(কাঠামো sleeplock *lk)
{
  acquire(&lk->lk);
  lk->locked = 0;
  lk->pid = 0;
  wakeup(lk);
  release(&lk->lk);
}

পূর্ণসংখ্যা
holdingsleep(কাঠামো sleeplock *lk)
{
  পূর্ণসংখ্যা r;

  acquire(&lk->lk);
  r = lk->locked && (lk->pid == myproc()->pid);
  release(&lk->lk);
  ফেরত r;
}
