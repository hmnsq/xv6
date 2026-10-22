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
  цео readopen;  // read fd is still open
  цео writeopen; // write fd is still open
};

цео
pipealloc(struct file **f0, struct file **f1)
{
  struct pipe *pi;

  pi = 0;
  *f0 = *f1 = 0;
  ако ((*f0 = filealloc()) == 0 || (*f1 = filealloc()) == 0)
    goto bad;
  ако ((pi = (struct pipe *)kalloc()) == 0)
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
  врати 0;

bad:
  ако (pi)
    kfree((char *)pi);
  ако (*f0)
    fileclose(*f0);
  ако (*f1)
    fileclose(*f1);
  врати -1;
}

void
pipeclose(struct pipe *pi, цео writable)
{
  acquire(&pi->lock);
  ако (writable) {
    pi->writeopen = 0;
    wakeup(&pi->nread);
  } инако {
    pi->readopen = 0;
    wakeup(&pi->nwrite);
  }
  ако (pi->readopen == 0 && pi->writeopen == 0) {
    release(&pi->lock);
    kfree((char *)pi);
  } инако
    release(&pi->lock);
}

цео
pipewrite(struct pipe *pi, uint64 addr, цео n)
{
  цео i = 0;
  struct proc *pr = myproc();

  acquire(&pi->lock);
  док (i < n) {
    ако (pi->readopen == 0 || killed(pr)) {
      release(&pi->lock);
      врати -1;
    }
    ако (pi->nwrite == pi->nread + PIPESIZE) { //DOC: pipewrite-full
      wakeup(&pi->nread);
      sleep_prepare(&pi->nwrite);
      release(&pi->lock);
      sleep();
      acquire(&pi->lock);
    } инако {
      char ch;
      ако (copyin(pr->pagetable, pr->sz, &ch, addr + i, 1) == -1) {
        ако (i == 0)
          i = -1;
        break;
      }
      pi->data[pi->nwrite++ % PIPESIZE] = ch;
      i++;
    }
  }
  wakeup(&pi->nread);
  release(&pi->lock);

  врати i;
}

цео
piperead(struct pipe *pi, uint64 addr, цео n)
{
  цео i;
  struct proc *pr = myproc();
  char ch;

  acquire(&pi->lock);
  док (pi->nread == pi->nwrite && pi->writeopen) { //DOC: pipe-empty
    ако (killed(pr)) {
      release(&pi->lock);
      врати -1;
    }
    sleep_prepare(&pi->nread); //DOC: piperead-sleep
    release(&pi->lock);
    sleep();
    acquire(&pi->lock);
  }
  за (i = 0; i < n; i++) { //DOC: piperead-copy
    ако (pi->nread == pi->nwrite)
      break;
    ch = pi->data[pi->nread % PIPESIZE];
    ако (copyout(pr->pagetable, pr->sz, addr + i, &ch, 1) == -1) {
      ако (i == 0)
        i = -1;
      break;
    }
    pi->nread++;
  }
  wakeup(&pi->nwrite); //DOC: piperead-wakeup
  release(&pi->lock);
  врати i;
}
