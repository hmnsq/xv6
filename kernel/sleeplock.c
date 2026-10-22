// Sleeping locks

#அந்தர்பாவ "types.h"
#அந்தர்பாவ "riscv.h"
#அந்தர்பாவ "defs.h"
#அந்தர்பாவ "param.h"
#அந்தர்பாவ "memlayout.h"
#அந்தர்பாவ "spinlock.h"
#அந்தர்பாவ "proc.h"
#அந்தர்பாவ "sleeplock.h"

வெறுமை
initsleeplock(கட்டமைப்பு sleeplock *lk, எழுத்து *name)
{
  initlock(&lk->lk, "sleep lock");
  lk->name = name;
  lk->locked = 0;
  lk->pid = 0;
}

வெறுமை
acquiresleep(கட்டமைப்பு sleeplock *lk)
{
  acquire(&lk->lk);
  போது (lk->locked) {
    sleep_prepare(lk);
    release(&lk->lk);
    sleep();
    acquire(&lk->lk);
  }
  lk->locked = 1;
  lk->pid = myproc()->pid;
  release(&lk->lk);
}

வெறுமை
releasesleep(கட்டமைப்பு sleeplock *lk)
{
  acquire(&lk->lk);
  lk->locked = 0;
  lk->pid = 0;
  wakeup(lk);
  release(&lk->lk);
}

முழு
holdingsleep(கட்டமைப்பு sleeplock *lk)
{
  முழு r;

  acquire(&lk->lk);
  r = lk->locked && (lk->pid == myproc()->pid);
  release(&lk->lk);
  திரும்பு r;
}
