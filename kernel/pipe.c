#ಅನ್ತರ್ಭಾವ "types.h"
#ಅನ್ತರ್ಭಾವ "riscv.h"
#ಅನ್ತರ್ಭಾವ "defs.h"
#ಅನ್ತರ್ಭಾವ "param.h"
#ಅನ್ತರ್ಭಾವ "spinlock.h"
#ಅನ್ತರ್ಭಾವ "proc.h"
#ಅನ್ತರ್ಭಾವ "fs.h"
#ಅನ್ತರ್ಭಾವ "sleeplock.h"
#ಅನ್ತರ್ಭಾವ "file.h"

#define PIPESIZE 512

ಸಂರಚನಾ pipe {
  ಸಂರಚನಾ spinlock lock;
  ವರ್ಣ data[PIPESIZE];
  uint nread;    // number of bytes read
  uint nwrite;   // number of bytes written
  ಪೂರ್ಣಾಙ್ಕ readopen;  // read fd is still open
  ಪೂರ್ಣಾಙ್ಕ writeopen; // write fd is still open
};

ಪೂರ್ಣಾಙ್ಕ
pipealloc(ಸಂರಚನಾ file **f0, ಸಂರಚನಾ file **f1)
{
  ಸಂರಚನಾ pipe *pi;

  pi = 0;
  *f0 = *f1 = 0;
  ಯದಿ ((*f0 = filealloc()) == 0 || (*f1 = filealloc()) == 0)
    ಗಚ್ಛ bad;
  ಯದಿ ((pi = (ಸಂರಚನಾ pipe *)kalloc()) == 0)
    ಗಚ್ಛ bad;
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
  ಪ್ರತ್ಯಾಗಮ 0;

bad:
  ಯದಿ (pi)
    kfree((ವರ್ಣ *)pi);
  ಯದಿ (*f0)
    fileclose(*f0);
  ಯದಿ (*f1)
    fileclose(*f1);
  ಪ್ರತ್ಯಾಗಮ -1;
}

ಶೂನ್ಯ
pipeclose(ಸಂರಚನಾ pipe *pi, ಪೂರ್ಣಾಙ್ಕ writable)
{
  acquire(&pi->lock);
  ಯದಿ (writable) {
    pi->writeopen = 0;
    wakeup(&pi->nread);
  } ಅನ್ಯಥಾ {
    pi->readopen = 0;
    wakeup(&pi->nwrite);
  }
  ಯದಿ (pi->readopen == 0 && pi->writeopen == 0) {
    release(&pi->lock);
    kfree((ವರ್ಣ *)pi);
  } ಅನ್ಯಥಾ
    release(&pi->lock);
}

ಪೂರ್ಣಾಙ್ಕ
pipewrite(ಸಂರಚನಾ pipe *pi, uint64 addr, ಪೂರ್ಣಾಙ್ಕ n)
{
  ಪೂರ್ಣಾಙ್ಕ i = 0;
  ಸಂರಚನಾ proc *pr = myproc();

  acquire(&pi->lock);
  ಯಾವತ್ (i < n) {
    ಯದಿ (pi->readopen == 0 || killed(pr)) {
      release(&pi->lock);
      ಪ್ರತ್ಯಾಗಮ -1;
    }
    ಯದಿ (pi->nwrite == pi->nread + PIPESIZE) { //DOC: pipewrite-full
      wakeup(&pi->nread);
      sleep_prepare(&pi->nwrite);
      release(&pi->lock);
      sleep();
      acquire(&pi->lock);
    } ಅನ್ಯಥಾ {
      ವರ್ಣ ch;
      ಯದಿ (copyin(pr->pagetable, pr->sz, &ch, addr + i, 1) == -1) {
        ಯದಿ (i == 0)
          i = -1;
        ಭಙ್ಗ;
      }
      pi->data[pi->nwrite++ % PIPESIZE] = ch;
      i++;
    }
  }
  wakeup(&pi->nread);
  release(&pi->lock);

  ಪ್ರತ್ಯಾಗಮ i;
}

ಪೂರ್ಣಾಙ್ಕ
piperead(ಸಂರಚನಾ pipe *pi, uint64 addr, ಪೂರ್ಣಾಙ್ಕ n)
{
  ಪೂರ್ಣಾಙ್ಕ i;
  ಸಂರಚನಾ proc *pr = myproc();
  ವರ್ಣ ch;

  acquire(&pi->lock);
  ಯಾವತ್ (pi->nread == pi->nwrite && pi->writeopen) { //DOC: pipe-empty
    ಯದಿ (killed(pr)) {
      release(&pi->lock);
      ಪ್ರತ್ಯಾಗಮ -1;
    }
    sleep_prepare(&pi->nread); //DOC: piperead-sleep
    release(&pi->lock);
    sleep();
    acquire(&pi->lock);
  }
  ಪ್ರತಿ (i = 0; i < n; i++) { //DOC: piperead-copy
    ಯದಿ (pi->nread == pi->nwrite)
      ಭಙ್ಗ;
    ch = pi->data[pi->nread % PIPESIZE];
    ಯದಿ (copyout(pr->pagetable, pr->sz, addr + i, &ch, 1) == -1) {
      ಯದಿ (i == 0)
        i = -1;
      ಭಙ್ಗ;
    }
    pi->nread++;
  }
  wakeup(&pi->nwrite); //DOC: piperead-wakeup
  release(&pi->lock);
  ಪ್ರತ್ಯಾಗಮ i;
}
