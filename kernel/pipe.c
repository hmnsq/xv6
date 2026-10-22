#ਅਨ੍ਤਰ੍ਭਾਵ "types.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "riscv.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "defs.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "param.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "spinlock.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "proc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "fs.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "sleeplock.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "file.h"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PIPESIZE 512

ਬਣਤਰ pipe {
  ਬਣਤਰ spinlock ਤਾਲਾ;
  ਅੱਖਰ ਅੰਕੜੇ[PIPESIZE];
  uint nread;    // number of bytes read
  uint nwrite;   // number of bytes written
  ਪੂਰਨ readopen;  // read fd is still open
  ਪੂਰਨ writeopen; // write fd is still open
};

ਪੂਰਨ
pipealloc(ਬਣਤਰ file **f0, ਬਣਤਰ file **f1)
{
  ਬਣਤਰ pipe *pi;

  pi = 0;
  *f0 = *f1 = 0;
  ਜੇ ((*f0 = filealloc()) == 0 || (*f1 = filealloc()) == 0)
    ਜਾਓ bad;
  ਜੇ ((pi = (ਬਣਤਰ pipe *)kalloc()) == 0)
    ਜਾਓ bad;
  pi->readopen = 1;
  pi->writeopen = 1;
  pi->nwrite = 0;
  pi->nread = 0;
  initlock(&pi->ਤਾਲਾ, "pipe");
  (*f0)->type = FD_PIPE;
  (*f0)->readable = 1;
  (*f0)->writable = 0;
  (*f0)->pipe = pi;
  (*f1)->type = FD_PIPE;
  (*f1)->readable = 0;
  (*f1)->writable = 1;
  (*f1)->pipe = pi;
  ਵਾਪਸ 0;

bad:
  ਜੇ (pi)
    kfree((ਅੱਖਰ *)pi);
  ਜੇ (*f0)
    fileclose(*f0);
  ਜੇ (*f1)
    fileclose(*f1);
  ਵਾਪਸ -1;
}

ਖਾਲੀ
pipeclose(ਬਣਤਰ pipe *pi, ਪੂਰਨ writable)
{
  acquire(&pi->ਤਾਲਾ);
  ਜੇ (writable) {
    pi->writeopen = 0;
    wakeup(&pi->nread);
  } ਨਹੀਂਤਾਂ {
    pi->readopen = 0;
    wakeup(&pi->nwrite);
  }
  ਜੇ (pi->readopen == 0 && pi->writeopen == 0) {
    release(&pi->ਤਾਲਾ);
    kfree((ਅੱਖਰ *)pi);
  } ਨਹੀਂਤਾਂ
    release(&pi->ਤਾਲਾ);
}

ਪੂਰਨ
pipewrite(ਬਣਤਰ pipe *pi, uint64 addr, ਪੂਰਨ n)
{
  ਪੂਰਨ i = 0;
  ਬਣਤਰ proc *pr = myproc();

  acquire(&pi->ਤਾਲਾ);
  ਜਦੋਂ (i < n) {
    ਜੇ (pi->readopen == 0 || killed(pr)) {
      release(&pi->ਤਾਲਾ);
      ਵਾਪਸ -1;
    }
    ਜੇ (pi->nwrite == pi->nread + PIPESIZE) { //DOC: pipewrite-full
      wakeup(&pi->nread);
      sleep_prepare(&pi->nwrite);
      release(&pi->ਤਾਲਾ);
      sleep();
      acquire(&pi->ਤਾਲਾ);
    } ਨਹੀਂਤਾਂ {
      ਅੱਖਰ ch;
      ਜੇ (copyin(pr->pagetable, pr->sz, &ch, addr + i, 1) == -1) {
        ਜੇ (i == 0)
          i = -1;
        ਰੋਕੋ;
      }
      pi->ਅੰਕੜੇ[pi->nwrite++ % PIPESIZE] = ch;
      i++;
    }
  }
  wakeup(&pi->nread);
  release(&pi->ਤਾਲਾ);

  ਵਾਪਸ i;
}

ਪੂਰਨ
piperead(ਬਣਤਰ pipe *pi, uint64 addr, ਪੂਰਨ n)
{
  ਪੂਰਨ i;
  ਬਣਤਰ proc *pr = myproc();
  ਅੱਖਰ ch;

  acquire(&pi->ਤਾਲਾ);
  ਜਦੋਂ (pi->nread == pi->nwrite && pi->writeopen) { //DOC: pipe-empty
    ਜੇ (killed(pr)) {
      release(&pi->ਤਾਲਾ);
      ਵਾਪਸ -1;
    }
    sleep_prepare(&pi->nread); //DOC: piperead-sleep
    release(&pi->ਤਾਲਾ);
    sleep();
    acquire(&pi->ਤਾਲਾ);
  }
  ਲਈ (i = 0; i < n; i++) { //DOC: piperead-copy
    ਜੇ (pi->nread == pi->nwrite)
      ਰੋਕੋ;
    ch = pi->ਅੰਕੜੇ[pi->nread % PIPESIZE];
    ਜੇ (copyout(pr->pagetable, pr->sz, addr + i, &ch, 1) == -1) {
      ਜੇ (i == 0)
        i = -1;
      ਰੋਕੋ;
    }
    pi->nread++;
  }
  wakeup(&pi->nwrite); //DOC: piperead-wakeup
  release(&pi->ਤਾਲਾ);
  ਵਾਪਸ i;
}
