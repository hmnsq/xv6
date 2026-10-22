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
  𐡄𐡍 ((*f0 = filealloc()) == 0 || (*f1 = filealloc()) == 0)
    اذهب bad;
  𐡄𐡍 ((pi = (بنية pipe *)kalloc()) == 0)
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
  أرجع 0;

bad:
  𐡄𐡍 (pi)
    kfree((حرف *)pi);
  𐡄𐡍 (*f0)
    fileclose(*f0);
  𐡄𐡍 (*f1)
    fileclose(*f1);
  أرجع -1;
}

فراغ
pipeclose(بنية pipe *pi, صحيح writable)
{
  acquire(&pi->lock);
  𐡄𐡍 (writable) {
    pi->writeopen = 0;
    wakeup(&pi->nread);
  } وإلا {
    pi->readopen = 0;
    wakeup(&pi->nwrite);
  }
  𐡄𐡍 (pi->readopen == 0 && pi->writeopen == 0) {
    release(&pi->lock);
    kfree((حرف *)pi);
  } وإلا
    release(&pi->lock);
}

صحيح
pipewrite(بنية pipe *pi, uint64 addr, صحيح n)
{
  صحيح i = 0;
  بنية proc *pr = myproc();

  acquire(&pi->lock);
  طالما (i < n) {
    𐡄𐡍 (pi->readopen == 0 || killed(pr)) {
      release(&pi->lock);
      أرجع -1;
    }
    𐡄𐡍 (pi->nwrite == pi->nread + PIPESIZE) { //DOC: pipewrite-full
      wakeup(&pi->nread);
      sleep_prepare(&pi->nwrite);
      release(&pi->lock);
      sleep();
      acquire(&pi->lock);
    } وإلا {
      حرف ch;
      𐡄𐡍 (copyin(pr->pagetable, pr->sz, &ch, addr + i, 1) == -1) {
        𐡄𐡍 (i == 0)
          i = -1;
        اكسر;
      }
      pi->data[pi->nwrite++ % PIPESIZE] = ch;
      i++;
    }
  }
  wakeup(&pi->nread);
  release(&pi->lock);

  أرجع i;
}

صحيح
piperead(بنية pipe *pi, uint64 addr, صحيح n)
{
  صحيح i;
  بنية proc *pr = myproc();
  حرف ch;

  acquire(&pi->lock);
  طالما (pi->nread == pi->nwrite && pi->writeopen) { //DOC: pipe-empty
    𐡄𐡍 (killed(pr)) {
      release(&pi->lock);
      أرجع -1;
    }
    sleep_prepare(&pi->nread); //DOC: piperead-sleep
    release(&pi->lock);
    sleep();
    acquire(&pi->lock);
  }
  لكل (i = 0; i < n; i++) { //DOC: piperead-copy
    𐡄𐡍 (pi->nread == pi->nwrite)
      اكسر;
    ch = pi->data[pi->nread % PIPESIZE];
    𐡄𐡍 (copyout(pr->pagetable, pr->sz, addr + i, &ch, 1) == -1) {
      𐡄𐡍 (i == 0)
        i = -1;
      اكسر;
    }
    pi->nread++;
  }
  wakeup(&pi->nwrite); //DOC: piperead-wakeup
  release(&pi->lock);
  أرجع i;
}
