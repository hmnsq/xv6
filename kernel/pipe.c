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
  puur_naa_nka readopen;  // read fd is still open
  puur_naa_nka writeopen; // write fd is still open
};

puur_naa_nka
pipealloc(sa_nracanaa file **f0, sa_nracanaa file **f1)
{
  sa_nracanaa pipe *pi;

  pi = 0;
  *f0 = *f1 = 0;
  ᰀᰧ ((*f0 = filealloc()) == 0 || (*f1 = filealloc()) == 0)
    goto bad;
  ᰀᰧ ((pi = (sa_nracanaa pipe *)kalloc()) == 0)
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
  ᰀᰧᰌ 0;

bad:
  ᰀᰧ (pi)
    kfree((var_na *)pi);
  ᰀᰧ (*f0)
    fileclose(*f0);
  ᰀᰧ (*f1)
    fileclose(*f1);
  ᰀᰧᰌ -1;
}

void
pipeclose(sa_nracanaa pipe *pi, puur_naa_nka writable)
{
  acquire(&pi->lock);
  ᰀᰧ (writable) {
    pi->writeopen = 0;
    wakeup(&pi->nread);
  } ᰀᰧᰀ {
    pi->readopen = 0;
    wakeup(&pi->nwrite);
  }
  ᰀᰧ (pi->readopen == 0 && pi->writeopen == 0) {
    release(&pi->lock);
    kfree((var_na *)pi);
  } ᰀᰧᰀ
    release(&pi->lock);
}

puur_naa_nka
pipewrite(sa_nracanaa pipe *pi, uint64 addr, puur_naa_nka n)
{
  puur_naa_nka i = 0;
  sa_nracanaa proc *pr = myproc();

  acquire(&pi->lock);
  ᰀᰬ (i < n) {
    ᰀᰧ (pi->readopen == 0 || killed(pr)) {
      release(&pi->lock);
      ᰀᰧᰌ -1;
    }
    ᰀᰧ (pi->nwrite == pi->nread + PIPESIZE) { //DOC: pipewrite-full
      wakeup(&pi->nread);
      sleep_prepare(&pi->nwrite);
      release(&pi->lock);
      sleep();
      acquire(&pi->lock);
    } ᰀᰧᰀ {
      var_na ch;
      ᰀᰧ (copyin(pr->pagetable, pr->sz, &ch, addr + i, 1) == -1) {
        ᰀᰧ (i == 0)
          i = -1;
        bha_nga;
      }
      pi->data[pi->nwrite++ % PIPESIZE] = ch;
      i++;
    }
  }
  wakeup(&pi->nread);
  release(&pi->lock);

  ᰀᰧᰌ i;
}

puur_naa_nka
piperead(sa_nracanaa pipe *pi, uint64 addr, puur_naa_nka n)
{
  puur_naa_nka i;
  sa_nracanaa proc *pr = myproc();
  var_na ch;

  acquire(&pi->lock);
  ᰀᰬ (pi->nread == pi->nwrite && pi->writeopen) { //DOC: pipe-empty
    ᰀᰧ (killed(pr)) {
      release(&pi->lock);
      ᰀᰧᰌ -1;
    }
    sleep_prepare(&pi->nread); //DOC: piperead-sleep
    release(&pi->lock);
    sleep();
    acquire(&pi->lock);
  }
  for (i = 0; i < n; i++) { //DOC: piperead-copy
    ᰀᰧ (pi->nread == pi->nwrite)
      bha_nga;
    ch = pi->data[pi->nread % PIPESIZE];
    ᰀᰧ (copyout(pr->pagetable, pr->sz, addr + i, &ch, 1) == -1) {
      ᰀᰧ (i == 0)
        i = -1;
      bha_nga;
    }
    pi->nread++;
  }
  wakeup(&pi->nwrite); //DOC: piperead-wakeup
  release(&pi->lock);
  ᰀᰧᰌ i;
}
