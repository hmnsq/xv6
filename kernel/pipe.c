#ଅନ୍ତର୍ଭାଵ "types.h"
#ଅନ୍ତର୍ଭାଵ "riscv.h"
#ଅନ୍ତର୍ଭାଵ "defs.h"
#ଅନ୍ତର୍ଭାଵ "param.h"
#ଅନ୍ତର୍ଭାଵ "spinlock.h"
#ଅନ୍ତର୍ଭାଵ "proc.h"
#ଅନ୍ତର୍ଭାଵ "fs.h"
#ଅନ୍ତର୍ଭାଵ "sleeplock.h"
#ଅନ୍ତର୍ଭାଵ "file.h"

#define PIPESIZE 512

ସଂରଚନା pipe {
  ସଂରଚନା spinlock lock;
  ଅକ୍ଷର data[PIPESIZE];
  uint nread;    // number of bytes read
  uint nwrite;   // number of bytes written
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା readopen;  // read fd is still open
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା writeopen; // write fd is still open
};

ପୂର୍ଣ୍ଣସଂଖ୍ୟା
pipealloc(ସଂରଚନା file **f0, ସଂରଚନା file **f1)
{
  ସଂରଚନା pipe *pi;

  pi = 0;
  *f0 = *f1 = 0;
  ଯଦି ((*f0 = filealloc()) == 0 || (*f1 = filealloc()) == 0)
    ଯାଅ bad;
  ଯଦି ((pi = (ସଂରଚନା pipe *)kalloc()) == 0)
    ଯାଅ bad;
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
  ଫେରନ୍ତୁ 0;

bad:
  ଯଦି (pi)
    kfree((ଅକ୍ଷର *)pi);
  ଯଦି (*f0)
    fileclose(*f0);
  ଯଦି (*f1)
    fileclose(*f1);
  ଫେରନ୍ତୁ -1;
}

ଶୂନ୍ୟ
pipeclose(ସଂରଚନା pipe *pi, ପୂର୍ଣ୍ଣସଂଖ୍ୟା writable)
{
  acquire(&pi->lock);
  ଯଦି (writable) {
    pi->writeopen = 0;
    wakeup(&pi->nread);
  } ଅନ୍ୟଥା {
    pi->readopen = 0;
    wakeup(&pi->nwrite);
  }
  ଯଦି (pi->readopen == 0 && pi->writeopen == 0) {
    release(&pi->lock);
    kfree((ଅକ୍ଷର *)pi);
  } ଅନ୍ୟଥା
    release(&pi->lock);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା
pipewrite(ସଂରଚନା pipe *pi, uint64 addr, ପୂର୍ଣ୍ଣସଂଖ୍ୟା n)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0;
  ସଂରଚନା proc *pr = myproc();

  acquire(&pi->lock);
  ଯେତେବେଳେ (i < n) {
    ଯଦି (pi->readopen == 0 || killed(pr)) {
      release(&pi->lock);
      ଫେରନ୍ତୁ -1;
    }
    ଯଦି (pi->nwrite == pi->nread + PIPESIZE) { //DOC: pipewrite-full
      wakeup(&pi->nread);
      sleep_prepare(&pi->nwrite);
      release(&pi->lock);
      sleep();
      acquire(&pi->lock);
    } ଅନ୍ୟଥା {
      ଅକ୍ଷର ch;
      ଯଦି (copyin(pr->pagetable, pr->sz, &ch, addr + i, 1) == -1) {
        ଯଦି (i == 0)
          i = -1;
        ଭାଙ୍ଗ;
      }
      pi->data[pi->nwrite++ % PIPESIZE] = ch;
      i++;
    }
  }
  wakeup(&pi->nread);
  release(&pi->lock);

  ଫେରନ୍ତୁ i;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା
piperead(ସଂରଚନା pipe *pi, uint64 addr, ପୂର୍ଣ୍ଣସଂଖ୍ୟା n)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
  ସଂରଚନା proc *pr = myproc();
  ଅକ୍ଷର ch;

  acquire(&pi->lock);
  ଯେତେବେଳେ (pi->nread == pi->nwrite && pi->writeopen) { //DOC: pipe-empty
    ଯଦି (killed(pr)) {
      release(&pi->lock);
      ଫେରନ୍ତୁ -1;
    }
    sleep_prepare(&pi->nread); //DOC: piperead-sleep
    release(&pi->lock);
    sleep();
    acquire(&pi->lock);
  }
  ପାଇଁ (i = 0; i < n; i++) { //DOC: piperead-copy
    ଯଦି (pi->nread == pi->nwrite)
      ଭାଙ୍ଗ;
    ch = pi->data[pi->nread % PIPESIZE];
    ଯଦି (copyout(pr->pagetable, pr->sz, addr + i, &ch, 1) == -1) {
      ଯଦି (i == 0)
        i = -1;
      ଭାଙ୍ଗ;
    }
    pi->nread++;
  }
  wakeup(&pi->nwrite); //DOC: piperead-wakeup
  release(&pi->lock);
  ଫେରନ୍ତୁ i;
}
