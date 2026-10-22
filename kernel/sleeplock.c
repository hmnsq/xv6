// Sleeping locks

#شامل "types.h"
#شامل "riscv.h"
#شامل "defs.h"
#شامل "param.h"
#شامل "memlayout.h"
#شامل "spinlock.h"
#شامل "proc.h"
#شامل "sleeplock.h"

تهی
initsleeplock(ساختار sleeplock *lk, نویسه *name)
{
  initlock(&lk->lk, "sleep lock");
  lk->name = name;
  lk->locked = 0;
  lk->pid = 0;
}

تهی
acquiresleep(ساختار sleeplock *lk)
{
  acquire(&lk->lk);
  وقتی (lk->locked) {
    sleep_prepare(lk);
    release(&lk->lk);
    sleep();
    acquire(&lk->lk);
  }
  lk->locked = 1;
  lk->pid = myproc()->pid;
  release(&lk->lk);
}

تهی
releasesleep(ساختار sleeplock *lk)
{
  acquire(&lk->lk);
  lk->locked = 0;
  lk->pid = 0;
  wakeup(lk);
  release(&lk->lk);
}

صحیح
holdingsleep(ساختار sleeplock *lk)
{
  صحیح r;

  acquire(&lk->lk);
  r = lk->locked && (lk->pid == myproc()->pid);
  release(&lk->lk);
  بازگشت r;
}
