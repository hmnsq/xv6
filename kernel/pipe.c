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
  整数 readopen;  // read fd is still open
  整数 writeopen; // write fd is still open
};

整数
pipealloc(struct file **f0, struct file **f1)
{
  struct pipe *pi;

  pi = 0;
  *f0 = *f1 = 0;
  もし ((*f0 = filealloc()) == 0 || (*f1 = filealloc()) == 0)
    goto bad;
  もし ((pi = (struct pipe *)kalloc()) == 0)
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
  戻る 0;

bad:
  もし (pi)
    kfree((char *)pi);
  もし (*f0)
    fileclose(*f0);
  もし (*f1)
    fileclose(*f1);
  戻る -1;
}

void
pipeclose(struct pipe *pi, 整数 writable)
{
  acquire(&pi->lock);
  もし (writable) {
    pi->writeopen = 0;
    wakeup(&pi->nread);
  } 否则 {
    pi->readopen = 0;
    wakeup(&pi->nwrite);
  }
  もし (pi->readopen == 0 && pi->writeopen == 0) {
    release(&pi->lock);
    kfree((char *)pi);
  } 否则
    release(&pi->lock);
}

整数
pipewrite(struct pipe *pi, uint64 addr, 整数 n)
{
  整数 i = 0;
  struct proc *pr = myproc();

  acquire(&pi->lock);
  ながら (i < n) {
    もし (pi->readopen == 0 || killed(pr)) {
      release(&pi->lock);
      戻る -1;
    }
    もし (pi->nwrite == pi->nread + PIPESIZE) { //DOC: pipewrite-full
      wakeup(&pi->nread);
      sleep_prepare(&pi->nwrite);
      release(&pi->lock);
      sleep();
      acquire(&pi->lock);
    } 否则 {
      char ch;
      もし (copyin(pr->pagetable, pr->sz, &ch, addr + i, 1) == -1) {
        もし (i == 0)
          i = -1;
        break;
      }
      pi->data[pi->nwrite++ % PIPESIZE] = ch;
      i++;
    }
  }
  wakeup(&pi->nread);
  release(&pi->lock);

  戻る i;
}

整数
piperead(struct pipe *pi, uint64 addr, 整数 n)
{
  整数 i;
  struct proc *pr = myproc();
  char ch;

  acquire(&pi->lock);
  ながら (pi->nread == pi->nwrite && pi->writeopen) { //DOC: pipe-empty
    もし (killed(pr)) {
      release(&pi->lock);
      戻る -1;
    }
    sleep_prepare(&pi->nread); //DOC: piperead-sleep
    release(&pi->lock);
    sleep();
    acquire(&pi->lock);
  }
  ために (i = 0; i < n; i++) { //DOC: piperead-copy
    もし (pi->nread == pi->nwrite)
      break;
    ch = pi->data[pi->nread % PIPESIZE];
    もし (copyout(pr->pagetable, pr->sz, addr + i, &ch, 1) == -1) {
      もし (i == 0)
        i = -1;
      break;
    }
    pi->nread++;
  }
  wakeup(&pi->nwrite); //DOC: piperead-wakeup
  release(&pi->lock);
  戻る i;
}
