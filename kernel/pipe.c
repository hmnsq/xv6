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
  ကိန်းပြည့် readopen;  // read fd is still open
  ကိန်းပြည့် writeopen; // write fd is still open
};

ကိန်းပြည့်
pipealloc(sa_nracanaa file **f0, sa_nracanaa file **f1)
{
  sa_nracanaa pipe *pi;

  pi = 0;
  *f0 = *f1 = 0;
  အကယ်၍ ((*f0 = filealloc()) == 0 || (*f1 = filealloc()) == 0)
    goto bad;
  အကယ်၍ ((pi = (sa_nracanaa pipe *)kalloc()) == 0)
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
  ပြန်ပေး 0;

bad:
  အကယ်၍ (pi)
    kfree((var_na *)pi);
  အကယ်၍ (*f0)
    fileclose(*f0);
  အကယ်၍ (*f1)
    fileclose(*f1);
  ပြန်ပေး -1;
}

void
pipeclose(sa_nracanaa pipe *pi, ကိန်းပြည့် writable)
{
  acquire(&pi->lock);
  အကယ်၍ (writable) {
    pi->writeopen = 0;
    wakeup(&pi->nread);
  } မဟုတ်ပါက {
    pi->readopen = 0;
    wakeup(&pi->nwrite);
  }
  အကယ်၍ (pi->readopen == 0 && pi->writeopen == 0) {
    release(&pi->lock);
    kfree((var_na *)pi);
  } မဟုတ်ပါက
    release(&pi->lock);
}

ကိန်းပြည့်
pipewrite(sa_nracanaa pipe *pi, uint64 addr, ကိန်းပြည့် n)
{
  ကိန်းပြည့် i = 0;
  sa_nracanaa proc *pr = myproc();

  acquire(&pi->lock);
  နေစဉ် (i < n) {
    အကယ်၍ (pi->readopen == 0 || killed(pr)) {
      release(&pi->lock);
      ပြန်ပေး -1;
    }
    အကယ်၍ (pi->nwrite == pi->nread + PIPESIZE) { //DOC: pipewrite-full
      wakeup(&pi->nread);
      sleep_prepare(&pi->nwrite);
      release(&pi->lock);
      sleep();
      acquire(&pi->lock);
    } မဟုတ်ပါက {
      var_na ch;
      အကယ်၍ (copyin(pr->pagetable, pr->sz, &ch, addr + i, 1) == -1) {
        အကယ်၍ (i == 0)
          i = -1;
        bha_nga;
      }
      pi->data[pi->nwrite++ % PIPESIZE] = ch;
      i++;
    }
  }
  wakeup(&pi->nread);
  release(&pi->lock);

  ပြန်ပေး i;
}

ကိန်းပြည့်
piperead(sa_nracanaa pipe *pi, uint64 addr, ကိန်းပြည့် n)
{
  ကိန်းပြည့် i;
  sa_nracanaa proc *pr = myproc();
  var_na ch;

  acquire(&pi->lock);
  နေစဉ် (pi->nread == pi->nwrite && pi->writeopen) { //DOC: pipe-empty
    အကယ်၍ (killed(pr)) {
      release(&pi->lock);
      ပြန်ပေး -1;
    }
    sleep_prepare(&pi->nread); //DOC: piperead-sleep
    release(&pi->lock);
    sleep();
    acquire(&pi->lock);
  }
  အတွက် (i = 0; i < n; i++) { //DOC: piperead-copy
    အကယ်၍ (pi->nread == pi->nwrite)
      bha_nga;
    ch = pi->data[pi->nread % PIPESIZE];
    အကယ်၍ (copyout(pr->pagetable, pr->sz, addr + i, &ch, 1) == -1) {
      အကယ်၍ (i == 0)
        i = -1;
      bha_nga;
    }
    pi->nread++;
  }
  wakeup(&pi->nwrite); //DOC: piperead-wakeup
  release(&pi->lock);
  ပြန်ပေး i;
}
