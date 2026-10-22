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
  यदि ((*f0 = filealloc()) == 0 || (*f1 = filealloc()) == 0)
    गच्छ bad;
  यदि ((pi = (संरचना pipe *)kalloc()) == 0)
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
  फर्काऊ 0;

bad:
  यदि (pi)
    kfree((वर्ण *)pi);
  यदि (*f0)
    fileclose(*f0);
  यदि (*f1)
    fileclose(*f1);
  फर्काऊ -1;
}

शून्य
pipeclose(संरचना pipe *pi, पूर्णाङ्क writable)
{
  acquire(&pi->lock);
  यदि (writable) {
    pi->writeopen = 0;
    wakeup(&pi->nread);
  } अन्यथा {
    pi->readopen = 0;
    wakeup(&pi->nwrite);
  }
  यदि (pi->readopen == 0 && pi->writeopen == 0) {
    release(&pi->lock);
    kfree((वर्ण *)pi);
  } अन्यथा
    release(&pi->lock);
}

पूर्णाङ्क
pipewrite(संरचना pipe *pi, uint64 addr, पूर्णाङ्क n)
{
  पूर्णाङ्क i = 0;
  संरचना proc *pr = myproc();

  acquire(&pi->lock);
  जबसम्म (i < n) {
    यदि (pi->readopen == 0 || killed(pr)) {
      release(&pi->lock);
      फर्काऊ -1;
    }
    यदि (pi->nwrite == pi->nread + PIPESIZE) { //DOC: pipewrite-full
      wakeup(&pi->nread);
      sleep_prepare(&pi->nwrite);
      release(&pi->lock);
      sleep();
      acquire(&pi->lock);
    } अन्यथा {
      वर्ण ch;
      यदि (copyin(pr->pagetable, pr->sz, &ch, addr + i, 1) == -1) {
        यदि (i == 0)
          i = -1;
        रोक;
      }
      pi->data[pi->nwrite++ % PIPESIZE] = ch;
      i++;
    }
  }
  wakeup(&pi->nread);
  release(&pi->lock);

  फर्काऊ i;
}

पूर्णाङ्क
piperead(संरचना pipe *pi, uint64 addr, पूर्णाङ्क n)
{
  पूर्णाङ्क i;
  संरचना proc *pr = myproc();
  वर्ण ch;

  acquire(&pi->lock);
  जबसम्म (pi->nread == pi->nwrite && pi->writeopen) { //DOC: pipe-empty
    यदि (killed(pr)) {
      release(&pi->lock);
      फर्काऊ -1;
    }
    sleep_prepare(&pi->nread); //DOC: piperead-sleep
    release(&pi->lock);
    sleep();
    acquire(&pi->lock);
  }
  लागि (i = 0; i < n; i++) { //DOC: piperead-copy
    यदि (pi->nread == pi->nwrite)
      रोक;
    ch = pi->data[pi->nread % PIPESIZE];
    यदि (copyout(pr->pagetable, pr->sz, addr + i, &ch, 1) == -1) {
      यदि (i == 0)
        i = -1;
      रोक;
    }
    pi->nread++;
  }
  wakeup(&pi->nwrite); //DOC: piperead-wakeup
  release(&pi->lock);
  फर्काऊ i;
}
