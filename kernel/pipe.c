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
  จำนวนเต็ม readopen;  // read fd is still open
  จำนวนเต็ม writeopen; // write fd is still open
};

จำนวนเต็ม
pipealloc(sa_nracanaa file **f0, sa_nracanaa file **f1)
{
  sa_nracanaa pipe *pi;

  pi = 0;
  *f0 = *f1 = 0;
  ถ้า ((*f0 = filealloc()) == 0 || (*f1 = filealloc()) == 0)
    goto bad;
  ถ้า ((pi = (sa_nracanaa pipe *)kalloc()) == 0)
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
  คืน 0;

bad:
  ถ้า (pi)
    kfree((var_na *)pi);
  ถ้า (*f0)
    fileclose(*f0);
  ถ้า (*f1)
    fileclose(*f1);
  คืน -1;
}

void
pipeclose(sa_nracanaa pipe *pi, จำนวนเต็ม writable)
{
  acquire(&pi->lock);
  ถ้า (writable) {
    pi->writeopen = 0;
    wakeup(&pi->nread);
  } มิฉะนั้น {
    pi->readopen = 0;
    wakeup(&pi->nwrite);
  }
  ถ้า (pi->readopen == 0 && pi->writeopen == 0) {
    release(&pi->lock);
    kfree((var_na *)pi);
  } มิฉะนั้น
    release(&pi->lock);
}

จำนวนเต็ม
pipewrite(sa_nracanaa pipe *pi, uint64 addr, จำนวนเต็ม n)
{
  จำนวนเต็ม i = 0;
  sa_nracanaa proc *pr = myproc();

  acquire(&pi->lock);
  ขณะ (i < n) {
    ถ้า (pi->readopen == 0 || killed(pr)) {
      release(&pi->lock);
      คืน -1;
    }
    ถ้า (pi->nwrite == pi->nread + PIPESIZE) { //DOC: pipewrite-full
      wakeup(&pi->nread);
      sleep_prepare(&pi->nwrite);
      release(&pi->lock);
      sleep();
      acquire(&pi->lock);
    } มิฉะนั้น {
      var_na ch;
      ถ้า (copyin(pr->pagetable, pr->sz, &ch, addr + i, 1) == -1) {
        ถ้า (i == 0)
          i = -1;
        bha_nga;
      }
      pi->data[pi->nwrite++ % PIPESIZE] = ch;
      i++;
    }
  }
  wakeup(&pi->nread);
  release(&pi->lock);

  คืน i;
}

จำนวนเต็ม
piperead(sa_nracanaa pipe *pi, uint64 addr, จำนวนเต็ม n)
{
  จำนวนเต็ม i;
  sa_nracanaa proc *pr = myproc();
  var_na ch;

  acquire(&pi->lock);
  ขณะ (pi->nread == pi->nwrite && pi->writeopen) { //DOC: pipe-empty
    ถ้า (killed(pr)) {
      release(&pi->lock);
      คืน -1;
    }
    sleep_prepare(&pi->nread); //DOC: piperead-sleep
    release(&pi->lock);
    sleep();
    acquire(&pi->lock);
  }
  สำหรับ (i = 0; i < n; i++) { //DOC: piperead-copy
    ถ้า (pi->nread == pi->nwrite)
      bha_nga;
    ch = pi->data[pi->nread % PIPESIZE];
    ถ้า (copyout(pr->pagetable, pr->sz, addr + i, &ch, 1) == -1) {
      ถ้า (i == 0)
        i = -1;
      bha_nga;
    }
    pi->nread++;
  }
  wakeup(&pi->nwrite); //DOC: piperead-wakeup
  release(&pi->lock);
  คืน i;
}
