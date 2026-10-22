#شامل "types.h"
#شامل "riscv.h"
#شامل "defs.h"
#شامل "param.h"
#شامل "spinlock.h"
#شامل "proc.h"
#شامل "fs.h"
#شامل "sleeplock.h"
#شامل "file.h"

#define PIPESIZE 512

ساختار pipe {
  ساختار spinlock lock;
  نویسه data[PIPESIZE];
  uint nread;    // number of bytes read
  uint nwrite;   // number of bytes written
  صحیح readopen;  // read fd is still open
  صحیح writeopen; // write fd is still open
};

صحیح
pipealloc(ساختار file **f0, ساختار file **f1)
{
  ساختار pipe *pi;

  pi = 0;
  *f0 = *f1 = 0;
  اگر ((*f0 = filealloc()) == 0 || (*f1 = filealloc()) == 0)
    برو bad;
  اگر ((pi = (ساختار pipe *)kalloc()) == 0)
    برو bad;
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
  بازگشت 0;

bad:
  اگر (pi)
    kfree((نویسه *)pi);
  اگر (*f0)
    fileclose(*f0);
  اگر (*f1)
    fileclose(*f1);
  بازگشت -1;
}

تهی
pipeclose(ساختار pipe *pi, صحیح writable)
{
  acquire(&pi->lock);
  اگر (writable) {
    pi->writeopen = 0;
    wakeup(&pi->nread);
  } وگرنه {
    pi->readopen = 0;
    wakeup(&pi->nwrite);
  }
  اگر (pi->readopen == 0 && pi->writeopen == 0) {
    release(&pi->lock);
    kfree((نویسه *)pi);
  } وگرنه
    release(&pi->lock);
}

صحیح
pipewrite(ساختار pipe *pi, uint64 addr, صحیح n)
{
  صحیح i = 0;
  ساختار proc *pr = myproc();

  acquire(&pi->lock);
  وقتی (i < n) {
    اگر (pi->readopen == 0 || killed(pr)) {
      release(&pi->lock);
      بازگشت -1;
    }
    اگر (pi->nwrite == pi->nread + PIPESIZE) { //DOC: pipewrite-full
      wakeup(&pi->nread);
      sleep_prepare(&pi->nwrite);
      release(&pi->lock);
      sleep();
      acquire(&pi->lock);
    } وگرنه {
      نویسه ch;
      اگر (copyin(pr->pagetable, pr->sz, &ch, addr + i, 1) == -1) {
        اگر (i == 0)
          i = -1;
        بشکن;
      }
      pi->data[pi->nwrite++ % PIPESIZE] = ch;
      i++;
    }
  }
  wakeup(&pi->nread);
  release(&pi->lock);

  بازگشت i;
}

صحیح
piperead(ساختار pipe *pi, uint64 addr, صحیح n)
{
  صحیح i;
  ساختار proc *pr = myproc();
  نویسه ch;

  acquire(&pi->lock);
  وقتی (pi->nread == pi->nwrite && pi->writeopen) { //DOC: pipe-empty
    اگر (killed(pr)) {
      release(&pi->lock);
      بازگشت -1;
    }
    sleep_prepare(&pi->nread); //DOC: piperead-sleep
    release(&pi->lock);
    sleep();
    acquire(&pi->lock);
  }
  برای (i = 0; i < n; i++) { //DOC: piperead-copy
    اگر (pi->nread == pi->nwrite)
      بشکن;
    ch = pi->data[pi->nread % PIPESIZE];
    اگر (copyout(pr->pagetable, pr->sz, addr + i, &ch, 1) == -1) {
      اگر (i == 0)
        i = -1;
      بشکن;
    }
    pi->nread++;
  }
  wakeup(&pi->nwrite); //DOC: piperead-wakeup
  release(&pi->lock);
  بازگشت i;
}
