#அந்தர்பாவ "types.h"
#அந்தர்பாவ "riscv.h"
#அந்தர்பாவ "defs.h"
#அந்தர்பாவ "param.h"
#அந்தர்பாவ "spinlock.h"
#அந்தர்பாவ "proc.h"
#அந்தர்பாவ "fs.h"
#அந்தர்பாவ "sleeplock.h"
#அந்தர்பாவ "file.h"

#define PIPESIZE 512

கட்டமைப்பு pipe {
  கட்டமைப்பு spinlock lock;
  எழுத்து data[PIPESIZE];
  uint nread;    // number of bytes read
  uint nwrite;   // number of bytes written
  முழு readopen;  // read fd is still open
  முழு writeopen; // write fd is still open
};

முழு
pipealloc(கட்டமைப்பு file **f0, கட்டமைப்பு file **f1)
{
  கட்டமைப்பு pipe *pi;

  pi = 0;
  *f0 = *f1 = 0;
  எனில் ((*f0 = filealloc()) == 0 || (*f1 = filealloc()) == 0)
    செல் bad;
  எனில் ((pi = (கட்டமைப்பு pipe *)kalloc()) == 0)
    செல் bad;
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
  திரும்பு 0;

bad:
  எனில் (pi)
    kfree((எழுத்து *)pi);
  எனில் (*f0)
    fileclose(*f0);
  எனில் (*f1)
    fileclose(*f1);
  திரும்பு -1;
}

வெறுமை
pipeclose(கட்டமைப்பு pipe *pi, முழு writable)
{
  acquire(&pi->lock);
  எனில் (writable) {
    pi->writeopen = 0;
    wakeup(&pi->nread);
  } இல்லையெனில் {
    pi->readopen = 0;
    wakeup(&pi->nwrite);
  }
  எனில் (pi->readopen == 0 && pi->writeopen == 0) {
    release(&pi->lock);
    kfree((எழுத்து *)pi);
  } இல்லையெனில்
    release(&pi->lock);
}

முழு
pipewrite(கட்டமைப்பு pipe *pi, uint64 addr, முழு n)
{
  முழு i = 0;
  கட்டமைப்பு proc *pr = myproc();

  acquire(&pi->lock);
  போது (i < n) {
    எனில் (pi->readopen == 0 || killed(pr)) {
      release(&pi->lock);
      திரும்பு -1;
    }
    எனில் (pi->nwrite == pi->nread + PIPESIZE) { //DOC: pipewrite-full
      wakeup(&pi->nread);
      sleep_prepare(&pi->nwrite);
      release(&pi->lock);
      sleep();
      acquire(&pi->lock);
    } இல்லையெனில் {
      எழுத்து ch;
      எனில் (copyin(pr->pagetable, pr->sz, &ch, addr + i, 1) == -1) {
        எனில் (i == 0)
          i = -1;
        நிறுத்து;
      }
      pi->data[pi->nwrite++ % PIPESIZE] = ch;
      i++;
    }
  }
  wakeup(&pi->nread);
  release(&pi->lock);

  திரும்பு i;
}

முழு
piperead(கட்டமைப்பு pipe *pi, uint64 addr, முழு n)
{
  முழு i;
  கட்டமைப்பு proc *pr = myproc();
  எழுத்து ch;

  acquire(&pi->lock);
  போது (pi->nread == pi->nwrite && pi->writeopen) { //DOC: pipe-empty
    எனில் (killed(pr)) {
      release(&pi->lock);
      திரும்பு -1;
    }
    sleep_prepare(&pi->nread); //DOC: piperead-sleep
    release(&pi->lock);
    sleep();
    acquire(&pi->lock);
  }
  க்கு (i = 0; i < n; i++) { //DOC: piperead-copy
    எனில் (pi->nread == pi->nwrite)
      நிறுத்து;
    ch = pi->data[pi->nread % PIPESIZE];
    எனில் (copyout(pr->pagetable, pr->sz, addr + i, &ch, 1) == -1) {
      எனில் (i == 0)
        i = -1;
      நிறுத்து;
    }
    pi->nread++;
  }
  wakeup(&pi->nwrite); //DOC: piperead-wakeup
  release(&pi->lock);
  திரும்பு i;
}
