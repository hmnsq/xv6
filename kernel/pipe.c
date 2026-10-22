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
  वर्ण data[PIPESIZE];
  uint nread;    // number of bytes read
  uint nwrite;   // number of bytes written
  पूर्णाङ्क readopen;  // read fd is still open
  पूर्णाङ्क writeopen; // write fd is still open
};

पूर्णाङ्क
pipealloc(संरचना file **f0, संरचना file **f1)
{
  संरचना pipe *pi;

  pi = 0;
  *f0 = *f1 = 0;
  सचे ((*f0 = filealloc()) == 0 || (*f1 = filealloc()) == 0)
    गच्छ bad;
  सचे ((pi = (संरचना pipe *)kalloc()) == 0)
    गच्छ bad;
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
  पटिनिवत्त 0;

bad:
  सचे (pi)
    kfree((वर्ण *)pi);
  सचे (*f0)
    fileclose(*f0);
  सचे (*f1)
    fileclose(*f1);
  पटिनिवत्त -1;
}

शून्य
pipeclose(संरचना pipe *pi, पूर्णाङ्क writable)
{
  acquire(&pi->lock);
  सचे (writable) {
    pi->writeopen = 0;
    wakeup(&pi->nread);
  } अञ्ञथा {
    pi->readopen = 0;
    wakeup(&pi->nwrite);
  }
  सचे (pi->readopen == 0 && pi->writeopen == 0) {
    release(&pi->lock);
    kfree((वर्ण *)pi);
  } अञ्ञथा
    release(&pi->lock);
}

पूर्णाङ्क
pipewrite(संरचना pipe *pi, uint64 addr, पूर्णाङ्क n)
{
  पूर्णाङ्क i = 0;
  संरचना proc *pr = myproc();

  acquire(&pi->lock);
  याव (i < n) {
    सचे (pi->readopen == 0 || killed(pr)) {
      release(&pi->lock);
      पटिनिवत्त -1;
    }
    सचे (pi->nwrite == pi->nread + PIPESIZE) { //DOC: pipewrite-full
      wakeup(&pi->nread);
      sleep_prepare(&pi->nwrite);
      release(&pi->lock);
      sleep();
      acquire(&pi->lock);
    } अञ्ञथा {
      वर्ण ch;
      सचे (copyin(pr->pagetable, pr->sz, &ch, addr + i, 1) == -1) {
        सचे (i == 0)
          i = -1;
        भङ्ग;
      }
      pi->data[pi->nwrite++ % PIPESIZE] = ch;
      i++;
    }
  }
  wakeup(&pi->nread);
  release(&pi->lock);

  पटिनिवत्त i;
}

पूर्णाङ्क
piperead(संरचना pipe *pi, uint64 addr, पूर्णाङ्क n)
{
  पूर्णाङ्क i;
  संरचना proc *pr = myproc();
  वर्ण ch;

  acquire(&pi->lock);
  याव (pi->nread == pi->nwrite && pi->writeopen) { //DOC: pipe-empty
    सचे (killed(pr)) {
      release(&pi->lock);
      पटिनिवत्त -1;
    }
    sleep_prepare(&pi->nread); //DOC: piperead-sleep
    release(&pi->lock);
    sleep();
    acquire(&pi->lock);
  }
  अत्थाय (i = 0; i < n; i++) { //DOC: piperead-copy
    सचे (pi->nread == pi->nwrite)
      भङ्ग;
    ch = pi->data[pi->nread % PIPESIZE];
    सचे (copyout(pr->pagetable, pr->sz, addr + i, &ch, 1) == -1) {
      सचे (i == 0)
        i = -1;
      भङ्ग;
    }
    pi->nread++;
  }
  wakeup(&pi->nwrite); //DOC: piperead-wakeup
  release(&pi->lock);
  पटिनिवत्त i;
}
