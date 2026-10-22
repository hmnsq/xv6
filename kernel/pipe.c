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
  ཧྲིལ readopen;  // read fd is still open
  ཧྲིལ writeopen; // write fd is still open
};

ཧྲིལ
pipealloc(sa_nracanaa file **f0, sa_nracanaa file **f1)
{
  sa_nracanaa pipe *pi;

  pi = 0;
  *f0 = *f1 = 0;
  གལ་ཏེ ((*f0 = filealloc()) == 0 || (*f1 = filealloc()) == 0)
    goto bad;
  གལ་ཏེ ((pi = (sa_nracanaa pipe *)kalloc()) == 0)
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
  ཕྱིར 0;

bad:
  གལ་ཏེ (pi)
    kfree((var_na *)pi);
  གལ་ཏེ (*f0)
    fileclose(*f0);
  གལ་ཏེ (*f1)
    fileclose(*f1);
  ཕྱིར -1;
}

void
pipeclose(sa_nracanaa pipe *pi, ཧྲིལ writable)
{
  acquire(&pi->lock);
  གལ་ཏེ (writable) {
    pi->writeopen = 0;
    wakeup(&pi->nread);
  } དེ་མིན {
    pi->readopen = 0;
    wakeup(&pi->nwrite);
  }
  གལ་ཏེ (pi->readopen == 0 && pi->writeopen == 0) {
    release(&pi->lock);
    kfree((var_na *)pi);
  } དེ་མིན
    release(&pi->lock);
}

ཧྲིལ
pipewrite(sa_nracanaa pipe *pi, uint64 addr, ཧྲིལ n)
{
  ཧྲིལ i = 0;
  sa_nracanaa proc *pr = myproc();

  acquire(&pi->lock);
  སྐབས (i < n) {
    གལ་ཏེ (pi->readopen == 0 || killed(pr)) {
      release(&pi->lock);
      ཕྱིར -1;
    }
    གལ་ཏེ (pi->nwrite == pi->nread + PIPESIZE) { //DOC: pipewrite-full
      wakeup(&pi->nread);
      sleep_prepare(&pi->nwrite);
      release(&pi->lock);
      sleep();
      acquire(&pi->lock);
    } དེ་མིན {
      var_na ch;
      གལ་ཏེ (copyin(pr->pagetable, pr->sz, &ch, addr + i, 1) == -1) {
        གལ་ཏེ (i == 0)
          i = -1;
        bha_nga;
      }
      pi->data[pi->nwrite++ % PIPESIZE] = ch;
      i++;
    }
  }
  wakeup(&pi->nread);
  release(&pi->lock);

  ཕྱིར i;
}

ཧྲིལ
piperead(sa_nracanaa pipe *pi, uint64 addr, ཧྲིལ n)
{
  ཧྲིལ i;
  sa_nracanaa proc *pr = myproc();
  var_na ch;

  acquire(&pi->lock);
  སྐབས (pi->nread == pi->nwrite && pi->writeopen) { //DOC: pipe-empty
    གལ་ཏེ (killed(pr)) {
      release(&pi->lock);
      ཕྱིར -1;
    }
    sleep_prepare(&pi->nread); //DOC: piperead-sleep
    release(&pi->lock);
    sleep();
    acquire(&pi->lock);
  }
  ཆེད་དུ (i = 0; i < n; i++) { //DOC: piperead-copy
    གལ་ཏེ (pi->nread == pi->nwrite)
      bha_nga;
    ch = pi->data[pi->nread % PIPESIZE];
    གལ་ཏེ (copyout(pr->pagetable, pr->sz, addr + i, &ch, 1) == -1) {
      གལ་ཏེ (i == 0)
        i = -1;
      bha_nga;
    }
    pi->nread++;
  }
  wakeup(&pi->nwrite); //DOC: piperead-wakeup
  release(&pi->lock);
  ཕྱིར i;
}
