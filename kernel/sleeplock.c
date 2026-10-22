// Sleeping locks

#અન્તર્ભાવ "types.h"
#અન્તર્ભાવ "riscv.h"
#અન્તર્ભાવ "defs.h"
#અન્તર્ભાવ "param.h"
#અન્તર્ભાવ "memlayout.h"
#અન્તર્ભાવ "spinlock.h"
#અન્તર્ભાવ "proc.h"
#અન્તર્ભાવ "sleeplock.h"

શૂન્ય
initsleeplock(સંરચના sleeplock *lk, અક્ષર *name)
{
  initlock(&lk->lk, "sleep lock");
  lk->name = name;
  lk->locked = 0;
  lk->pid = 0;
}

શૂન્ય
acquiresleep(સંરચના sleeplock *lk)
{
  acquire(&lk->lk);
  જ્યારે (lk->locked) {
    sleep_prepare(lk);
    release(&lk->lk);
    sleep();
    acquire(&lk->lk);
  }
  lk->locked = 1;
  lk->pid = myproc()->pid;
  release(&lk->lk);
}

શૂન્ય
releasesleep(સંરચના sleeplock *lk)
{
  acquire(&lk->lk);
  lk->locked = 0;
  lk->pid = 0;
  wakeup(lk);
  release(&lk->lk);
}

પૂર્ણાંક
holdingsleep(સંરચના sleeplock *lk)
{
  પૂર્ણાંક r;

  acquire(&lk->lk);
  r = lk->locked && (lk->pid == myproc()->pid);
  release(&lk->lk);
  પાછા r;
}
