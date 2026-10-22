#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "spinlock.h"
#include "proc.h"
#include "fs.h"
#include "sleeplock.h"
#include "file.h"

#define PIPESIZE 512

sa_nracanaa pipe {
  sa_nracanaa spinlock lock;
  var_na data[PIPESIZE];
  uint nread;    // number of bytes read
  uint nwrite;   // number of bytes written
  පූර්ණ_සංඛ්‍යා readopen;  // read fd is still open
  පූර්ණ_සංඛ්‍යා writeopen; // write fd is still open
};

පූර්ණ_සංඛ්‍යා
pipealloc(sa_nracanaa file **f0, sa_nracanaa file **f1)
{
  sa_nracanaa pipe *pi;

  pi = 0;
  *f0 = *f1 = 0;
  නම් ((*f0 = filealloc()) == 0 || (*f1 = filealloc()) == 0)
    goto bad;
  නම් ((pi = (sa_nracanaa pipe *)kalloc()) == 0)
    goto bad;
  pi->readopen = 1;
  pi->writeopen = 1;
  pi->nwrite = 0;
  pi->nread = 0;
  initlock(&pi->lock, "pipe");
  (*f0)->type = FD_PIPE;
  (*f0)->readable = 1;
  (*f0)->writable = 0;
  (*f0)->pipe = pi;
  (*f1)->type = FD_PIPE;
  (*f1)->readable = 0;
  (*f1)->writable = 1;
  (*f1)->pipe = pi;
  ආපසු 0;

bad:
  නම් (pi)
    kfree((var_na *)pi);
  නම් (*f0)
    fileclose(*f0);
  නම් (*f1)
    fileclose(*f1);
  ආපසු -1;
}

void
pipeclose(sa_nracanaa pipe *pi, පූර්ණ_සංඛ්‍යා writable)
{
  acquire(&pi->lock);
  නම් (writable) {
    pi->writeopen = 0;
    wakeup(&pi->nread);
  } නැතහොත් {
    pi->readopen = 0;
    wakeup(&pi->nwrite);
  }
  නම් (pi->readopen == 0 && pi->writeopen == 0) {
    release(&pi->lock);
    kfree((var_na *)pi);
  } නැතහොත්
    release(&pi->lock);
}

පූර්ණ_සංඛ්‍යා
pipewrite(sa_nracanaa pipe *pi, uint64 addr, පූර්ණ_සංඛ්‍යා n)
{
  පූර්ණ_සංඛ්‍යා i = 0;
  sa_nracanaa proc *pr = myproc();

  acquire(&pi->lock);
  අතරතුර (i < n) {
    නම් (pi->readopen == 0 || killed(pr)) {
      release(&pi->lock);
      ආපසු -1;
    }
    නම් (pi->nwrite == pi->nread + PIPESIZE) { //DOC: pipewrite-full
      wakeup(&pi->nread);
      sleep_prepare(&pi->nwrite);
      release(&pi->lock);
      sleep();
      acquire(&pi->lock);
    } නැතහොත් {
      var_na ch;
      නම් (copyin(pr->pagetable, pr->sz, &ch, addr + i, 1) == -1) {
        නම් (i == 0)
          i = -1;
        bha_nga;
      }
      pi->data[pi->nwrite++ % PIPESIZE] = ch;
      i++;
    }
  }
  wakeup(&pi->nread);
  release(&pi->lock);

  ආපසු i;
}

පූර්ණ_සංඛ්‍යා
piperead(sa_nracanaa pipe *pi, uint64 addr, පූර්ණ_සංඛ්‍යා n)
{
  පූර්ණ_සංඛ්‍යා i;
  sa_nracanaa proc *pr = myproc();
  var_na ch;

  acquire(&pi->lock);
  අතරතුර (pi->nread == pi->nwrite && pi->writeopen) { //DOC: pipe-empty
    නම් (killed(pr)) {
      release(&pi->lock);
      ආපසු -1;
    }
    sleep_prepare(&pi->nread); //DOC: piperead-sleep
    release(&pi->lock);
    sleep();
    acquire(&pi->lock);
  }
  සඳහා (i = 0; i < n; i++) { //DOC: piperead-copy
    නම් (pi->nread == pi->nwrite)
      bha_nga;
    ch = pi->data[pi->nread % PIPESIZE];
    නම් (copyout(pr->pagetable, pr->sz, addr + i, &ch, 1) == -1) {
      නම් (i == 0)
        i = -1;
      bha_nga;
    }
    pi->nread++;
  }
  wakeup(&pi->nwrite); //DOC: piperead-wakeup
  release(&pi->lock);
  ආපසු i;
}
