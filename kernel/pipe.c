#అన్తర్భావ "types.h"
#అన్తర్భావ "riscv.h"
#అన్తర్భావ "defs.h"
#అన్తర్భావ "param.h"
#అన్తర్భావ "spinlock.h"
#అన్తర్భావ "proc.h"
#అన్తర్భావ "fs.h"
#అన్తర్భావ "sleeplock.h"
#అన్తర్భావ "file.h"

#నిర్వచించు PIPESIZE 512

నిర్మాణం pipe {
  నిర్మాణం spinlock తాళం;
  అక్షరం దత్తాంశం[PIPESIZE];
  uint nread;    // number of bytes read
  uint nwrite;   // number of bytes written
  పూర్ణాంకం readopen;  // read fd is still open
  పూర్ణాంకం writeopen; // write fd is still open
};

పూర్ణాంకం
pipealloc(నిర్మాణం file **f0, నిర్మాణం file **f1)
{
  నిర్మాణం pipe *pi;

  pi = 0;
  *f0 = *f1 = 0;
  ఒకవేళ ((*f0 = filealloc()) == 0 || (*f1 = filealloc()) == 0)
    వెళ్ళు bad;
  ఒకవేళ ((pi = (నిర్మాణం pipe *)kalloc()) == 0)
    వెళ్ళు bad;
  pi->readopen = 1;
  pi->writeopen = 1;
  pi->nwrite = 0;
  pi->nread = 0;
  initlock(&pi->తాళం, "pipe");
  (*f0)->type = FD_PIPE;
  (*f0)->readable = 1;
  (*f0)->writable = 0;
  (*f0)->pipe = pi;
  (*f1)->type = FD_PIPE;
  (*f1)->readable = 0;
  (*f1)->writable = 1;
  (*f1)->pipe = pi;
  తిరిగి 0;

bad:
  ఒకవేళ (pi)
    kfree((అక్షరం *)pi);
  ఒకవేళ (*f0)
    fileclose(*f0);
  ఒకవేళ (*f1)
    fileclose(*f1);
  తిరిగి -1;
}

శూన్య
pipeclose(నిర్మాణం pipe *pi, పూర్ణాంకం writable)
{
  acquire(&pi->తాళం);
  ఒకవేళ (writable) {
    pi->writeopen = 0;
    wakeup(&pi->nread);
  } లేకపోతే {
    pi->readopen = 0;
    wakeup(&pi->nwrite);
  }
  ఒకవేళ (pi->readopen == 0 && pi->writeopen == 0) {
    release(&pi->తాళం);
    kfree((అక్షరం *)pi);
  } లేకపోతే
    release(&pi->తాళం);
}

పూర్ణాంకం
pipewrite(నిర్మాణం pipe *pi, uint64 addr, పూర్ణాంకం n)
{
  పూర్ణాంకం i = 0;
  నిర్మాణం proc *pr = myproc();

  acquire(&pi->తాళం);
  అయితే (i < n) {
    ఒకవేళ (pi->readopen == 0 || killed(pr)) {
      release(&pi->తాళం);
      తిరిగి -1;
    }
    ఒకవేళ (pi->nwrite == pi->nread + PIPESIZE) { //DOC: pipewrite-full
      wakeup(&pi->nread);
      sleep_prepare(&pi->nwrite);
      release(&pi->తాళం);
      sleep();
      acquire(&pi->తాళం);
    } లేకపోతే {
      అక్షరం ch;
      ఒకవేళ (copyin(pr->pagetable, pr->sz, &ch, addr + i, 1) == -1) {
        ఒకవేళ (i == 0)
          i = -1;
        ఆపు;
      }
      pi->దత్తాంశం[pi->nwrite++ % PIPESIZE] = ch;
      i++;
    }
  }
  wakeup(&pi->nread);
  release(&pi->తాళం);

  తిరిగి i;
}

పూర్ణాంకం
piperead(నిర్మాణం pipe *pi, uint64 addr, పూర్ణాంకం n)
{
  పూర్ణాంకం i;
  నిర్మాణం proc *pr = myproc();
  అక్షరం ch;

  acquire(&pi->తాళం);
  అయితే (pi->nread == pi->nwrite && pi->writeopen) { //DOC: pipe-empty
    ఒకవేళ (killed(pr)) {
      release(&pi->తాళం);
      తిరిగి -1;
    }
    sleep_prepare(&pi->nread); //DOC: piperead-sleep
    release(&pi->తాళం);
    sleep();
    acquire(&pi->తాళం);
  }
  కొరకు (i = 0; i < n; i++) { //DOC: piperead-copy
    ఒకవేళ (pi->nread == pi->nwrite)
      ఆపు;
    ch = pi->దత్తాంశం[pi->nread % PIPESIZE];
    ఒకవేళ (copyout(pr->pagetable, pr->sz, addr + i, &ch, 1) == -1) {
      ఒకవేళ (i == 0)
        i = -1;
      ఆపు;
    }
    pi->nread++;
  }
  wakeup(&pi->nwrite); //DOC: piperead-wakeup
  release(&pi->తాళం);
  తిరిగి i;
}
