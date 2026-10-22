// Sleeping locks

#ਅਨ੍ਤਰ੍ਭਾਵ "types.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "riscv.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "defs.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "param.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "memlayout.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "spinlock.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "proc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "sleeplock.h"

ਖਾਲੀ
initsleeplock(ਬਣਤਰ sleeplock *lk, ਅੱਖਰ *name)
{
  initlock(&lk->lk, "sleep lock");
  lk->name = name;
  lk->locked = 0;
  lk->pid = 0;
}

ਖਾਲੀ
acquiresleep(ਬਣਤਰ sleeplock *lk)
{
  acquire(&lk->lk);
  ਜਦੋਂ (lk->locked) {
    sleep_prepare(lk);
    release(&lk->lk);
    sleep();
    acquire(&lk->lk);
  }
  lk->locked = 1;
  lk->pid = myproc()->pid;
  release(&lk->lk);
}

ਖਾਲੀ
releasesleep(ਬਣਤਰ sleeplock *lk)
{
  acquire(&lk->lk);
  lk->locked = 0;
  lk->pid = 0;
  wakeup(lk);
  release(&lk->lk);
}

ਪੂਰਨ
holdingsleep(ਬਣਤਰ sleeplock *lk)
{
  ਪੂਰਨ r;

  acquire(&lk->lk);
  r = lk->locked && (lk->pid == myproc()->pid);
  release(&lk->lk);
  ਵਾਪਸ r;
}
