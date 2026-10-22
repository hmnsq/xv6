// Sleeping locks

#ଅନ୍ତର୍ଭାଵ "types.h"
#ଅନ୍ତର୍ଭାଵ "riscv.h"
#ଅନ୍ତର୍ଭାଵ "defs.h"
#ଅନ୍ତର୍ଭାଵ "param.h"
#ଅନ୍ତର୍ଭାଵ "memlayout.h"
#ଅନ୍ତର୍ଭାଵ "spinlock.h"
#ଅନ୍ତର୍ଭାଵ "proc.h"
#ଅନ୍ତର୍ଭାଵ "sleeplock.h"

ଶୂନ୍ୟ
initsleeplock(ସଂରଚନା sleeplock *lk, ଅକ୍ଷର *name)
{
  initlock(&lk->lk, "sleep lock");
  lk->name = name;
  lk->locked = 0;
  lk->pid = 0;
}

ଶୂନ୍ୟ
acquiresleep(ସଂରଚନା sleeplock *lk)
{
  acquire(&lk->lk);
  ଯେତେବେଳେ (lk->locked) {
    sleep_prepare(lk);
    release(&lk->lk);
    sleep();
    acquire(&lk->lk);
  }
  lk->locked = 1;
  lk->pid = myproc()->pid;
  release(&lk->lk);
}

ଶୂନ୍ୟ
releasesleep(ସଂରଚନା sleeplock *lk)
{
  acquire(&lk->lk);
  lk->locked = 0;
  lk->pid = 0;
  wakeup(lk);
  release(&lk->lk);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା
holdingsleep(ସଂରଚନା sleeplock *lk)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

  acquire(&lk->lk);
  r = lk->locked && (lk->pid == myproc()->pid);
  release(&lk->lk);
  ଫେରନ୍ତୁ r;
}
