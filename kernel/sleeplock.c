// Sleeping locks

#समावेश "types.h"
#समावेश "riscv.h"
#समावेश "defs.h"
#समावेश "param.h"
#समावेश "memlayout.h"
#समावेश "spinlock.h"
#समावेश "proc.h"
#समावेश "sleeplock.h"

व्योम
initsleeplock(काष्ठा sleeplock *lk, अक्षर *name)
{
  initlock(&lk->lk, "sleep lock");
  lk->name = name;
  lk->locked = 0;
  lk->pid = 0;
}

व्योम
acquiresleep(काष्ठा sleeplock *lk)
{
  acquire(&lk->lk);
  जबतक (lk->locked) {
    sleep_prepare(lk);
    release(&lk->lk);
    sleep();
    acquire(&lk->lk);
  }
  lk->locked = 1;
  lk->pid = myproc()->pid;
  release(&lk->lk);
}

व्योम
releasesleep(काष्ठा sleeplock *lk)
{
  acquire(&lk->lk);
  lk->locked = 0;
  lk->pid = 0;
  wakeup(lk);
  release(&lk->lk);
}

पूर्णांक
holdingsleep(काष्ठा sleeplock *lk)
{
  पूर्णांक r;

  acquire(&lk->lk);
  r = lk->locked && (lk->pid == myproc()->pid);
  release(&lk->lk);
  वापस r;
}
