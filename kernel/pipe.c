#അന്തര്ഭാവ "types.h"
#അന്തര്ഭാവ "riscv.h"
#അന്തര്ഭാവ "defs.h"
#അന്തര്ഭാവ "param.h"
#അന്തര്ഭാവ "spinlock.h"
#അന്തര്ഭാവ "proc.h"
#അന്തര്ഭാവ "fs.h"
#അന്തര്ഭാവ "sleeplock.h"
#അന്തര്ഭാവ "file.h"

#define PIPESIZE 512

ഘടന pipe {
  ഘടന spinlock lock;
  അക്ഷരം data[PIPESIZE];
  uint nread;    // number of bytes read
  uint nwrite;   // number of bytes written
  പൂർണ്ണസംഖ്യ readopen;  // read fd is still open
  പൂർണ്ണസംഖ്യ writeopen; // write fd is still open
};

പൂർണ്ണസംഖ്യ
pipealloc(ഘടന file **f0, ഘടന file **f1)
{
  ഘടന pipe *pi;

  pi = 0;
  *f0 = *f1 = 0;
  എങ്കിൽ ((*f0 = filealloc()) == 0 || (*f1 = filealloc()) == 0)
    പോകുക bad;
  എങ്കിൽ ((pi = (ഘടന pipe *)kalloc()) == 0)
    പോകുക bad;
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
  തിരികെ 0;

bad:
  എങ്കിൽ (pi)
    kfree((അക്ഷരം *)pi);
  എങ്കിൽ (*f0)
    fileclose(*f0);
  എങ്കിൽ (*f1)
    fileclose(*f1);
  തിരികെ -1;
}

ശൂന്യം
pipeclose(ഘടന pipe *pi, പൂർണ്ണസംഖ്യ writable)
{
  acquire(&pi->lock);
  എങ്കിൽ (writable) {
    pi->writeopen = 0;
    wakeup(&pi->nread);
  } അല്ലെങ്കിൽ {
    pi->readopen = 0;
    wakeup(&pi->nwrite);
  }
  എങ്കിൽ (pi->readopen == 0 && pi->writeopen == 0) {
    release(&pi->lock);
    kfree((അക്ഷരം *)pi);
  } അല്ലെങ്കിൽ
    release(&pi->lock);
}

പൂർണ്ണസംഖ്യ
pipewrite(ഘടന pipe *pi, uint64 addr, പൂർണ്ണസംഖ്യ n)
{
  പൂർണ്ണസംഖ്യ i = 0;
  ഘടന proc *pr = myproc();

  acquire(&pi->lock);
  ആകുമ്പോൾ (i < n) {
    എങ്കിൽ (pi->readopen == 0 || killed(pr)) {
      release(&pi->lock);
      തിരികെ -1;
    }
    എങ്കിൽ (pi->nwrite == pi->nread + PIPESIZE) { //DOC: pipewrite-full
      wakeup(&pi->nread);
      sleep_prepare(&pi->nwrite);
      release(&pi->lock);
      sleep();
      acquire(&pi->lock);
    } അല്ലെങ്കിൽ {
      അക്ഷരം ch;
      എങ്കിൽ (copyin(pr->pagetable, pr->sz, &ch, addr + i, 1) == -1) {
        എങ്കിൽ (i == 0)
          i = -1;
        നിർത്തുക;
      }
      pi->data[pi->nwrite++ % PIPESIZE] = ch;
      i++;
    }
  }
  wakeup(&pi->nread);
  release(&pi->lock);

  തിരികെ i;
}

പൂർണ്ണസംഖ്യ
piperead(ഘടന pipe *pi, uint64 addr, പൂർണ്ണസംഖ്യ n)
{
  പൂർണ്ണസംഖ്യ i;
  ഘടന proc *pr = myproc();
  അക്ഷരം ch;

  acquire(&pi->lock);
  ആകുമ്പോൾ (pi->nread == pi->nwrite && pi->writeopen) { //DOC: pipe-empty
    എങ്കിൽ (killed(pr)) {
      release(&pi->lock);
      തിരികെ -1;
    }
    sleep_prepare(&pi->nread); //DOC: piperead-sleep
    release(&pi->lock);
    sleep();
    acquire(&pi->lock);
  }
  വേണ്ടി (i = 0; i < n; i++) { //DOC: piperead-copy
    എങ്കിൽ (pi->nread == pi->nwrite)
      നിർത്തുക;
    ch = pi->data[pi->nread % PIPESIZE];
    എങ്കിൽ (copyout(pr->pagetable, pr->sz, addr + i, &ch, 1) == -1) {
      എങ്കിൽ (i == 0)
        i = -1;
      നിർത്തുക;
    }
    pi->nread++;
  }
  wakeup(&pi->nwrite); //DOC: piperead-wakeup
  release(&pi->lock);
  തിരികെ i;
}
