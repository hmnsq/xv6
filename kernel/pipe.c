#অন্তর্ভাৱ "types.h"
#অন্তর্ভাৱ "riscv.h"
#অন্তর্ভাৱ "defs.h"
#অন্তর্ভাৱ "param.h"
#অন্তর্ভাৱ "spinlock.h"
#অন্তর্ভাৱ "proc.h"
#অন্তর্ভাৱ "fs.h"
#অন্তর্ভাৱ "sleeplock.h"
#অন্তর্ভাৱ "file.h"

#define PIPESIZE 512

সংরচনা pipe {
  সংরচনা spinlock lock;
  ৱর্ণ data[PIPESIZE];
  uint nread;    // number of bytes read
  uint nwrite;   // number of bytes written
  পূর্ণাঙ্ক readopen;  // read fd is still open
  পূর্ণাঙ্ক writeopen; // write fd is still open
};

পূর্ণাঙ্ক
pipealloc(সংরচনা file **f0, সংরচনা file **f1)
{
  সংরচনা pipe *pi;

  pi = 0;
  *f0 = *f1 = 0;
  যদি ((*f0 = filealloc()) == 0 || (*f1 = filealloc()) == 0)
    গচ্ছ bad;
  যদি ((pi = (সংরচনা pipe *)kalloc()) == 0)
    গচ্ছ bad;
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
  ঘূৰাই 0;

bad:
  যদি (pi)
    kfree((ৱর্ণ *)pi);
  যদি (*f0)
    fileclose(*f0);
  যদি (*f1)
    fileclose(*f1);
  ঘূৰাই -1;
}

শূন্য
pipeclose(সংরচনা pipe *pi, পূর্ণাঙ্ক writable)
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
    kfree((ৱর্ণ *)pi);
  } অন্যথা
    release(&pi->lock);
}

পূর্ণাঙ্ক
pipewrite(সংরচনা pipe *pi, uint64 addr, পূর্ণাঙ্ক n)
{
  পূর্ণাঙ্ক i = 0;
  সংরচনা proc *pr = myproc();

  acquire(&pi->lock);
  যেতিয়ালৈ (i < n) {
    যদি (pi->readopen == 0 || killed(pr)) {
      release(&pi->lock);
      ঘূৰাই -1;
    }
    যদি (pi->nwrite == pi->nread + PIPESIZE) { //DOC: pipewrite-full
      wakeup(&pi->nread);
      sleep_prepare(&pi->nwrite);
      release(&pi->lock);
      sleep();
      acquire(&pi->lock);
    } অন্যথা {
      ৱর্ণ ch;
      যদি (copyin(pr->pagetable, pr->sz, &ch, addr + i, 1) == -1) {
        যদি (i == 0)
          i = -1;
        ভঙ্গ;
      }
      pi->data[pi->nwrite++ % PIPESIZE] = ch;
      i++;
    }
  }
  wakeup(&pi->nread);
  release(&pi->lock);

  ঘূৰাই i;
}

পূর্ণাঙ্ক
piperead(সংরচনা pipe *pi, uint64 addr, পূর্ণাঙ্ক n)
{
  পূর্ণাঙ্ক i;
  সংরচনা proc *pr = myproc();
  ৱর্ণ ch;

  acquire(&pi->lock);
  যেতিয়ালৈ (pi->nread == pi->nwrite && pi->writeopen) { //DOC: pipe-empty
    যদি (killed(pr)) {
      release(&pi->lock);
      ঘূৰাই -1;
    }
    sleep_prepare(&pi->nread); //DOC: piperead-sleep
    release(&pi->lock);
    sleep();
    acquire(&pi->lock);
  }
  বাবে (i = 0; i < n; i++) { //DOC: piperead-copy
    যদি (pi->nread == pi->nwrite)
      ভঙ্গ;
    ch = pi->data[pi->nread % PIPESIZE];
    যদি (copyout(pr->pagetable, pr->sz, addr + i, &ch, 1) == -1) {
      যদি (i == 0)
        i = -1;
      ভঙ্গ;
    }
    pi->nread++;
  }
  wakeup(&pi->nwrite); //DOC: piperead-wakeup
  release(&pi->lock);
  ঘূৰাই i;
}
