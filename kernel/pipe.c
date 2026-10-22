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

struct pipe {
  struct spinlock lock;
  char data[PIPESIZE];
  uint nread;    // number of bytes read
  uint nwrite;   // number of bytes written
  số_nguyên readopen;  // read fd is still open
  số_nguyên writeopen; // write fd is still open
};

số_nguyên
pipealloc(struct file **f0, struct file **f1)
{
  struct pipe *pi;

  pi = 0;
  *f0 = *f1 = 0;
  nếu ((*f0 = filealloc()) == 0 || (*f1 = filealloc()) == 0)
    goto bad;
  nếu ((pi = (struct pipe *)kalloc()) == 0)
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
  trả_về 0;

bad:
  nếu (pi)
    kfree((char *)pi);
  nếu (*f0)
    fileclose(*f0);
  nếu (*f1)
    fileclose(*f1);
  trả_về -1;
}

void
pipeclose(struct pipe *pi, số_nguyên writable)
{
  acquire(&pi->lock);
  nếu (writable) {
    pi->writeopen = 0;
    wakeup(&pi->nread);
  } không_thì {
    pi->readopen = 0;
    wakeup(&pi->nwrite);
  }
  nếu (pi->readopen == 0 && pi->writeopen == 0) {
    release(&pi->lock);
    kfree((char *)pi);
  } không_thì
    release(&pi->lock);
}

số_nguyên
pipewrite(struct pipe *pi, uint64 addr, số_nguyên n)
{
  số_nguyên i = 0;
  struct proc *pr = myproc();

  acquire(&pi->lock);
  trong_khi (i < n) {
    nếu (pi->readopen == 0 || killed(pr)) {
      release(&pi->lock);
      trả_về -1;
    }
    nếu (pi->nwrite == pi->nread + PIPESIZE) { //DOC: pipewrite-full
      wakeup(&pi->nread);
      sleep_prepare(&pi->nwrite);
      release(&pi->lock);
      sleep();
      acquire(&pi->lock);
    } không_thì {
      char ch;
      nếu (copyin(pr->pagetable, pr->sz, &ch, addr + i, 1) == -1) {
        nếu (i == 0)
          i = -1;
        break;
      }
      pi->data[pi->nwrite++ % PIPESIZE] = ch;
      i++;
    }
  }
  wakeup(&pi->nread);
  release(&pi->lock);

  trả_về i;
}

số_nguyên
piperead(struct pipe *pi, uint64 addr, số_nguyên n)
{
  số_nguyên i;
  struct proc *pr = myproc();
  char ch;

  acquire(&pi->lock);
  trong_khi (pi->nread == pi->nwrite && pi->writeopen) { //DOC: pipe-empty
    nếu (killed(pr)) {
      release(&pi->lock);
      trả_về -1;
    }
    sleep_prepare(&pi->nread); //DOC: piperead-sleep
    release(&pi->lock);
    sleep();
    acquire(&pi->lock);
  }
  cho (i = 0; i < n; i++) { //DOC: piperead-copy
    nếu (pi->nread == pi->nwrite)
      break;
    ch = pi->data[pi->nread % PIPESIZE];
    nếu (copyout(pr->pagetable, pr->sz, addr + i, &ch, 1) == -1) {
      nếu (i == 0)
        i = -1;
      break;
    }
    pi->nread++;
  }
  wakeup(&pi->nwrite); //DOC: piperead-wakeup
  release(&pi->lock);
  trả_về i;
}
