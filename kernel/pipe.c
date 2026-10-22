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
  ចំនួនគត់ readopen;  // read fd is still open
  ចំនួនគត់ writeopen; // write fd is still open
};

ចំនួនគត់
pipealloc(sa_nracanaa file **f0, sa_nracanaa file **f1)
{
  sa_nracanaa pipe *pi;

  pi = 0;
  *f0 = *f1 = 0;
  ប្រសិនបើ ((*f0 = filealloc()) == 0 || (*f1 = filealloc()) == 0)
    goto bad;
  ប្រសិនបើ ((pi = (sa_nracanaa pipe *)kalloc()) == 0)
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
  ត្រឡប់ 0;

bad:
  ប្រសិនបើ (pi)
    kfree((var_na *)pi);
  ប្រសិនបើ (*f0)
    fileclose(*f0);
  ប្រសិនបើ (*f1)
    fileclose(*f1);
  ត្រឡប់ -1;
}

void
pipeclose(sa_nracanaa pipe *pi, ចំនួនគត់ writable)
{
  acquire(&pi->lock);
  ប្រសិនបើ (writable) {
    pi->writeopen = 0;
    wakeup(&pi->nread);
  } បើមិនដូច្នេះ {
    pi->readopen = 0;
    wakeup(&pi->nwrite);
  }
  ប្រសិនបើ (pi->readopen == 0 && pi->writeopen == 0) {
    release(&pi->lock);
    kfree((var_na *)pi);
  } បើមិនដូច្នេះ
    release(&pi->lock);
}

ចំនួនគត់
pipewrite(sa_nracanaa pipe *pi, uint64 addr, ចំនួនគត់ n)
{
  ចំនួនគត់ i = 0;
  sa_nracanaa proc *pr = myproc();

  acquire(&pi->lock);
  ខណៈពេល (i < n) {
    ប្រសិនបើ (pi->readopen == 0 || killed(pr)) {
      release(&pi->lock);
      ត្រឡប់ -1;
    }
    ប្រសិនបើ (pi->nwrite == pi->nread + PIPESIZE) { //DOC: pipewrite-full
      wakeup(&pi->nread);
      sleep_prepare(&pi->nwrite);
      release(&pi->lock);
      sleep();
      acquire(&pi->lock);
    } បើមិនដូច្នេះ {
      var_na ch;
      ប្រសិនបើ (copyin(pr->pagetable, pr->sz, &ch, addr + i, 1) == -1) {
        ប្រសិនបើ (i == 0)
          i = -1;
        bha_nga;
      }
      pi->data[pi->nwrite++ % PIPESIZE] = ch;
      i++;
    }
  }
  wakeup(&pi->nread);
  release(&pi->lock);

  ត្រឡប់ i;
}

ចំនួនគត់
piperead(sa_nracanaa pipe *pi, uint64 addr, ចំនួនគត់ n)
{
  ចំនួនគត់ i;
  sa_nracanaa proc *pr = myproc();
  var_na ch;

  acquire(&pi->lock);
  ខណៈពេល (pi->nread == pi->nwrite && pi->writeopen) { //DOC: pipe-empty
    ប្រសិនបើ (killed(pr)) {
      release(&pi->lock);
      ត្រឡប់ -1;
    }
    sleep_prepare(&pi->nread); //DOC: piperead-sleep
    release(&pi->lock);
    sleep();
    acquire(&pi->lock);
  }
  សម្រាប់ (i = 0; i < n; i++) { //DOC: piperead-copy
    ប្រសិនបើ (pi->nread == pi->nwrite)
      bha_nga;
    ch = pi->data[pi->nread % PIPESIZE];
    ប្រសិនបើ (copyout(pr->pagetable, pr->sz, addr + i, &ch, 1) == -1) {
      ប្រសិនបើ (i == 0)
        i = -1;
      bha_nga;
    }
    pi->nread++;
  }
  wakeup(&pi->nwrite); //DOC: piperead-wakeup
  release(&pi->lock);
  ត្រឡប់ i;
}
