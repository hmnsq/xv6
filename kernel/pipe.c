#अन्तर्भाव "types.h"
#अन्तर्भाव "riscv.h"
#अन्तर्भाव "defs.h"
#अन्तर्भाव "param.h"
#अन्तर्भाव "spinlock.h"
#अन्तर्भाव "proc.h"
#अन्तर्भाव "fs.h"
#अन्तर्भाव "sleeplock.h"
#अन्तर्भाव "file.h"

#परिभाषय PIPESIZE 512

संरचना pipe {
  संरचना spinlock तालकम्;
  वर्ण दत्त[PIPESIZE];
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
  initlock(&pi->तालकम्, "pipe");
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
  यदि (pi)
    kfree((वर्ण *)pi);
  यदि (*f0)
    fileclose(*f0);
  यदि (*f1)
    fileclose(*f1);
  प्रत्यागम -1;
}

शून्य
pipeclose(संरचना pipe *pi, पूर्णाङ्क writable)
{
  acquire(&pi->तालकम्);
  यदि (writable) {
    pi->writeopen = 0;
    wakeup(&pi->nread);
  } अन्यथा {
    pi->readopen = 0;
    wakeup(&pi->nwrite);
  }
  यदि (pi->readopen == 0 && pi->writeopen == 0) {
    release(&pi->तालकम्);
    kfree((वर्ण *)pi);
  } अन्यथा
    release(&pi->तालकम्);
}

पूर्णाङ्क
pipewrite(संरचना pipe *pi, uint64 addr, पूर्णाङ्क n)
{
  पूर्णाङ्क i = 0;
  संरचना proc *pr = myproc();

  acquire(&pi->तालकम्);
  यावत् (i < n) {
    यदि (pi->readopen == 0 || killed(pr)) {
      release(&pi->तालकम्);
      प्रत्यागम -1;
    }
    यदि (pi->nwrite == pi->nread + PIPESIZE) { //DOC: pipewrite-full
      wakeup(&pi->nread);
      sleep_prepare(&pi->nwrite);
      release(&pi->तालकम्);
      sleep();
      acquire(&pi->तालकम्);
    } अन्यथा {
      वर्ण ch;
      यदि (copyin(pr->pagetable, pr->sz, &ch, addr + i, 1) == -1) {
        यदि (i == 0)
          i = -1;
        भङ्ग;
      }
      pi->दत्त[pi->nwrite++ % PIPESIZE] = ch;
      i++;
    }
  }
  wakeup(&pi->nread);
  release(&pi->तालकम्);

  प्रत्यागम i;
}

पूर्णाङ्क
piperead(संरचना pipe *pi, uint64 addr, पूर्णाङ्क n)
{
  पूर्णाङ्क i;
  संरचना proc *pr = myproc();
  वर्ण ch;

  acquire(&pi->तालकम्);
  यावत् (pi->nread == pi->nwrite && pi->writeopen) { //DOC: pipe-empty
    यदि (killed(pr)) {
      release(&pi->तालकम्);
      प्रत्यागम -1;
    }
    sleep_prepare(&pi->nread); //DOC: piperead-sleep
    release(&pi->तालकम्);
    sleep();
    acquire(&pi->तालकम्);
  }
  प्रति (i = 0; i < n; i++) { //DOC: piperead-copy
    यदि (pi->nread == pi->nwrite)
      भङ्ग;
    ch = pi->दत्त[pi->nread % PIPESIZE];
    यदि (copyout(pr->pagetable, pr->sz, addr + i, &ch, 1) == -1) {
      यदि (i == 0)
        i = -1;
      भङ्ग;
    }
    pi->nread++;
  }
  wakeup(&pi->nwrite); //DOC: piperead-wakeup
  release(&pi->तालकम्);
  प्रत्यागम i;
}
