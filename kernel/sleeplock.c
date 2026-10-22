// Sleeping locks

#അന്തര്ഭാവ "types.h"
#അന്തര്ഭാവ "riscv.h"
#അന്തര്ഭാവ "defs.h"
#അന്തര്ഭാവ "param.h"
#അന്തര്ഭാവ "memlayout.h"
#അന്തര്ഭാവ "spinlock.h"
#അന്തര്ഭാവ "proc.h"
#അന്തര്ഭാവ "sleeplock.h"

ശൂന്യം
initsleeplock(ഘടന sleeplock *lk, അക്ഷരം *name)
{
  initlock(&lk->lk, "sleep lock");
  lk->name = name;
  lk->locked = 0;
  lk->pid = 0;
}

ശൂന്യം
acquiresleep(ഘടന sleeplock *lk)
{
  acquire(&lk->lk);
  ആകുമ്പോൾ (lk->locked) {
    sleep_prepare(lk);
    release(&lk->lk);
    sleep();
    acquire(&lk->lk);
  }
  lk->locked = 1;
  lk->pid = myproc()->pid;
  release(&lk->lk);
}

ശൂന്യം
releasesleep(ഘടന sleeplock *lk)
{
  acquire(&lk->lk);
  lk->locked = 0;
  lk->pid = 0;
  wakeup(lk);
  release(&lk->lk);
}

പൂർണ്ണസംഖ്യ
holdingsleep(ഘടന sleeplock *lk)
{
  പൂർണ്ണസംഖ്യ r;

  acquire(&lk->lk);
  r = lk->locked && (lk->pid == myproc()->pid);
  release(&lk->lk);
  തിരികെ r;
}
