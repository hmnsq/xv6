#અન્તર્ભાવ "types.h"
#અન્તર્ભાવ "riscv.h"
#અન્તર્ભાવ "defs.h"
#અન્તર્ભાવ "param.h"
#અન્તર્ભાવ "spinlock.h"
#અન્તર્ભાવ "proc.h"
#અન્તર્ભાવ "fs.h"
#અન્તર્ભાવ "sleeplock.h"
#અન્તર્ભાવ "file.h"

#define PIPESIZE 512

સંરચના pipe {
  સંરચના spinlock lock;
  અક્ષર data[PIPESIZE];
  uint nread;    // number of bytes read
  uint nwrite;   // number of bytes written
  પૂર્ણાંક readopen;  // read fd is still open
  પૂર્ણાંક writeopen; // write fd is still open
};

પૂર્ણાંક
pipealloc(સંરચના file **f0, સંરચના file **f1)
{
  સંરચના pipe *pi;

  pi = 0;
  *f0 = *f1 = 0;
  જો ((*f0 = filealloc()) == 0 || (*f1 = filealloc()) == 0)
    જાઓ bad;
  જો ((pi = (સંરચના pipe *)kalloc()) == 0)
    જાઓ bad;
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
  પાછા 0;

bad:
  જો (pi)
    kfree((અક્ષર *)pi);
  જો (*f0)
    fileclose(*f0);
  જો (*f1)
    fileclose(*f1);
  પાછા -1;
}

શૂન્ય
pipeclose(સંરચના pipe *pi, પૂર્ણાંક writable)
{
  acquire(&pi->lock);
  જો (writable) {
    pi->writeopen = 0;
    wakeup(&pi->nread);
  } નહીંતર {
    pi->readopen = 0;
    wakeup(&pi->nwrite);
  }
  જો (pi->readopen == 0 && pi->writeopen == 0) {
    release(&pi->lock);
    kfree((અક્ષર *)pi);
  } નહીંતર
    release(&pi->lock);
}

પૂર્ણાંક
pipewrite(સંરચના pipe *pi, uint64 addr, પૂર્ણાંક n)
{
  પૂર્ણાંક i = 0;
  સંરચના proc *pr = myproc();

  acquire(&pi->lock);
  જ્યારે (i < n) {
    જો (pi->readopen == 0 || killed(pr)) {
      release(&pi->lock);
      પાછા -1;
    }
    જો (pi->nwrite == pi->nread + PIPESIZE) { //DOC: pipewrite-full
      wakeup(&pi->nread);
      sleep_prepare(&pi->nwrite);
      release(&pi->lock);
      sleep();
      acquire(&pi->lock);
    } નહીંતર {
      અક્ષર ch;
      જો (copyin(pr->pagetable, pr->sz, &ch, addr + i, 1) == -1) {
        જો (i == 0)
          i = -1;
        તોડો;
      }
      pi->data[pi->nwrite++ % PIPESIZE] = ch;
      i++;
    }
  }
  wakeup(&pi->nread);
  release(&pi->lock);

  પાછા i;
}

પૂર્ણાંક
piperead(સંરચના pipe *pi, uint64 addr, પૂર્ણાંક n)
{
  પૂર્ણાંક i;
  સંરચના proc *pr = myproc();
  અક્ષર ch;

  acquire(&pi->lock);
  જ્યારે (pi->nread == pi->nwrite && pi->writeopen) { //DOC: pipe-empty
    જો (killed(pr)) {
      release(&pi->lock);
      પાછા -1;
    }
    sleep_prepare(&pi->nread); //DOC: piperead-sleep
    release(&pi->lock);
    sleep();
    acquire(&pi->lock);
  }
  માટે (i = 0; i < n; i++) { //DOC: piperead-copy
    જો (pi->nread == pi->nwrite)
      તોડો;
    ch = pi->data[pi->nread % PIPESIZE];
    જો (copyout(pr->pagetable, pr->sz, addr + i, &ch, 1) == -1) {
      જો (i == 0)
        i = -1;
      તોડો;
    }
    pi->nread++;
  }
  wakeup(&pi->nwrite); //DOC: piperead-wakeup
  release(&pi->lock);
  પાછા i;
}
