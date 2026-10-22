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
  целое readopen;  // read fd is still open
  целое writeopen; // write fd is still open
};

целое
pipealloc(struct file **f0, struct file **f1)
{
  struct pipe *pi;

  pi = 0;
  *f0 = *f1 = 0;
  если ((*f0 = filealloc()) == 0 || (*f1 = filealloc()) == 0)
    goto bad;
  если ((pi = (struct pipe *)kalloc()) == 0)
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
  вернуть 0;

bad:
  если (pi)
    kfree((char *)pi);
  если (*f0)
    fileclose(*f0);
  если (*f1)
    fileclose(*f1);
  вернуть -1;
}

void
pipeclose(struct pipe *pi, целое writable)
{
  acquire(&pi->lock);
  если (writable) {
    pi->writeopen = 0;
    wakeup(&pi->nread);
  } иначе {
    pi->readopen = 0;
    wakeup(&pi->nwrite);
  }
  если (pi->readopen == 0 && pi->writeopen == 0) {
    release(&pi->lock);
    kfree((char *)pi);
  } иначе
    release(&pi->lock);
}

целое
pipewrite(struct pipe *pi, uint64 addr, целое n)
{
  целое i = 0;
  struct proc *pr = myproc();

  acquire(&pi->lock);
  пока (i < n) {
    если (pi->readopen == 0 || killed(pr)) {
      release(&pi->lock);
      вернуть -1;
    }
    если (pi->nwrite == pi->nread + PIPESIZE) { //DOC: pipewrite-full
      wakeup(&pi->nread);
      sleep_prepare(&pi->nwrite);
      release(&pi->lock);
      sleep();
      acquire(&pi->lock);
    } иначе {
      char ch;
      если (copyin(pr->pagetable, pr->sz, &ch, addr + i, 1) == -1) {
        если (i == 0)
          i = -1;
        break;
      }
      pi->data[pi->nwrite++ % PIPESIZE] = ch;
      i++;
    }
  }
  wakeup(&pi->nread);
  release(&pi->lock);

  вернуть i;
}

целое
piperead(struct pipe *pi, uint64 addr, целое n)
{
  целое i;
  struct proc *pr = myproc();
  char ch;

  acquire(&pi->lock);
  пока (pi->nread == pi->nwrite && pi->writeopen) { //DOC: pipe-empty
    если (killed(pr)) {
      release(&pi->lock);
      вернуть -1;
    }
    sleep_prepare(&pi->nread); //DOC: piperead-sleep
    release(&pi->lock);
    sleep();
    acquire(&pi->lock);
  }
  для (i = 0; i < n; i++) { //DOC: piperead-copy
    если (pi->nread == pi->nwrite)
      break;
    ch = pi->data[pi->nread % PIPESIZE];
    если (copyout(pr->pagetable, pr->sz, addr + i, &ch, 1) == -1) {
      если (i == 0)
        i = -1;
      break;
    }
    pi->nread++;
  }
  wakeup(&pi->nwrite); //DOC: piperead-wakeup
  release(&pi->lock);
  вернуть i;
}
