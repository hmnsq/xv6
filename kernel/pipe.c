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
  जइ ((*f0 = filealloc()) == 0 || (*f1 = filealloc()) == 0)
    गच्छ bad;
  जइ ((pi = (संरचना pipe *)kalloc()) == 0)
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
  प्रत्यागम 0;

bad:
  जइ (pi)
    kfree((वर्ण *)pi);
  जइ (*f0)
    fileclose(*f0);
  जइ (*f1)
    fileclose(*f1);
  प्रत्यागम -1;
}

शून्य
pipeclose(संरचना pipe *pi, पूर्णाङ्क writable)
{
  acquire(&pi->lock);
  जइ (writable) {
    pi->writeopen = 0;
    wakeup(&pi->nread);
  } अण्णहा {
    pi->readopen = 0;
    wakeup(&pi->nwrite);
  }
  जइ (pi->readopen == 0 && pi->writeopen == 0) {
    release(&pi->lock);
    kfree((वर्ण *)pi);
  } अण्णहा
    release(&pi->lock);
}

पूर्णाङ्क
pipewrite(संरचना pipe *pi, uint64 addr, पूर्णाङ्क n)
{
  पूर्णाङ्क i = 0;
  संरचना proc *pr = myproc();

  acquire(&pi->lock);
  जाव (i < n) {
    जइ (pi->readopen == 0 || killed(pr)) {
      release(&pi->lock);
      प्रत्यागम -1;
    }
    जइ (pi->nwrite == pi->nread + PIPESIZE) { //DOC: pipewrite-full
      wakeup(&pi->nread);
      sleep_prepare(&pi->nwrite);
      release(&pi->lock);
      sleep();
      acquire(&pi->lock);
    } अण्णहा {
      वर्ण ch;
      जइ (copyin(pr->pagetable, pr->sz, &ch, addr + i, 1) == -1) {
        जइ (i == 0)
          i = -1;
        भङ्ग;
      }
      pi->data[pi->nwrite++ % PIPESIZE] = ch;
      i++;
    }
  }
  wakeup(&pi->nread);
  release(&pi->lock);

  प्रत्यागम i;
}

पूर्णाङ्क
piperead(संरचना pipe *pi, uint64 addr, पूर्णाङ्क n)
{
  पूर्णाङ्क i;
  संरचना proc *pr = myproc();
  वर्ण ch;

  acquire(&pi->lock);
  जाव (pi->nread == pi->nwrite && pi->writeopen) { //DOC: pipe-empty
    जइ (killed(pr)) {
      release(&pi->lock);
      प्रत्यागम -1;
    }
    sleep_prepare(&pi->nread); //DOC: piperead-sleep
    release(&pi->lock);
    sleep();
    acquire(&pi->lock);
  }
  प्रति (i = 0; i < n; i++) { //DOC: piperead-copy
    जइ (pi->nread == pi->nwrite)
      भङ्ग;
    ch = pi->data[pi->nread % PIPESIZE];
    जइ (copyout(pr->pagetable, pr->sz, addr + i, &ch, 1) == -1) {
      जइ (i == 0)
        i = -1;
      भङ्ग;
    }
    pi->nread++;
  }
  wakeup(&pi->nwrite); //DOC: piperead-wakeup
  release(&pi->lock);
  प्रत्यागम i;
}
