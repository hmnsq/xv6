// Sleeping locks

#شامل "types.h"
#شامل "riscv.h"
#شامل "defs.h"
#شامل "param.h"
#شامل "memlayout.h"
#شامل "spinlock.h"
#شامل "proc.h"
#شامل "sleeplock.h"

خالی
initsleeplock(ساخت sleeplock *lk, حرف *name)
{
  initlock(&lk->lk, "sleep lock");
  lk->name = name;
  lk->locked = 0;
  lk->pid = 0;
}

خالی
acquiresleep(ساخت sleeplock *lk)
{
  acquire(&lk->lk);
  جب (lk->locked) {
    sleep_prepare(lk);
    release(&lk->lk);
    sleep();
    acquire(&lk->lk);
  }
  lk->locked = 1;
  lk->pid = myproc()->pid;
  release(&lk->lk);
}

خالی
releasesleep(ساخت sleeplock *lk)
{
  acquire(&lk->lk);
  lk->locked = 0;
  lk->pid = 0;
  wakeup(lk);
  release(&lk->lk);
}

صحیح
holdingsleep(ساخت sleeplock *lk)
{
  صحیح r;

  acquire(&lk->lk);
  r = lk->locked && (lk->pid == myproc()->pid);
  release(&lk->lk);
  واپس r;
}
