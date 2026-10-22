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
  정수 readopen;  // read fd is still open
  정수 writeopen; // write fd is still open
};

정수
pipealloc(struct file **f0, struct file **f1)
{
  struct pipe *pi;

  pi = 0;
  *f0 = *f1 = 0;
  만약 ((*f0 = filealloc()) == 0 || (*f1 = filealloc()) == 0)
    goto bad;
  만약 ((pi = (struct pipe *)kalloc()) == 0)
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
  반환 0;

bad:
  만약 (pi)
    kfree((char *)pi);
  만약 (*f0)
    fileclose(*f0);
  만약 (*f1)
    fileclose(*f1);
  반환 -1;
}

void
pipeclose(struct pipe *pi, 정수 writable)
{
  acquire(&pi->lock);
  만약 (writable) {
    pi->writeopen = 0;
    wakeup(&pi->nread);
  } 아니면 {
    pi->readopen = 0;
    wakeup(&pi->nwrite);
  }
  만약 (pi->readopen == 0 && pi->writeopen == 0) {
    release(&pi->lock);
    kfree((char *)pi);
  } 아니면
    release(&pi->lock);
}

정수
pipewrite(struct pipe *pi, uint64 addr, 정수 n)
{
  정수 i = 0;
  struct proc *pr = myproc();

  acquire(&pi->lock);
  동안 (i < n) {
    만약 (pi->readopen == 0 || killed(pr)) {
      release(&pi->lock);
      반환 -1;
    }
    만약 (pi->nwrite == pi->nread + PIPESIZE) { //DOC: pipewrite-full
      wakeup(&pi->nread);
      sleep_prepare(&pi->nwrite);
      release(&pi->lock);
      sleep();
      acquire(&pi->lock);
    } 아니면 {
      char ch;
      만약 (copyin(pr->pagetable, pr->sz, &ch, addr + i, 1) == -1) {
        만약 (i == 0)
          i = -1;
        break;
      }
      pi->data[pi->nwrite++ % PIPESIZE] = ch;
      i++;
    }
  }
  wakeup(&pi->nread);
  release(&pi->lock);

  반환 i;
}

정수
piperead(struct pipe *pi, uint64 addr, 정수 n)
{
  정수 i;
  struct proc *pr = myproc();
  char ch;

  acquire(&pi->lock);
  동안 (pi->nread == pi->nwrite && pi->writeopen) { //DOC: pipe-empty
    만약 (killed(pr)) {
      release(&pi->lock);
      반환 -1;
    }
    sleep_prepare(&pi->nread); //DOC: piperead-sleep
    release(&pi->lock);
    sleep();
    acquire(&pi->lock);
  }
  위해 (i = 0; i < n; i++) { //DOC: piperead-copy
    만약 (pi->nread == pi->nwrite)
      break;
    ch = pi->data[pi->nread % PIPESIZE];
    만약 (copyout(pr->pagetable, pr->sz, addr + i, &ch, 1) == -1) {
      만약 (i == 0)
        i = -1;
      break;
    }
    pi->nread++;
  }
  wakeup(&pi->nwrite); //DOC: piperead-wakeup
  release(&pi->lock);
  반환 i;
}
