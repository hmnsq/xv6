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
  ຈຳນວນເຕັມ readopen;  // read fd is still open
  ຈຳນວນເຕັມ writeopen; // write fd is still open
};

ຈຳນວນເຕັມ
pipealloc(sa_nracanaa file **f0, sa_nracanaa file **f1)
{
  sa_nracanaa pipe *pi;

  pi = 0;
  *f0 = *f1 = 0;
  ຖ້າ ((*f0 = filealloc()) == 0 || (*f1 = filealloc()) == 0)
    goto bad;
  ຖ້າ ((pi = (sa_nracanaa pipe *)kalloc()) == 0)
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
  ສົ່ງຄືນ 0;

bad:
  ຖ້າ (pi)
    kfree((var_na *)pi);
  ຖ້າ (*f0)
    fileclose(*f0);
  ຖ້າ (*f1)
    fileclose(*f1);
  ສົ່ງຄືນ -1;
}

void
pipeclose(sa_nracanaa pipe *pi, ຈຳນວນເຕັມ writable)
{
  acquire(&pi->lock);
  ຖ້າ (writable) {
    pi->writeopen = 0;
    wakeup(&pi->nread);
  } ບໍ່ດັ່ງນັ້ນ {
    pi->readopen = 0;
    wakeup(&pi->nwrite);
  }
  ຖ້າ (pi->readopen == 0 && pi->writeopen == 0) {
    release(&pi->lock);
    kfree((var_na *)pi);
  } ບໍ່ດັ່ງນັ້ນ
    release(&pi->lock);
}

ຈຳນວນເຕັມ
pipewrite(sa_nracanaa pipe *pi, uint64 addr, ຈຳນວນເຕັມ n)
{
  ຈຳນວນເຕັມ i = 0;
  sa_nracanaa proc *pr = myproc();

  acquire(&pi->lock);
  ໃນຂະນະທີ່ (i < n) {
    ຖ້າ (pi->readopen == 0 || killed(pr)) {
      release(&pi->lock);
      ສົ່ງຄືນ -1;
    }
    ຖ້າ (pi->nwrite == pi->nread + PIPESIZE) { //DOC: pipewrite-full
      wakeup(&pi->nread);
      sleep_prepare(&pi->nwrite);
      release(&pi->lock);
      sleep();
      acquire(&pi->lock);
    } ບໍ່ດັ່ງນັ້ນ {
      var_na ch;
      ຖ້າ (copyin(pr->pagetable, pr->sz, &ch, addr + i, 1) == -1) {
        ຖ້າ (i == 0)
          i = -1;
        bha_nga;
      }
      pi->data[pi->nwrite++ % PIPESIZE] = ch;
      i++;
    }
  }
  wakeup(&pi->nread);
  release(&pi->lock);

  ສົ່ງຄືນ i;
}

ຈຳນວນເຕັມ
piperead(sa_nracanaa pipe *pi, uint64 addr, ຈຳນວນເຕັມ n)
{
  ຈຳນວນເຕັມ i;
  sa_nracanaa proc *pr = myproc();
  var_na ch;

  acquire(&pi->lock);
  ໃນຂະນະທີ່ (pi->nread == pi->nwrite && pi->writeopen) { //DOC: pipe-empty
    ຖ້າ (killed(pr)) {
      release(&pi->lock);
      ສົ່ງຄືນ -1;
    }
    sleep_prepare(&pi->nread); //DOC: piperead-sleep
    release(&pi->lock);
    sleep();
    acquire(&pi->lock);
  }
  ສຳລັບ (i = 0; i < n; i++) { //DOC: piperead-copy
    ຖ້າ (pi->nread == pi->nwrite)
      bha_nga;
    ch = pi->data[pi->nread % PIPESIZE];
    ຖ້າ (copyout(pr->pagetable, pr->sz, addr + i, &ch, 1) == -1) {
      ຖ້າ (i == 0)
        i = -1;
      bha_nga;
    }
    pi->nread++;
  }
  wakeup(&pi->nwrite); //DOC: piperead-wakeup
  release(&pi->lock);
  ສົ່ງຄືນ i;
}
