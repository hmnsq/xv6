#अन्तर्भाव "types.h"
#अन्तर्भाव "riscv.h"
#अन्तर्भाव "defs.h"
#अन्तर्भाव "param.h"
#अन्तर्भाव "spinlock.h"
#अन्तर्भाव "proc.h"
#अन्तर्भाव "fs.h"
#अन्तर्भाव "sleeplock.h"
#अन्तर्भाव "file.h"

#define PIPESIZE 512

संरचना pipe {
  संरचना spinlock lock;
  अक्षर data[PIPESIZE];
  uint nread;    // number of bytes read
  uint nwrite;   // number of bytes written
  पूर्णांक readopen;  // read fd is still open
  पूर्णांक writeopen; // write fd is still open
};

पूर्णांक
pipealloc(संरचना file **f0, संरचना file **f1)
{
  संरचना pipe *pi;

  pi = 0;
  *f0 = *f1 = 0;
  जर ((*f0 = filealloc()) == 0 || (*f1 = filealloc()) == 0)
    जा bad;
  जर ((pi = (संरचना pipe *)kalloc()) == 0)
    जा bad;
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
  परत 0;

bad:
  जर (pi)
    kfree((अक्षर *)pi);
  जर (*f0)
    fileclose(*f0);
  जर (*f1)
    fileclose(*f1);
  परत -1;
}

शून्य
pipeclose(संरचना pipe *pi, पूर्णांक writable)
{
  acquire(&pi->lock);
  जर (writable) {
    pi->writeopen = 0;
    wakeup(&pi->nread);
  } अन्यथा {
    pi->readopen = 0;
    wakeup(&pi->nwrite);
  }
  जर (pi->readopen == 0 && pi->writeopen == 0) {
    release(&pi->lock);
    kfree((अक्षर *)pi);
  } अन्यथा
    release(&pi->lock);
}

पूर्णांक
pipewrite(संरचना pipe *pi, uint64 addr, पूर्णांक n)
{
  पूर्णांक i = 0;
  संरचना proc *pr = myproc();

  acquire(&pi->lock);
  पर्यंत (i < n) {
    जर (pi->readopen == 0 || killed(pr)) {
      release(&pi->lock);
      परत -1;
    }
    जर (pi->nwrite == pi->nread + PIPESIZE) { //DOC: pipewrite-full
      wakeup(&pi->nread);
      sleep_prepare(&pi->nwrite);
      release(&pi->lock);
      sleep();
      acquire(&pi->lock);
    } अन्यथा {
      अक्षर ch;
      जर (copyin(pr->pagetable, pr->sz, &ch, addr + i, 1) == -1) {
        जर (i == 0)
          i = -1;
        थांबा;
      }
      pi->data[pi->nwrite++ % PIPESIZE] = ch;
      i++;
    }
  }
  wakeup(&pi->nread);
  release(&pi->lock);

  परत i;
}

पूर्णांक
piperead(संरचना pipe *pi, uint64 addr, पूर्णांक n)
{
  पूर्णांक i;
  संरचना proc *pr = myproc();
  अक्षर ch;

  acquire(&pi->lock);
  पर्यंत (pi->nread == pi->nwrite && pi->writeopen) { //DOC: pipe-empty
    जर (killed(pr)) {
      release(&pi->lock);
      परत -1;
    }
    sleep_prepare(&pi->nread); //DOC: piperead-sleep
    release(&pi->lock);
    sleep();
    acquire(&pi->lock);
  }
  साठी (i = 0; i < n; i++) { //DOC: piperead-copy
    जर (pi->nread == pi->nwrite)
      थांबा;
    ch = pi->data[pi->nread % PIPESIZE];
    जर (copyout(pr->pagetable, pr->sz, addr + i, &ch, 1) == -1) {
      जर (i == 0)
        i = -1;
      थांबा;
    }
    pi->nread++;
  }
  wakeup(&pi->nwrite); //DOC: piperead-wakeup
  release(&pi->lock);
  परत i;
}
