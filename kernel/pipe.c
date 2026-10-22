#ضمن "types.h"
#ضمن "riscv.h"
#ضمن "defs.h"
#ضمن "param.h"
#ضمن "spinlock.h"
#ضمن "proc.h"
#ضمن "fs.h"
#ضمن "sleeplock.h"
#ضمن "file.h"

#define PIPESIZE 512

بنية pipe {
  بنية spinlock lock;
  حرف data[PIPESIZE];
  uint nread;    // number of bytes read
  uint nwrite;   // number of bytes written
  صحيح readopen;  // read fd is still open
  صحيح writeopen; // write fd is still open
};

صحيح
pipealloc(بنية file **f0, بنية file **f1)
{
  بنية pipe *pi;

  pi = 0;
  *f0 = *f1 = 0;
  אם ((*f0 = filealloc()) == 0 || (*f1 = filealloc()) == 0)
    اذهب bad;
  אם ((pi = (بنية pipe *)kalloc()) == 0)
    اذهب bad;
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
  החזר 0;

bad:
  אם (pi)
    kfree((حرف *)pi);
  אם (*f0)
    fileclose(*f0);
  אם (*f1)
    fileclose(*f1);
  החזר -1;
}

فراغ
pipeclose(بنية pipe *pi, صحيح writable)
{
  acquire(&pi->lock);
  אם (writable) {
    pi->writeopen = 0;
    wakeup(&pi->nread);
  } אחרת {
    pi->readopen = 0;
    wakeup(&pi->nwrite);
  }
  אם (pi->readopen == 0 && pi->writeopen == 0) {
    release(&pi->lock);
    kfree((حرف *)pi);
  } אחרת
    release(&pi->lock);
}

صحيح
pipewrite(بنية pipe *pi, uint64 addr, صحيح n)
{
  صحيح i = 0;
  بنية proc *pr = myproc();

  acquire(&pi->lock);
  כלעוד (i < n) {
    אם (pi->readopen == 0 || killed(pr)) {
      release(&pi->lock);
      החזר -1;
    }
    אם (pi->nwrite == pi->nread + PIPESIZE) { //DOC: pipewrite-full
      wakeup(&pi->nread);
      sleep_prepare(&pi->nwrite);
      release(&pi->lock);
      sleep();
      acquire(&pi->lock);
    } אחרת {
      حرف ch;
      אם (copyin(pr->pagetable, pr->sz, &ch, addr + i, 1) == -1) {
        אם (i == 0)
          i = -1;
        اكسر;
      }
      pi->data[pi->nwrite++ % PIPESIZE] = ch;
      i++;
    }
  }
  wakeup(&pi->nread);
  release(&pi->lock);

  החזר i;
}

صحيح
piperead(بنية pipe *pi, uint64 addr, صحيح n)
{
  صحيح i;
  بنية proc *pr = myproc();
  حرف ch;

  acquire(&pi->lock);
  כלעוד (pi->nread == pi->nwrite && pi->writeopen) { //DOC: pipe-empty
    אם (killed(pr)) {
      release(&pi->lock);
      החזר -1;
    }
    sleep_prepare(&pi->nread); //DOC: piperead-sleep
    release(&pi->lock);
    sleep();
    acquire(&pi->lock);
  }
  עבור (i = 0; i < n; i++) { //DOC: piperead-copy
    אם (pi->nread == pi->nwrite)
      اكسر;
    ch = pi->data[pi->nread % PIPESIZE];
    אם (copyout(pr->pagetable, pr->sz, addr + i, &ch, 1) == -1) {
      אם (i == 0)
        i = -1;
      اكسر;
    }
    pi->nread++;
  }
  wakeup(&pi->nwrite); //DOC: piperead-wakeup
  release(&pi->lock);
  החזר i;
}
