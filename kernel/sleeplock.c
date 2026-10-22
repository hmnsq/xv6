// Sleeping locks

#ضمن "types.h"
#ضمن "riscv.h"
#ضمن "defs.h"
#ضمن "param.h"
#ضمن "memlayout.h"
#ضمن "spinlock.h"
#ضمن "proc.h"
#ضمن "sleeplock.h"

فراغ
initsleeplock(بنية sleeplock *lk, حرف *name)
{
  initlock(&lk->lk, "sleep lock");
  lk->name = name;
  lk->locked = 0;
  lk->pid = 0;
}

فراغ
acquiresleep(بنية sleeplock *lk)
{
  acquire(&lk->lk);
  بينما (lk->locked) {
    sleep_prepare(lk);
    release(&lk->lk);
    sleep();
    acquire(&lk->lk);
  }
  lk->locked = 1;
  lk->pid = myproc()->pid;
  release(&lk->lk);
}

فراغ
releasesleep(بنية sleeplock *lk)
{
  acquire(&lk->lk);
  lk->locked = 0;
  lk->pid = 0;
  wakeup(lk);
  release(&lk->lk);
}

صحيح
holdingsleep(بنية sleeplock *lk)
{
  صحيح r;

  acquire(&lk->lk);
  r = lk->locked && (lk->pid == myproc()->pid);
  release(&lk->lk);
  إرجاع r;
}
