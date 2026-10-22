#অন্তর্ভাব "types.h"
#অন্তর্ভাব "riscv.h"
#অন্তর্ভাব "defs.h"
#অন্তর্ভাব "param.h"
#অন্তর্ভাব "spinlock.h"
#অন্তর্ভাব "proc.h"
#অন্তর্ভাব "fs.h"
#অন্তর্ভাব "sleeplock.h"
#অন্তর্ভাব "file.h"

#define PIPESIZE 512

কাঠামো pipe {
  কাঠামো spinlock lock;
  অক্ষর তথ্য[PIPESIZE];
  uint nread;    // number of bytes read
  uint nwrite;   // number of bytes written
  পূর্ণসংখ্যা readopen;  // read fd is still open
  পূর্ণসংখ্যা writeopen; // write fd is still open
};

পূর্ণসংখ্যা
pipealloc(কাঠামো file **f0, কাঠামো file **f1)
{
  কাঠামো pipe *pi;

  pi = 0;
  *f0 = *f1 = 0;
  যদি ((*f0 = filealloc()) == 0 || (*f1 = filealloc()) == 0)
    যাও bad;
  যদি ((pi = (কাঠামো pipe *)kalloc()) == 0)
    যাও bad;
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
  ফেরত 0;

bad:
  যদি (pi)
    kfree((অক্ষর *)pi);
  যদি (*f0)
    fileclose(*f0);
  যদি (*f1)
    fileclose(*f1);
  ফেরত -1;
}

শূন্য
pipeclose(কাঠামো pipe *pi, পূর্ণসংখ্যা writable)
{
  acquire(&pi->lock);
  যদি (writable) {
    pi->writeopen = 0;
    wakeup(&pi->nread);
  } অন্যথা {
    pi->readopen = 0;
    wakeup(&pi->nwrite);
  }
  যদি (pi->readopen == 0 && pi->writeopen == 0) {
    release(&pi->lock);
    kfree((অক্ষর *)pi);
  } অন্যথা
    release(&pi->lock);
}

পূর্ণসংখ্যা
pipewrite(কাঠামো pipe *pi, uint64 addr, পূর্ণসংখ্যা n)
{
  পূর্ণসংখ্যা i = 0;
  কাঠামো proc *pr = myproc();

  acquire(&pi->lock);
  যখন (i < n) {
    যদি (pi->readopen == 0 || killed(pr)) {
      release(&pi->lock);
      ফেরত -1;
    }
    যদি (pi->nwrite == pi->nread + PIPESIZE) { //DOC: pipewrite-full
      wakeup(&pi->nread);
      sleep_prepare(&pi->nwrite);
      release(&pi->lock);
      sleep();
      acquire(&pi->lock);
    } অন্যথা {
      অক্ষর ch;
      যদি (copyin(pr->pagetable, pr->sz, &ch, addr + i, 1) == -1) {
        যদি (i == 0)
          i = -1;
        বিরতি;
      }
      pi->তথ্য[pi->nwrite++ % PIPESIZE] = ch;
      i++;
    }
  }
  wakeup(&pi->nread);
  release(&pi->lock);

  ফেরত i;
}

পূর্ণসংখ্যা
piperead(কাঠামো pipe *pi, uint64 addr, পূর্ণসংখ্যা n)
{
  পূর্ণসংখ্যা i;
  কাঠামো proc *pr = myproc();
  অক্ষর ch;

  acquire(&pi->lock);
  যখন (pi->nread == pi->nwrite && pi->writeopen) { //DOC: pipe-empty
    যদি (killed(pr)) {
      release(&pi->lock);
      ফেরত -1;
    }
    sleep_prepare(&pi->nread); //DOC: piperead-sleep
    release(&pi->lock);
    sleep();
    acquire(&pi->lock);
  }
  জন্য (i = 0; i < n; i++) { //DOC: piperead-copy
    যদি (pi->nread == pi->nwrite)
      বিরতি;
    ch = pi->তথ্য[pi->nread % PIPESIZE];
    যদি (copyout(pr->pagetable, pr->sz, addr + i, &ch, 1) == -1) {
      যদি (i == 0)
        i = -1;
      বিরতি;
    }
    pi->nread++;
  }
  wakeup(&pi->nwrite); //DOC: piperead-wakeup
  release(&pi->lock);
  ফেরত i;
}
